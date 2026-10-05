#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>

using namespace std;

double getRAMUsage()
{
    ifstream file("/proc/meminfo");
    string line;
    long total = 0, available = 0;

    while (getline(file, line))
    {
        if (line.find("MemTotal:") == 0)
            sscanf(line.c_str(), "MemTotal: %ld", &total);

        if (line.find("MemAvailable:") == 0)
            sscanf(line.c_str(), "MemAvailable: %ld", &available);
    }

    if (total == 0)
        return 0;

    return ((double)(total - available) / total) * 100.0;
}

double getDiskUsage()
{
    FILE* pipe = popen("df / | tail -1 | awk '{print $5}'", "r");

    if (!pipe)
        return 0;

    char buffer[32];

    if (fgets(buffer, sizeof(buffer), pipe) == nullptr)
    {
        pclose(pipe);
        return 0;
    }

    pclose(pipe);

    return stod(string(buffer).substr(0, string(buffer).find('%')));
}

void logAlert(const string& message)
{
    ofstream file("lens_alerts.log", ios::app);

    time_t now = time(nullptr);
    char* timeStr = ctime(&now);

    if (timeStr)
        file << "[" << string(timeStr).substr(0, 24) << "] "
             << message << "\n";
}

int main()
{
    cout << "\n========================================\n";
    cout << "      LENS SECURITY ALERT ENGINE\n";
    cout << "========================================\n";

    double ram = getRAMUsage();
    double disk = getDiskUsage();

    bool alertFound = false;

    cout << "RAM Usage    : " << ram << "%\n";
    cout << "Disk Usage   : " << disk << "%\n";
    cout << "----------------------------------------\n";

    if (ram >= 90)
    {
        cout << "[CRITICAL] High RAM usage detected!\n";
        logAlert("CRITICAL: High RAM usage detected: " +
                 to_string(ram) + "%");
        alertFound = true;
    }
    else if (ram >= 75)
    {
        cout << "[WARNING] High RAM usage detected!\n";
        logAlert("WARNING: High RAM usage detected: " +
                 to_string(ram) + "%");
        alertFound = true;
    }

    if (disk >= 90)
    {
        cout << "[CRITICAL] Disk space critically high!\n";
        logAlert("CRITICAL: Disk usage detected: " +
                 to_string(disk) + "%");
        alertFound = true;
    }
    else if (disk >= 75)
    {
        cout << "[WARNING] Disk usage is high!\n";
        logAlert("WARNING: Disk usage detected: " +
                 to_string(disk) + "%");
        alertFound = true;
    }

    if (!alertFound)
    {
        cout << "[NORMAL] No security alerts detected.\n";
    }

    cout << "----------------------------------------\n";
    cout << "Alerts logged to: lens_alerts.log\n";
    cout << "========================================\n";

    return 0;
}

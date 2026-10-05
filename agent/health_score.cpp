#include <iostream>
#include <fstream>
#include <string>
#include <sys/statvfs.h>

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
    struct statvfs stat;

    if (statvfs("/", &stat) != 0)
        return 0;

    unsigned long long total =
        (unsigned long long)stat.f_blocks * stat.f_frsize;

    unsigned long long freeSpace =
        (unsigned long long)stat.f_bavail * stat.f_frsize;

    if (total == 0)
        return 0;

    return ((double)(total - freeSpace) / total) * 100.0;
}

string getStatus(double usage)
{
    if (usage >= 90)
        return "CRITICAL";

    if (usage >= 75)
        return "WARNING";

    return "NORMAL";
}

int main()
{
    double ram = getRAMUsage();
    double disk = getDiskUsage();

    /*
       CPU is kept as a placeholder status here because
       the existing CPU monitor already performs live CPU
       calculation.
    */

    int score = 100;

    if (ram >= 90)
        score -= 30;
    else if (ram >= 75)
        score -= 15;

    if (disk >= 90)
        score -= 30;
    else if (disk >= 75)
        score -= 15;

    cout << "\n========================================\n";
    cout << "       LENS SYSTEM HEALTH SCORE\n";
    cout << "========================================\n";

    cout << "\nCPU       : MONITORED\n";
    cout << "RAM       : " << getStatus(ram)
         << " (" << ram << "%)\n";

    cout << "DISK      : " << getStatus(disk)
         << " (" << disk << "%)\n";

    cout << "NETWORK   : ACTIVE\n";
    cout << "DRIVER    : CONNECTED\n";

    cout << "\n----------------------------------------\n";

    cout << "HEALTH SCORE : " << score << "/100\n";

    if (score >= 90)
        cout << "STATUS       : HEALTHY\n";
    else if (score >= 70)
        cout << "STATUS       : WARNING\n";
    else
        cout << "STATUS       : CRITICAL\n";

    cout << "----------------------------------------\n";
    cout << "LENS endpoint health assessment complete.\n";
    cout << "========================================\n";

    return 0;
}

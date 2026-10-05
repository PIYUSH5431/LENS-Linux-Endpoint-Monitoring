#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <dirent.h>
#include <unistd.h>
#include <algorithm>

using namespace std;

struct ProcessInfo
{
    int pid;
    string name;
    double cpu;
    double ram;
};

bool isNumber(const string& str)
{
    if (str.empty())
        return false;

    for (char c : str)
    {
        if (!isdigit(c))
            return false;
    }

    return true;
}

string getProcessName(int pid)
{
    string path = "/proc/" + to_string(pid) + "/comm";
    ifstream file(path);

    string name;
    getline(file, name);

    return name;
}

double getProcessRAM(int pid)
{
    string path = "/proc/" + to_string(pid) + "/status";
    ifstream file(path);

    string line;

    long rss = 0;
    long totalRAM = 0;

    while (getline(file, line))
    {
        if (line.find("VmRSS:") == 0)
        {
            sscanf(line.c_str(), "VmRSS: %ld", &rss);
        }
    }

    ifstream meminfo("/proc/meminfo");

    while (getline(meminfo, line))
    {
        if (line.find("MemTotal:") == 0)
        {
            sscanf(line.c_str(), "MemTotal: %ld", &totalRAM);
            break;
        }
    }

    if (totalRAM == 0)
        return 0;

    return ((double)rss / totalRAM) * 100.0;
}

int main()
{
    cout << "\n========================================\n";
    cout << "       LENS PROCESS SECURITY SCAN\n";
    cout << "========================================\n";

    DIR* dir = opendir("/proc");

    if (!dir)
    {
        cerr << "ERROR: Cannot access /proc\n";
        return 1;
    }

    vector<ProcessInfo> suspicious;

    struct dirent* entry;

    int scanned = 0;

    while ((entry = readdir(dir)) != nullptr)
    {
        string pidString = entry->d_name;

        if (!isNumber(pidString))
            continue;

        int pid = stoi(pidString);

        string name = getProcessName(pid);

        if (name.empty())
            continue;

        double ram = getProcessRAM(pid);

        scanned++;

        // Security rule:
        // Flag processes using more than 20% of total RAM.
        if (ram >= 20.0)
        {
            ProcessInfo process;

            process.pid = pid;
            process.name = name;
            process.cpu = 0.0;
            process.ram = ram;

            suspicious.push_back(process);
        }
    }

    closedir(dir);

    cout << "\nProcesses Scanned : " << scanned << "\n";
    cout << "RAM Threshold     : 20%\n";
    cout << "----------------------------------------\n";

    if (suspicious.empty())
    {
        cout << "[OK] No high-resource suspicious processes found.\n";
        cout << "\nSecurity Status   : SAFE\n";
    }
    else
    {
        cout << "[ALERT] High-resource processes detected!\n\n";

        for (const auto& process : suspicious)
        {
            cout << "PID      : " << process.pid << "\n";
            cout << "Process  : " << process.name << "\n";
            cout << "RAM      : " << process.ram << "%\n";
            cout << "Severity : HIGH\n";
            cout << "----------------------------------------\n";
        }

        cout << "Security Status   : WARNING\n";
    }

    cout << "\n========================================\n";
    cout << "      SECURITY SCAN COMPLETED\n";
    cout << "========================================\n";

    return 0;
}

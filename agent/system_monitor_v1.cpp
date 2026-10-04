#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/statvfs.h>

struct CPUStats {
    unsigned long long idle;
    unsigned long long total;
};

CPUStats readCPUStats() {
    std::ifstream file("/proc/stat");
    std::string cpu;

    unsigned long long user, nice, system, idle;
    unsigned long long iowait, irq, softirq, steal;

    file >> cpu >> user >> nice >> system >> idle
         >> iowait >> irq >> softirq >> steal;

    unsigned long long total =
        user + nice + system + idle +
        iowait + irq + softirq + steal;

    unsigned long long idleTotal = idle + iowait;

    return {idleTotal, total};
}

double getCPUUsage() {
    CPUStats first = readCPUStats();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    CPUStats second = readCPUStats();

    unsigned long long totalDiff =
        second.total - first.total;

    unsigned long long idleDiff =
        second.idle - first.idle;

    if (totalDiff == 0)
        return 0.0;

    return 100.0 *
           (1.0 - static_cast<double>(idleDiff) / totalDiff);
}

double getRAMUsage() {
    std::ifstream file("/proc/meminfo");

    std::string label;
    unsigned long long value;
    std::string unit;

    unsigned long long totalRAM = 0;
    unsigned long long availableRAM = 0;

    while (file >> label >> value >> unit) {
        if (label == "MemTotal:")
            totalRAM = value;

        else if (label == "MemAvailable:")
            availableRAM = value;
    }

    if (totalRAM == 0)
        return 0.0;

    return 100.0 *
           (1.0 - static_cast<double>(availableRAM) / totalRAM);
}

double getDiskUsage() {
    struct statvfs disk;

    if (statvfs("/", &disk) != 0)
        return 0.0;

    unsigned long long totalBytes =
        static_cast<unsigned long long>(disk.f_blocks) * disk.f_frsize;

    unsigned long long freeBytes =
        static_cast<unsigned long long>(disk.f_bavail) * disk.f_frsize;

    unsigned long long usedBytes =
        totalBytes - freeBytes;

    if (totalBytes == 0)
        return 0.0;

    return 100.0 *
           static_cast<double>(usedBytes) / totalBytes;
}
double getUptime() {
    std::ifstream file("/proc/uptime");

    double uptimeSeconds;

    if (!(file >> uptimeSeconds))
        return 0.0;

    return uptimeSeconds;
}

std::string getStatus(double value) {
    if (value >= 90.0)
        return "CRITICAL";

    if (value >= 75.0)
        return "WARNING";

    return "NORMAL";
}

int main() {

    double cpu = getCPUUsage();
    double ram = getRAMUsage();
    double disk = getDiskUsage();
    double uptime = getUptime();
    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "           LENS SYSTEM MONITOR\n";
    std::cout << "============================================\n\n";

    std::cout << " CPU Usage       : "
              << cpu << "%    ["
              << getStatus(cpu) << "]\n";

    std::cout << " RAM Usage       : "
              << ram << "%    ["
              << getStatus(ram) << "]\n";

    std::cout << " Disk Usage      : "
              << disk << "%    ["
              << getStatus(disk) << "]\n";

    std::cout << "\n--------------------------------------------\n";
    int hours = static_cast<int>(uptime) / 3600;
int minutes = (static_cast<int>(uptime) % 3600) / 60;

std::cout << " Uptime          : "
          << hours << "h "
          << minutes << "m\n";

    if (cpu >= 90 || ram >= 90 || disk >= 90)
        std::cout << " System Status   : CRITICAL\n";
    else if (cpu >= 75 || ram >= 75 || disk >= 75)
        std::cout << " System Status   : WARNING\n";
    else
        std::cout << " System Status   : NORMAL\n";

    std::cout << "============================================\n\n";

     
    return 0;
}

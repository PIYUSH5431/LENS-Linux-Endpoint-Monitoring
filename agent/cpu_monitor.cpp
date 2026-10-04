#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

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

double calculateCPUUsage(CPUStats first, CPUStats second) {
    unsigned long long totalDiff = second.total - first.total;
    unsigned long long idleDiff = second.idle - first.idle;

    if (totalDiff == 0)
        return 0.0;

    return 100.0 * (1.0 - 
        static_cast<double>(idleDiff) / totalDiff);
}

int main() {
    std::cout << "LENS CPU MONITOR\n";
    std::cout << "================\n";

    CPUStats first = readCPUStats();

    std::this_thread::sleep_for(std::chrono::seconds(1));

    CPUStats second = readCPUStats();

    double usage = calculateCPUUsage(first, second);

    std::cout << "CPU Usage: " << usage << "%\n";

    return 0;
}

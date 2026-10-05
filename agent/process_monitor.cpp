#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

struct ProcessInfo {
    int pid;
    std::string name;
    unsigned long long memoryKB;
    unsigned long long cpuTime;
};

bool isNumber(const std::string& s) {
    if (s.empty()) return false;

    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }

    return true;
}

bool readProcess(ProcessInfo& p) {

    std::string statPath =
        "/proc/" + std::to_string(p.pid) + "/stat";

    std::ifstream file(statPath);

    if (!file)
        return false;

    std::string line;
    std::getline(file, line);

    size_t open = line.find('(');
    size_t close = line.rfind(')');

    if (open == std::string::npos ||
        close == std::string::npos)
        return false;

    p.name = line.substr(open + 1, close - open - 1);

    std::string rest = line.substr(close + 2);

    std::istringstream ss(rest);

    char state;
    unsigned long long value;

    ss >> state;

    // Fields after state:
    // ppid, pgrp, session, tty_nr, tpgid,
    // flags, minflt, cminflt, majflt, cmajflt,
    // utime, stime

    for (int i = 0; i < 10; i++)
        ss >> value;

    unsigned long long utime, stime;

    ss >> utime >> stime;

    p.cpuTime = utime + stime;

    return true;
}

unsigned long long getTotalCPUTime() {

    std::ifstream file("/proc/stat");

    std::string cpu;

    unsigned long long user, nice, system;
    unsigned long long idle, iowait, irq, softirq, steal;

    file >> cpu
         >> user
         >> nice
         >> system
         >> idle
         >> iowait
         >> irq
         >> softirq
         >> steal;

    return user + nice + system + idle +
           iowait + irq + softirq + steal;
}

unsigned long long getTotalRAM() {

    std::ifstream file("/proc/meminfo");

    std::string key;
    unsigned long long value;
    std::string unit;

    while (file >> key >> value >> unit) {

        if (key == "MemTotal:")
            return value;
    }

    return 0;
}

unsigned long long getProcessRAM(int pid) {

    std::ifstream file(
        "/proc/" + std::to_string(pid) + "/status"
    );

    std::string key;
    unsigned long long value;
    std::string unit;

    while (file >> key >> value >> unit) {

        if (key == "VmRSS:")
            return value;
    }

    return 0;
}

int main() {

    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "          LENS PROCESS MONITOR\n";
    std::cout << "       CPU + MEMORY ANALYSIS\n";
    std::cout << "============================================\n";

    unsigned long long totalCPU1 =
        getTotalCPUTime();

    std::vector<ProcessInfo> processes;

    for (const auto& entry :
         fs::directory_iterator("/proc")) {

        std::string pidStr =
            entry.path().filename().string();

        if (!isNumber(pidStr))
            continue;

        ProcessInfo p;

        try {
            p.pid = std::stoi(pidStr);
        }
        catch (...) {
            continue;
        }

        if (readProcess(p)) {

            p.memoryKB =
                getProcessRAM(p.pid);

            processes.push_back(p);
        }
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(500)
    );

    unsigned long long totalCPU2 =
        getTotalCPUTime();

    unsigned long long totalCPUDelta =
        totalCPU2 - totalCPU1;

    for (auto& p : processes) {

        ProcessInfo second;

        second.pid = p.pid;

        if (!readProcess(second)) {
            p.cpuTime = 0;
            continue;
        }

        unsigned long long processDelta =
            second.cpuTime - p.cpuTime;

        if (totalCPUDelta > 0) {

            p.cpuTime =
                (processDelta * 100.0) /
                totalCPUDelta;
        }
        else {
            p.cpuTime = 0;
        }
    }

    unsigned long long totalRAM =
        getTotalRAM();

    std::sort(
        processes.begin(),
        processes.end(),
        [](const ProcessInfo& a,
           const ProcessInfo& b) {

            return a.cpuTime > b.cpuTime;
        }
    );

    std::cout << "\n";
    std::cout << "PID\tCPU%\tRAM%\tPROCESS\n";
    std::cout << "--------------------------------------------\n";

    int count = 0;

    for (const auto& p : processes) {

        double ramPercent = 0;

        if (totalRAM > 0) {

            ramPercent =
                (p.memoryKB * 100.0) /
                totalRAM;
        }

        std::cout << p.pid
                  << "\t"
                  << p.cpuTime
                  << "\t"
                  << ramPercent
                  << "\t"
                  << p.name
                  << "\n";

        count++;

        if (count >= 20)
            break;
    }

    std::cout << "\n--------------------------------------------\n";

    std::cout << "Total Processes: "
              << processes.size()
              << "\n";

    std::cout << "Showing Top 20 CPU Processes\n";

    std::cout << "============================================\n";

    return 0;
}

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

namespace fs = std::filesystem;

struct ProcessInfo {
    int pid;
    std::string name;
    char state;
    unsigned long long memoryKB;
};

bool isNumber(const std::string& str) {
    if (str.empty())
        return false;

    for (char c : str) {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }

    return true;
}

unsigned long long getTotalRAMKB() {
    std::ifstream file("/proc/meminfo");

    std::string label;
    unsigned long long value;
    std::string unit;

    while (file >> label >> value >> unit) {
        if (label == "MemTotal:")
            return value;
    }

    return 0;
}

bool readProcessInfo(
    const fs::path& processPath,
    ProcessInfo& process
) {
    std::ifstream statusFile(processPath / "status");

    if (!statusFile)
        return false;

    std::string line;

    process.memoryKB = 0;
    process.state = '?';

    while (std::getline(statusFile, line)) {

        if (line.rfind("Name:", 0) == 0) {
            process.name = line.substr(5);

            while (!process.name.empty() &&
                   (process.name.front() == '\t' ||
                    process.name.front() == ' ')) {
                process.name.erase(process.name.begin());
            }
        }

        else if (line.rfind("State:", 0) == 0) {
            if (line.length() > 7)
                process.state = line[7];
        }

        else if (line.rfind("VmRSS:", 0) == 0) {
            std::string value =
                line.substr(6);

            try {
                process.memoryKB =
                    std::stoull(value);
            }
            catch (...) {
                process.memoryKB = 0;
            }
        }
    }

    return !process.name.empty();
}

int main() {

    unsigned long long totalRAMKB =
        getTotalRAMKB();

    if (totalRAMKB == 0) {
        std::cerr << "Unable to read total RAM.\n";
        return 1;
    }

    std::vector<ProcessInfo> processes;

    for (const auto& entry :
         fs::directory_iterator("/proc")) {

        if (!entry.is_directory())
            continue;

        std::string dirname =
            entry.path().filename().string();

        if (!isNumber(dirname))
            continue;

        ProcessInfo process;

        try {
            process.pid = std::stoi(dirname);
        }
        catch (...) {
            continue;
        }

        if (readProcessInfo(entry.path(), process))
            processes.push_back(process);
    }

    std::sort(
        processes.begin(),
        processes.end(),
        [](const ProcessInfo& a,
           const ProcessInfo& b) {
            return a.memoryKB > b.memoryKB;
        }
    );

    std::cout << "\n";
    std::cout << "===============================================\n";
    std::cout << "           LENS PROCESS MONITOR\n";
    std::cout << "          Process Memory Analysis\n";
    std::cout << "===============================================\n\n";

    std::cout << "PID\tSTATE\tMEMORY\tPROCESS\n";
    std::cout << "-----------------------------------------------\n";

    int displayed = 0;

    for (const auto& process : processes) {

        double memoryPercent =
            (static_cast<double>(process.memoryKB) /
             totalRAMKB) * 100.0;

        std::cout << process.pid
                  << "\t"
                  << process.state
                  << "\t"
                  << memoryPercent
                  << "%\t"
                  << process.name
                  << "\n";

        displayed++;

        // Display top 20 memory-consuming processes
        if (displayed >= 20)
            break;
    }

    std::cout << "\n-----------------------------------------------\n";
    std::cout << "Total Processes Detected: "
              << processes.size()
              << "\n";

    std::cout << "Showing Top 20 by Memory Usage\n";

    std::cout << "===============================================\n";

    return 0;
}

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>

namespace fs = std::filesystem;

struct ProcessInfo {
    int pid;
    std::string name;
    char state;
};

bool isNumber(const std::string& str) {
    return !str.empty() &&
           std::all_of(str.begin(), str.end(),
                       [](char c) {
                           return std::isdigit(c);
                       });
}

int main() {

    std::vector<ProcessInfo> processes;

    for (const auto& entry : fs::directory_iterator("/proc")) {

        if (!entry.is_directory())
            continue;

        std::string dirname =
            entry.path().filename().string();

        if (!isNumber(dirname))
            continue;

        int pid = std::stoi(dirname);

        std::ifstream statusFile(
            entry.path() / "status"
        );

        if (!statusFile)
            continue;

        std::string line;
        std::string name;
        char state = '?';

        while (std::getline(statusFile, line)) {

            if (line.rfind("Name:", 0) == 0) {
                name = line.substr(6);

                while (!name.empty() &&
                       name.front() == '\t') {
                    name.erase(name.begin());
                }
            }

            else if (line.rfind("State:", 0) == 0) {
                if (line.length() > 7)
                    state = line[7];
            }
        }

        if (!name.empty()) {
            processes.push_back({
                pid,
                name,
                state
            });
        }
    }

    std::sort(
        processes.begin(),
        processes.end(),
        [](const ProcessInfo& a,
           const ProcessInfo& b) {
            return a.pid < b.pid;
        }
    );

    std::cout << "\n";
    std::cout << "============================================\n";
    std::cout << "          LENS PROCESS MONITOR\n";
    std::cout << "============================================\n\n";

    std::cout << "PID\tSTATE\tPROCESS\n";
    std::cout << "--------------------------------------------\n";

    for (const auto& process : processes) {

        std::cout << process.pid
                  << "\t"
                  << process.state
                  << "\t"
                  << process.name
                  << "\n";
    }

    std::cout << "\n--------------------------------------------\n";
    std::cout << "Total Processes: "
              << processes.size()
              << "\n";

    std::cout << "============================================\n";

    return 0;
}

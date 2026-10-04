#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream file("/proc/meminfo");

    if (!file) {
        std::cerr << "Error: Cannot read /proc/meminfo\n";
        return 1;
    }

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

    if (totalRAM == 0) {
        std::cerr << "Error: Could not read RAM information\n";
        return 1;
    }

    double usedRAM =
        100.0 * (1.0 - static_cast<double>(availableRAM) / totalRAM);

    double totalGB = totalRAM / (1024.0 * 1024.0);
    double availableGB = availableRAM / (1024.0 * 1024.0);

    std::cout << "LENS RAM MONITOR\n";
    std::cout << "================\n";
    std::cout << "Total RAM     : " << totalGB << " GB\n";
    std::cout << "Available RAM : " << availableGB << " GB\n";
    std::cout << "RAM Usage     : " << usedRAM << "%\n";

    return 0;
}

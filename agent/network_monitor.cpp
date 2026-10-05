#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream file("/proc/net/dev");

    if (!file) {
        std::cerr << "Error: Cannot read network information\n";
        return 1;
    }

    std::string line;

    std::cout << "\n========================================\n";
    std::cout << "       LENS NETWORK MONITOR\n";
    std::cout << "========================================\n";

    while (std::getline(file, line)) {
        if (line.find(":") == std::string::npos)
            continue;

        std::size_t pos = line.find(":");
        std::string interfaceName = line.substr(0, pos);

        while (!interfaceName.empty() && interfaceName.front() == ' ')
            interfaceName.erase(interfaceName.begin());

        unsigned long long rxBytes = 0;
        unsigned long long txBytes = 0;

        std::sscanf(
            line.c_str() + pos + 1,
            "%llu %*u %*u %*u %*u %*u %*u %*u %llu",
            &rxBytes,
            &txBytes
        );

        std::cout << "Interface : " << interfaceName << "\n";
        std::cout << "RX Bytes  : " << rxBytes << "\n";
        std::cout << "TX Bytes  : " << txBytes << "\n";
        std::cout << "----------------------------------------\n";
    }

    return 0;
}

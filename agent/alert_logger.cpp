#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

void logAlert(const std::string& message) {
    std::ofstream file("lens_alerts.log", std::ios::app);

    time_t now = time(nullptr);
    char* timeStr = ctime(&now);

    if (timeStr)
        file << "[" << timeStr << "] " << message << "\n";

    file.close();
}

int main() {
    std::cout << "========================================\n";
    std::cout << "        LENS ALERT LOGGER\n";
    std::cout << "========================================\n";

    logAlert("LENS monitoring agent started.");

    std::cout << "Alert logging started successfully.\n";
    std::cout << "Log file: lens_alerts.log\n";

    return 0;
}

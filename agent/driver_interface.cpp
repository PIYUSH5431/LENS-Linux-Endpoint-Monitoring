#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::ifstream driver("/dev/lens_monitor");

    if (!driver.is_open())
    {
        std::cerr << "ERROR: Cannot open LENS kernel driver.\n";
        std::cerr << "Try running with sudo.\n";
        return 1;
    }

    std::string message;
    std::getline(driver, message);

    std::cout << "========================================\n";
    std::cout << "       LENS DRIVER INTERFACE\n";
    std::cout << "========================================\n";
    std::cout << "Kernel Driver Status : CONNECTED\n";
    std::cout << "Driver Message       : " << message << "\n";
    std::cout << "========================================\n";

    driver.close();

    return 0;
}

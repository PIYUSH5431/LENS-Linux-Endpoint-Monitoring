#include <iostream>
#include <sys/statvfs.h>

int main() {
    struct statvfs disk;

    if (statvfs("/", &disk) != 0) {
        std::cerr << "Error: Cannot read disk information\n";
        return 1;
    }

    unsigned long long totalBytes =
        static_cast<unsigned long long>(disk.f_blocks) * disk.f_frsize;

    unsigned long long freeBytes =
        static_cast<unsigned long long>(disk.f_bavail) * disk.f_frsize;

    unsigned long long usedBytes = totalBytes - freeBytes;

    double totalGB = totalBytes / (1024.0 * 1024.0 * 1024.0);
    double usedGB = usedBytes / (1024.0 * 1024.0 * 1024.0);

    double usage =
        (static_cast<double>(usedBytes) / totalBytes) * 100.0;

    std::cout << "LENS DISK MONITOR\n";
    std::cout << "=================\n";
    std::cout << "Total Disk : " << totalGB << " GB\n";
    std::cout << "Used Disk  : " << usedGB << " GB\n";
    std::cout << "Disk Usage : " << usage << "%\n";

    return 0;
}

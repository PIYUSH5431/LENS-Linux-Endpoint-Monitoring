#include <iostream>
#include <cstdlib>

using namespace std;

void showMenu()
{
    cout << "\n========================================\n";
    cout << "        LENS SECURITY AGENT\n";
    cout << "   Linux Endpoint Monitoring System\n";
    cout << "========================================\n";

    cout << "1. System Monitor\n";
    cout << "2. Process Monitor\n";
    cout << "3. Network Monitor\n";
    cout << "4. Alert Logger\n";
    cout << "5. Security Alert Engine\n";
    cout << "6. Kernel Driver Interface\n";
    cout << "7. System Health Score\n";
    cout << "8. Process Security Scan\n";
    cout << "9. Run Full Monitoring\n";
    cout << "0. Exit\n";

    cout << "========================================\n";
    cout << "Enter choice: ";
}

int main()
{
    int choice;

    cout << "\n========================================\n";
    cout << "       LENS ENDPOINT MONITORING\n";
    cout << "========================================\n";
    cout << "Linux Endpoint Monitoring & Security Agent\n";

    while (true)
    {
        showMenu();
        cin >> choice;

        switch (choice)
        {
            case 1:
                system("./system_monitor");
                break;

            case 2:
                system("./process_monitor");
                break;

            case 3:
                system("./network_monitor");
                break;

            case 4:
                system("./alert_logger");
                break;

            case 5:
                system("./security_alert");
                break;

            case 6:
                system("sudo ./driver_interface");
                break;

            case 7:
                system("./health_score");
                break;

            case 8:
                system("./process_security");
                break;

            case 9:
                cout << "\n[1/6] Starting System Monitor...\n";
                system("timeout 5 ./system_monitor");

                cout << "\n[2/6] Starting Process Monitor...\n";
                system("./process_monitor");

                cout << "\n[3/6] Starting Network Monitor...\n";
                system("./network_monitor");

                cout << "\n[4/6] Checking Kernel Driver...\n";
                system("sudo ./driver_interface");

                cout << "\n[5/6] Running Security Alert Engine...\n";
                system("./security_alert");

                cout << "\n[6/6] Running Process Security Scan...\n";
                system("./process_security");

                cout << "\n========================================\n";
                cout << " LENS FULL MONITORING CYCLE COMPLETED\n";
                cout << "========================================\n";
                break;

            case 0:
                cout << "\nLENS Agent shutting down...\n";
                return 0;

            default:
                cout << "\nInvalid choice. Try again.\n";
        }
    }

    return 0;
}

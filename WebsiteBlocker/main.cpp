#include "pch.h"  // Must be first line
#include "WebsiteBlocker.h"
#include <iostream>
#include <string>
#include <sstream>
#include <windows.h>
#include <shellapi.h>
#include <windows.h>
#include <shellapi.h>

// Function to restart as administrator
bool RestartAsAdmin() {
    char exePath[MAX_PATH];
    GetModuleFileNameA(NULL, exePath, MAX_PATH);

    // Use ShellExecuteA to restart with "runas" verb
    HINSTANCE result = ShellExecuteA(NULL, "runas", exePath, NULL, NULL, SW_SHOWNORMAL);

    return (int)result > 32; // Success if > 32
}

int main() {
    // Check if running as administrator
    if (!WebsiteBlocker::IsRunningAsAdmin()) {
        std::cout << "This application requires administrator privileges.\n";
        std::cout << "Attempting to restart with admin rights...\n";

        if (RestartAsAdmin()) {
            std::cout << "Please approve the UAC prompt to continue.\n";
            return 0; // Exit this instance
        }
        else {
            std::cout << "Failed to restart as administrator.\n";
            std::cout << "Please manually run this program as administrator.\n";
            std::cin.get(); // Wait for user input
            return 1;
        }
    }

    WebsiteBlocker blocker;
    std::string input;

    std::cout << "=== Website Blocker (Running as Administrator) ===\n";
    std::cout << "Commands:\n";
    std::cout << "  block <domain>    - Block a website\n";
    std::cout << "  unblock <domain>  - Unblock a website\n";
    std::cout << "  list              - Show blocked domains\n";
    std::cout << "  exit              - Exit program\n\n";

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        std::istringstream iss(input);
        std::string command, domain;
        iss >> command >> domain;

        if (command == "block" && !domain.empty()) {
            if (blocker.BlockDomain(domain)) {
                std::cout << "Successfully blocked: " << domain << "\n";
            }
        }
        else if (command == "unblock" && !domain.empty()) {
            if (blocker.UnblockDomain(domain)) {
                std::cout << "Successfully unblocked: " << domain << "\n";
            }
        }
        else if (command == "list") {
            auto blocked = blocker.GetBlockedDomains();
            if (blocked.empty()) {
                std::cout << "No domains are currently blocked.\n";
            }
            else {
                std::cout << "Blocked domains:\n";
                for (const auto& d : blocked) {
                    std::cout << "  " << d << "\n";
                }
            }
        }
        else {
            std::cout << "Invalid command. Use: block/unblock <domain>, list, or exit\n";
        }
    }

    return 0;
}
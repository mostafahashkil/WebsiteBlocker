#include "WebsiteBlocker.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <windows.h>
#include <shellapi.h>

// Static member definitions
const std::string WebsiteBlocker::REDIRECT_IP = "127.0.0.1";
const std::string WebsiteBlocker::BLOCK_MARKER_START = "# Website Blocker - Blocked Sites";
const std::string WebsiteBlocker::BLOCK_MARKER_END = "# Website Blocker - End";

WebsiteBlocker::WebsiteBlocker() {
    // Get Windows directory and construct hosts file path
    char windowsDir[MAX_PATH];
    GetWindowsDirectoryA(windowsDir, MAX_PATH);
    hostsFilePath = std::filesystem::path(windowsDir) / "System32" / "drivers" / "etc" / "hosts";

    std::cout << "Using hosts file: " << hostsFilePath << "\n";
}

bool WebsiteBlocker::IsRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    HANDLE token = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation;
        DWORD size = sizeof(TOKEN_ELEVATION);

        if (GetTokenInformation(token, TokenElevation, &elevation, sizeof(elevation), &size)) {
            isAdmin = elevation.TokenIsElevated;
        }
        CloseHandle(token);
    }

    return isAdmin == TRUE;
}

bool WebsiteBlocker::BlockDomain(const std::string& domain) {
    if (domain.empty()) {
        LogError("Domain cannot be empty");
        return false;
    }

    try {
        auto lines = ReadHostsFile();

        // Check if domain is already blocked
        if (IsDomainAlreadyBlocked(lines, domain)) {
            std::cout << "Domain " << domain << " is already blocked.\n";
            return true;
        }

        // Find or create our section
        auto startIt = std::find(lines.begin(), lines.end(), BLOCK_MARKER_START);
        auto endIt = std::find(lines.begin(), lines.end(), BLOCK_MARKER_END);

        if (startIt == lines.end() || endIt == lines.end()) {
            lines.push_back("");
            lines.push_back(BLOCK_MARKER_START);
            lines.push_back(BLOCK_MARKER_END);
        }
        startIt = std::find(lines.begin(), lines.end(), BLOCK_MARKER_START);
        endIt = std::find(lines.begin(), lines.end(), BLOCK_MARKER_END);

        // Generate domain variants and add them
        auto variants = GenerateDomainVariants(domain);
        std::vector<std::string> entries;
        for (const auto& variant : variants) {
            entries.push_back(REDIRECT_IP + " " + variant);
        }
        lines.insert(endIt, entries.begin(), entries.end());

        if (WriteHostsFile(lines)) {
            FlushDNSCache();
            return true;
        }

    }
    catch (const std::exception& e) {
        LogError("Exception while blocking domain: " + std::string(e.what()));
    }

    return false;
}

bool WebsiteBlocker::UnblockDomain(const std::string& domain) {
    if (domain.empty()) {
        LogError("Domain cannot be empty");
        return false;
    }

    try {
        auto lines = ReadHostsFile();
        auto newLines = RemoveDomainEntries(lines, domain);

        if (lines.size() == newLines.size()) {
            std::cout << "Domain " << domain << " was not found in blocked list.\n";
            return true;
        }

        if (WriteHostsFile(newLines)) {
            FlushDNSCache();
            return true;
        }

    }
    catch (const std::exception& e) {
        LogError("Exception while unblocking domain: " + std::string(e.what()));
    }

    return false;
}

std::vector<std::string> WebsiteBlocker::GetBlockedDomains() const {
    std::vector<std::string> domains;

    try {
        auto lines = ReadHostsFile();
        bool inOurSection = false;

        for (const auto& line : lines) {
            if (line == BLOCK_MARKER_START) {
                inOurSection = true;
                continue;
            }
            if (line == BLOCK_MARKER_END) {
                inOurSection = false;
                continue;
            }

            if (inOurSection && line.find(REDIRECT_IP) == 0) {
                // Extract domain from line like "127.0.0.1 domain.com"
                std::istringstream iss(line);
                std::string ip, domain;
                if (iss >> ip >> domain && !domain.empty()) {
                    domains.push_back(domain);
                }
            }
        }
    }
    catch (const std::exception& e) {
        LogError("Exception while reading blocked domains: " + std::string(e.what()));
    }

    return domains;
}

bool WebsiteBlocker::FlushDNSCache() {
    int result = system("ipconfig /flushdns > nul 2>&1");
    if (result == 0) {
        std::cout << "DNS cache flushed successfully.\n";
        return true;
    }
    else {
        LogError("Failed to flush DNS cache");
        return false;
    }
}

std::vector<std::string> WebsiteBlocker::ReadHostsFile() const {
    std::vector<std::string> lines;
    std::ifstream file(hostsFilePath);

    if (!file.is_open()) {
        LogError("Cannot open hosts file for reading. Check administrator privileges.");
        return lines;
    }

    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    return lines;
}

bool WebsiteBlocker::WriteHostsFile(const std::vector<std::string>& lines) const {
    std::ofstream file(hostsFilePath);

    if (!file.is_open()) {
        LogError("Cannot open hosts file for writing. Ensure you have administrator privileges.");
        return false;
    }

    for (const auto& line : lines) {
        file << line << "\n";
    }

    if (file.bad()) {
        LogError("Error occurred while writing to hosts file");
        return false;
    }

    return true;
}

std::vector<std::string> WebsiteBlocker::GenerateDomainVariants(const std::string& domain) const {
    std::vector<std::string> variants;

    // Add the domain as-is
    variants.push_back(domain);

    // If domain doesn't start with www., add www. variant
    if (domain.find("www.") != 0) {
        variants.push_back("www." + domain);
    }

    // If domain starts with www., add non-www variant
    if (domain.find("www.") == 0) {
        variants.push_back(domain.substr(4)); // Remove "www."
    }

    return variants;
}

bool WebsiteBlocker::IsDomainAlreadyBlocked(const std::vector<std::string>& lines, const std::string& domain) const {
    auto variants = GenerateDomainVariants(domain);

    for (const auto& line : lines) {
        for (const auto& variant : variants) {
            if (line.find(REDIRECT_IP + " " + variant) != std::string::npos) {
                return true;
            }
        }
    }

    return false;
}

std::vector<std::string> WebsiteBlocker::RemoveDomainEntries(const std::vector<std::string>& lines, const std::string& domain) const {
    std::vector<std::string> result;
    auto variants = GenerateDomainVariants(domain);

    for (const auto& line : lines) {
        bool shouldRemove = false;

        for (const auto& variant : variants) {
            if (line.find(REDIRECT_IP + " " + variant) != std::string::npos) {
                shouldRemove = true;
                break;
            }
        }

        if (!shouldRemove) {
            result.push_back(line);
        }
    }

    return result;
}

void WebsiteBlocker::LogError(const std::string& message) const {
    std::cerr << "Error: " << message << "\n";
}
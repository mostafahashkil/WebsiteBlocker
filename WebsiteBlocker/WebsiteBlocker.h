#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include "pch.h"

class WebsiteBlocker {
public:
    WebsiteBlocker();
    ~WebsiteBlocker() = default;

    // Main functionality
    bool BlockDomain(const std::string& domain);
    bool UnblockDomain(const std::string& domain);
    std::vector<std::string> GetBlockedDomains() const;

    // Utility functions
    static bool IsRunningAsAdmin();
    bool FlushDNSCache();

private:
    std::filesystem::path hostsFilePath;
    static const std::string REDIRECT_IP;
    static const std::string BLOCK_MARKER_START;
    static const std::string BLOCK_MARKER_END;

    // Helper functions
    std::vector<std::string> ReadHostsFile() const;
    bool WriteHostsFile(const std::vector<std::string>& lines) const;
    std::vector<std::string> GenerateDomainVariants(const std::string& domain) const;
    bool IsDomainAlreadyBlocked(const std::vector<std::string>& lines, const std::string& domain) const;
    std::vector<std::string> RemoveDomainEntries(const std::vector<std::string>& lines, const std::string& domain) const;
    void LogError(const std::string& message) const;
};
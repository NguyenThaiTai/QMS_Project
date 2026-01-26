// ConfigManager.h
#pragma once
#include <atlstr.h>

class ConfigManager
{
private:
    ConfigManager();
    ~ConfigManager();
    ConfigManager(const ConfigManager&) = delete;
    void operator=(const ConfigManager&) = delete;

    // add start configuration member variables NTTai 20260123
    CString m_strApiBaseUrl;
    CString m_strApiKey;
    CString m_strPrinterName;
    int m_nConnectionTimeout;
    // add end configuration member variables NTTai 20260123

public:
    // add start singleton instance accessor NTTai 20260123
    static ConfigManager& GetInstance();
    // add end singleton instance accessor NTTai 20260123

    // add start load and save configuration methods NTTai 20260123
    void LoadConfig();
    void SaveConfig();
    // add end load and save configuration methods NTTai 20260123

    // add start getters and setters for config properties NTTai 20260123
    CString GetApiBaseUrl() const;
    CString GetApiKey() const;
    int GetConnectionTimeout() const;
    CString GetPrinterName() const;

    void SetApiBaseUrl(CString strUrl);
    // add end getters and setters for config properties NTTai 20260123
     
    // add start helper to get config file path NTTai 20260123
    CString GetConfigFilePath();
    // add end helper to get config file path NTTai 20260123
};
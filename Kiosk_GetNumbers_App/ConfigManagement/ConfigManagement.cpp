// ConfigManager.cpp
#include "pch.h"
#include "ConfigManagement.h"

ConfigManager::ConfigManager()
{
    // add start initialize default values NTTai 20260123
    m_strApiBaseUrl = L"";
    m_strApiKey = L"";
    m_nConnectionTimeout = 0;
    m_strPrinterName = L"";
    // add end initialize default values NTTai 20260123
}

ConfigManager::~ConfigManager() {}

// add start singleton instance implementation NTTai 20260123
ConfigManager& ConfigManager::GetInstance()
{
    static ConfigManager instance;
    return instance;
}
// add end singleton instance implementation NTTai 20260123

// add start logic to resolve absolute config file path NTTai 20260123
CString ConfigManager::GetConfigFilePath()
{
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, MAX_PATH))
    {
        CString strPath = szPath;
        int pos = strPath.ReverseFind('\\');
        if (pos != -1)
            strPath = strPath.Left(pos + 1);

        return strPath + L"config.ini";
    }
    return L"config.ini";
}
// add end logic to resolve absolute config file path NTTai 20260123

// add start load configuration from INI file NTTai 20260123
void ConfigManager::LoadConfig()
{
    CString strPath = GetConfigFilePath();

    TCHAR szBuffer[1024];
    GetPrivateProfileString(L"Server", L"ApiBaseUrl", L"http://10.10.1.21:8081", szBuffer, 1024, strPath);
    m_strApiBaseUrl = szBuffer;

    if (m_strApiBaseUrl.Right(1) == L"/") {
        m_strApiBaseUrl = m_strApiBaseUrl.Left(m_strApiBaseUrl.GetLength() - 1);
    }

    m_nConnectionTimeout = GetPrivateProfileInt(L"Server", L"ConnectionTimeout", 15000, strPath);

    GetPrivateProfileString(L"Auth", L"ApiKey", L"", szBuffer, 1024, strPath);
    m_strApiKey = szBuffer;

    GetPrivateProfileString(L"Printer", L"PrinterName", L"", szBuffer, 1024, strPath);
    m_strPrinterName = szBuffer;
}
// add end load configuration from INI file NTTai 20260123

// add start save configuration to INI file NTTai 20260123
void ConfigManager::SaveConfig()
{
    CString strPath = GetConfigFilePath();
    WritePrivateProfileString(L"Server", L"ApiBaseUrl", m_strApiBaseUrl, strPath);

    CString strTimeout;
    strTimeout.Format(L"%d", m_nConnectionTimeout);
    WritePrivateProfileString(L"Server", L"ConnectionTimeout", strTimeout, strPath);
}
// add end save configuration to INI file NTTai 20260123

// add start getters implementation NTTai 20260123
CString ConfigManager::GetApiBaseUrl() const { return m_strApiBaseUrl; }
CString ConfigManager::GetApiKey() const { return m_strApiKey; }
int ConfigManager::GetConnectionTimeout() const { return m_nConnectionTimeout; }
CString ConfigManager::GetPrinterName() const { return m_strPrinterName; }
// add end getters implementation NTTai 20260123

// add start setters implementation NTTai 20260123
void ConfigManager::SetApiBaseUrl(CString strUrl) { m_strApiBaseUrl = strUrl; }
// add end setters implementation NTTai 20260123
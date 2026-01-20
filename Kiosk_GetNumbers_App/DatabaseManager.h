#pragma once
#include "sqlite3.h"
#include "Common.h" // add include shared data structure NTTai 20260114
#include <string>

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool InitializeDB(CString strDbName = L"KioskData.db"); // add initialize database connection NTTai 20260114
    bool SaveCustomer(const CitizenCardData& data); // add save customer data to SQLite NTTai 20260114
    bool IsCustomerExist(CString strIDNumber); // add check if customer exists NTTai 20260114
    bool DeleteAllCustomers(); // add delete all records NTTai 20260114
    void CloseDB(); // add close database connection NTTai 20260114

private:
    sqlite3* m_db;
    std::string CStringToUTF8(const CString& str); // add helper convert CString to UTF8 string NTTai 20260114
};
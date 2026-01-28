#include "pch.h"
#include "DatabaseManager.h"

DatabaseManager::DatabaseManager() { m_db = nullptr; }
DatabaseManager::~DatabaseManager() { CloseDB(); }

void DatabaseManager::CloseDB() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

// add start helper to convert CString to UTF8 NTTai 20260114
std::string DatabaseManager::CStringToUTF8(const CString& str) {
    CT2A ascii(str, CP_UTF8);
    return std::string(ascii.m_psz);
}
// add end helper to convert CString to UTF8 NTTai 20260114

// add start initialize database and create table NTTai 20260114
bool DatabaseManager::InitializeDB(CString strDbName) {
    TCHAR szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, MAX_PATH) == 0) return false;

    CString strFullPath = szPath;
    int pos = strFullPath.ReverseFind('\\');
    if (pos != -1) strFullPath = strFullPath.Left(pos + 1);

    strFullPath += strDbName;
    std::string dbNameUTF8 = CStringToUTF8(strFullPath);

    int rc = sqlite3_open(dbNameUTF8.c_str(), &m_db);
    if (rc != SQLITE_OK) return false;

    // add create table with 7 data columns + check-in time NTTai 20260114
    const char* sqlCreateTable =
        "CREATE TABLE IF NOT EXISTS CustomerInfo ("
        "CCCD TEXT PRIMARY KEY, "      // 1
        "CMND TEXT, "                  // 2
        "FullName TEXT, "              // 3
        "DateOfBirth TEXT, "           // 4
        "Gender TEXT, "                // 5
        "Address TEXT, "               // 6
        "DateOfIssue TEXT, "           // 7
        "CheckInTime DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");";

    char* errMsg = 0;
    rc = sqlite3_exec(m_db, sqlCreateTable, 0, 0, &errMsg);
    if (rc != SQLITE_OK) { sqlite3_free(errMsg); return false; }
    return true;
}
// add end initialize database and create table NTTai 20260114

// add start save customer information to database NTTai 20260114
bool DatabaseManager::SaveCustomer(const CitizenCardData& data) {
    if (!m_db) return false;

    const char* sqlInsert = "INSERT OR REPLACE INTO CustomerInfo "
        "(CCCD, CMND, FullName, DateOfBirth, Gender, Address, DateOfIssue) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sqlInsert, -1, &stmt, 0) != SQLITE_OK) return false;

    // add map data fields from struct NTTai 20260114
    std::string s1 = CStringToUTF8(data.strIDNumber);
    std::string s2 = CStringToUTF8(data.strCMND);
    std::string s3 = CStringToUTF8(data.strFullName);
    std::string s4 = CStringToUTF8(data.strDateOfBirth);
    std::string s5 = CStringToUTF8(data.strGender);
    std::string s6 = CStringToUTF8(data.strPlaceOfResidence); // add map address field NTTai 20260114
    std::string s7 = CStringToUTF8(data.strDateOfIssue);

    sqlite3_bind_text(stmt, 1, s1.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, s2.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, s3.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, s4.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, s5.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, s6.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, s7.c_str(), -1, SQLITE_STATIC);

    bool bSuccess = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return bSuccess;
}
// add end save customer information to database NTTai 20260114

// add start check if customer exists by ID NTTai 20260114
bool DatabaseManager::IsCustomerExist(CString strIDNumber) {
    if (!m_db) return false;
    const char* sqlCheck = "SELECT COUNT(*) FROM CustomerInfo WHERE CCCD = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sqlCheck, -1, &stmt, 0) != SQLITE_OK) return false;

    std::string sID = CStringToUTF8(strIDNumber);
    sqlite3_bind_text(stmt, 1, sID.c_str(), -1, SQLITE_STATIC);

    bool bExist = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt, 0) > 0) bExist = true;
    }
    sqlite3_finalize(stmt);
    return bExist;
}
// add end check if customer exists by ID NTTai 20260114

// add start delete all customer records NTTai 20260114
bool DatabaseManager::DeleteAllCustomers() {
    if (!m_db) return false;
    return (sqlite3_exec(m_db, "DELETE FROM CustomerInfo;", 0, 0, 0) == SQLITE_OK);
}
// add end delete all customer records NTTai 20260114
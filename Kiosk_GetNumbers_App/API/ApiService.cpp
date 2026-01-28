// File: ApiService.cpp
#include "pch.h"
#include "ApiService.h"

// add start include library for path handling NTTai 20260123
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
// add end include library for path handling NTTai 20260123

// add start helper to read API URL from config file NTTai 20260123
CString ApiService::ReadApiBaseUrl() {
    CString strPath = ConfigManager::GetInstance().GetConfigFilePath();
    TCHAR szValue[1024];
    GetPrivateProfileString(L"Server", L"ApiBaseUrl", L"", szValue, 1024, strPath);
    CString strUrl = CString(szValue);
    if (!strUrl.IsEmpty()) {
        if (strUrl.Find(L"://") == -1) strUrl = L"http://" + strUrl;
        if (strUrl.Right(1) == L"/") strUrl = strUrl.Left(strUrl.GetLength() - 1);
    }
    return strUrl;
}
// add end helper to read API URL from config file NTTai 20260123

ApiService::ApiService() {}
ApiService::~ApiService() {}

ApiService* ApiService::instance = nullptr;
ApiService* ApiService::GetInstance()
{
    if (instance == nullptr) {
        instance = new ApiService();
    }
    return instance;
}
// add start logic to fetch active services (GET) NTTai 20260123
bool ApiService::FetchActiveServices(std::vector<ServiceData>& outList) {
    outList.clear();
    CString strBaseUrl = ReadApiBaseUrl();
    if (strBaseUrl.IsEmpty()) {
        return false;
    }

    CString strUrl = strBaseUrl + L"/api/service/activelist";
    CInternetSession session(_T("AgribankKioskSession"), 1, INTERNET_OPEN_TYPE_DIRECT);
    session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 3000);
    session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 3000);

    CHttpFile* pFile = nullptr;
    CString strResponse = L"";
    bool bSuccess = false;

    try {
        // add use flags to prevent caching issues NTTai 20260123
        pFile = (CHttpFile*)session.OpenURL(strUrl, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE);
        if (pFile) {
            CStringA strRaw; char buffer[4096]; UINT nRead = 0;

            while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) { 
                buffer[nRead] = '\0'; 
                strRaw += buffer; 
            }

            pFile->Close(); delete pFile;
            strResponse = CA2W(strRaw, CP_UTF8);
            bSuccess = !strResponse.IsEmpty();
        }
    }
    catch (CInternetException* pEx) { 
        TCHAR szErr[1024];
        pEx->GetErrorMessage(szErr, 1024);

        DWORD dwErrorCode = pEx->m_dwError;

        TRACE(L"[API ERROR] FetchActiveServices | URL: %s | Code: %d | Msg: %s\n",
            strUrl, dwErrorCode, szErr);

        #ifdef _DEBUG
                CString strDebugMsg;
                strDebugMsg.Format(L"API Exception!\nUrl: %s\nCode: %d\nMsg: %s", strUrl, dwErrorCode, szErr);
        #endif
        pEx->Delete();
        bSuccess = false;
    }
    session.Close();
    if (bSuccess) ParseServiceListJson(strResponse, outList);
    return bSuccess;
}
// add end logic to fetch active services (GET) NTTai 20260123

// add start logic to parse service list JSON manually NTTai 20260123
void ApiService::ParseServiceListJson(CString strJSON, std::vector<ServiceData>& outList) {
    outList.clear();
    int len = strJSON.GetLength();
    int currentPos = 0;
    int arrayStart = strJSON.Find(L"\"services\"", 0);
    if (arrayStart != -1) currentPos = arrayStart;

    while (currentPos < len) {
        int startObj = strJSON.Find(L"{", currentPos);
        if (startObj == -1) break;
        int endObj = -1; int braceCount = 1;
        for (int i = startObj + 1; i < len; i++) {
            if (strJSON[i] == L'{') braceCount++;
            else if (strJSON[i] == L'}') { braceCount--; if (braceCount == 0) { endObj = i; break; } }
        }
        if (endObj == -1) break;

        CString strObj = strJSON.Mid(startObj, endObj - startObj + 1);
        int nId = -1;
        int posId = strObj.Find(L"\"id\"");
        if (posId != -1) {
            int startNum = -1;
            for (int k = posId + 3; k < strObj.GetLength(); k++) {
                wchar_t c = strObj[k];
                if (c >= L'0' && c <= L'9') { if (startNum == -1) startNum = k; }
                else if (startNum != -1) { nId = _wtoi(strObj.Mid(startNum, k - startNum)); break; }
            }
            if (nId == -1 && startNum != -1) nId = _wtoi(strObj.Mid(startNum));
        }
        CString strTitle = L"";
        int posName = strObj.Find(L"\"name\"");
        if (posName != -1) {
            int startQuote = strObj.Find(L"\"", posName + 6);
            if (startQuote != -1) {
                int endQuote = strObj.Find(L"\"", startQuote + 1);
                while (endQuote != -1 && strObj[endQuote - 1] == L'\\') { endQuote = strObj.Find(L"\"", endQuote + 1); }
                if (endQuote != -1) {
                    CString strRawName = strObj.Mid(startQuote + 1, endQuote - startQuote - 1);
                    strTitle = DecodeJsonString(strRawName);
                }
            }
        }
        if (nId != -1 && !strTitle.IsEmpty()) {
            ServiceData item; item.id = nId; item.title = strTitle; item.description = L"Giao dịch tại quầy";
            outList.push_back(item);
        }
        currentPos = endObj + 1;
    }
}
// add end logic to parse service list JSON manually NTTai 20260123

CString ApiService::DecodeJsonString(CString strRaw) {
    CString strResult = L""; int len = strRaw.GetLength(); int i = 0;
    while (i < len) {
        if (i + 5 < len && strRaw[i] == L'\\' && strRaw[i + 1] == L'u') {
            CString strHex = strRaw.Mid(i + 2, 4); wchar_t* pEnd; long code = wcstol(strHex, &pEnd, 16);
            if (code != 0) { strResult += (wchar_t)code; i += 6; }
            else { strResult += strRaw[i]; i++; }
        }
        else { strResult += strRaw[i]; i++; }
    }
    return strResult;
}

// add start mapping helpers return Unicode CString to avoid double encoding NTTai 20260123
CString ApiService::MapGenderToAPI(CString strGender) { 
    if (strGender.Trim().CompareNoCase(L"Nam") == 0) return L"male"; 
    if (strGender.Trim().CompareNoCase(L"Nữ") == 0) return L"female"; 
    return L"other"; 
}
CString ApiService::MapDateToAPI(CString strDate) { 
    if (strDate.GetLength() >= 10 && strDate.GetAt(2) == L'/') { 
        CString day = strDate.Left(2); 
        CString month = strDate.Mid(3, 2); 
        CString year = strDate.Right(4); 
        CString strResult; 
        strResult.Format(L"%s-%s-%s", year, month, day); 
        return strResult; 
    } 
    return strDate; 
}
// add end mapping helpers return Unicode CString to avoid double encoding NTTai 20260123

CString ApiService::ParseTicketNumberJson(CString strJson) { 
    CString strTicket = L""; 
    int posKey = strJson.Find(L"\"ticket_number\""); 
    if (posKey != -1) { 
        int colon = strJson.Find(L":", posKey); 
        int q1 = strJson.Find(L"\"", colon); 
        int q2 = strJson.Find(L"\"", q1 + 1); 
        if (q1 != -1 && q2 != -1) strTicket = strJson.Mid(q1 + 1, q2 - q1 - 1); 
    } 
    return strTicket; 
}
CString ApiService::ParseUrlFromJson(CString strJson) { 
    CString strUrl = L""; 
    int posKey = strJson.Find(L"\"url\""); 
    if (posKey != -1) { 
        int colon = strJson.Find(L":", posKey); 
        int q1 = strJson.Find(L"\"", colon); 
        int q2 = strJson.Find(L"\"", q1 + 1); 
        if (q1 != -1 && q2 != -1) { 
            strUrl = strJson.Mid(q1 + 1, q2 - q1 - 1); 
            strUrl.Replace(L"\\/", L"/"); 
        } 
    } 
    return strUrl; 
}

// add start helper to check if error is retryable NTTai 20260126
bool IsNetworkErrorRetryable(DWORD dwError) {
    // 12002: Timeout, 12029: Cannot Connect, 12007: Name not resolved
    return (dwError == 12002 || dwError == 12029 || dwError == 12007);
}
// add end helper to check if error is retryable NTTai 20260126

// add start send POST request with Headers matching Postman NTTai 20260123
CString ApiService::SendPostRequest(CString url, CString jsonBodyW) {
    // add start increase timeout and setup retry logic NTTai 20260126
    ConfigManager::GetInstance().LoadConfig();
    int nTimeout = ConfigManager::GetInstance().GetConnectionTimeout();

    CInternetSession session(_T("AgribankKioskSession"), 1, INTERNET_OPEN_TYPE_PRECONFIG);
    session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, nTimeout);
    session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, nTimeout);
    session.SetOption(INTERNET_OPTION_SEND_TIMEOUT, nTimeout);

    CHttpConnection* pConnection = nullptr;
    CHttpFile* pFile = nullptr;
    CString strResult = L"";

    int nRetryCount = 0;
    const int MAX_RETRIES = 3;
    bool bSuccess = false;

    while (nRetryCount < MAX_RETRIES && !bSuccess) {
        try {
            DWORD dwServiceType; CString strServerName, strObject; INTERNET_PORT nPort;
            if (AfxParseURL(url, dwServiceType, strServerName, strObject, nPort)) {
                if (pFile) { delete pFile; pFile = nullptr; }
                if (pConnection) { delete pConnection; pConnection = nullptr; }

                pConnection = session.GetHttpConnection(strServerName, nPort);

                pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strObject,
                    NULL, 1, NULL, NULL,
                    INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_KEEP_CONNECTION);

                CString strHeaders = L"Content-Type: application/json; charset=utf-8\r\nAccept: application/json";
                CStringA strBodyA = CW2A(jsonBodyW, CP_UTF8);

                // Send Request
                if (pFile->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)(LPCSTR)strBodyA, strBodyA.GetLength())) {

                    DWORD dwStatusCode;
                    pFile->QueryInfoStatusCode(dwStatusCode);

                    CStringA strRaw; char buffer[4096]; UINT nRead = 0;
                    while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) {
                        buffer[nRead] = '\0'; strRaw += buffer;
                    }
                    strResult = CA2W(strRaw, CP_UTF8);

                    // Check Logic: 
                    // 2xx -> Success
                    // 408, 502, 503, 504 -> Gateway/Server Timeout (Retryable)
                    // 400, 401, 500 -> Logic Error (Do NOT Retry)

                    if (dwStatusCode >= 200 && dwStatusCode < 300) {
                        bSuccess = true; 
                    }
                    else if (   dwStatusCode == 408 ||
                                dwStatusCode == 502 || 
                                dwStatusCode == 503 || 
                                dwStatusCode == 504) 
                    {
                        throw new CInternetException(12002);
                    }
                    else {
                        // Hard error (Bad Request, Unauthorized...) -> Stop retrying
                        CString strErr;
                        strErr.Format(L"ERROR API (%d):\n%s", dwStatusCode, strResult);
                        AfxMessageBox(strErr);
                        strResult = L"";
                        break;
                    }
                }
            }
        }
        catch (CInternetException* pEx) {
            TCHAR szErr[1024]; pEx->GetErrorMessage(szErr, 1024);
            DWORD dwCode = pEx->m_dwError;
            pEx->Delete();

            TRACE(L"[API RETRY] Attempt %d/%d Failed. Code: %d. Msg: %s\n", nRetryCount + 1, MAX_RETRIES, dwCode, szErr);

            if (IsNetworkErrorRetryable(dwCode) && nRetryCount < MAX_RETRIES - 1) {
                nRetryCount++;
                Sleep(1000);
                continue;    // Loop again
            }
            else {
                CString msg; 
                msg.Format(L"Lỗi kết nối (Sau %d lần thử):\n%s", nRetryCount + 1, szErr);
                AfxMessageBox(msg);
            }
        }
    }

    if (pFile) delete pFile;
    if (pConnection) delete pConnection;
    session.Close();
    return strResult;
    // add end increase timeout and setup retry logic NTTai 20260126
}
// add end send POST request with Headers matching Postman NTTai 20260123

// add start issue ticket logic (POST) with Unicode support NTTai 20260123
ApiResponse ApiService::IssueTicket(int serviceID, const CitizenCardData* pAuthData) {
    ApiResponse response = { false, L"", L"" };
    CString strBaseUrl = ReadApiBaseUrl();
    if (strBaseUrl.IsEmpty()) { response.message = L"Lỗi Config"; return response; }

    CString strUrl; strUrl.Format(L"%s/api/ticketAuth/%d", strBaseUrl, serviceID);

    CString strBody; // add build JSON body using Unicode CString to prevent font corruption NTTai 20260123

    if (pAuthData) {
        strBody.Format(
            L"{"
            L"\"adapter_type\": \"NFC\","
            L"\"data\": {"
            L"\"cccd\": \"%s\","
            L"\"cmnd\": \"%s\","
            L"\"full_name\": \"%s\","
            L"\"date_of_birth\": \"%s\","
            L"\"sex\": \"%s\","
            L"\"address\": \"%s\","
            L"\"date_of_issue\": \"%s\""
            L"}"
            L"}",
            pAuthData->strIDNumber,
            pAuthData->strCMND,
            pAuthData->strFullName,
            MapDateToAPI(pAuthData->strDateOfBirth),
            MapGenderToAPI(pAuthData->strGender),
            pAuthData->strPlaceOfResidence,
            MapDateToAPI(pAuthData->strDateOfIssue)
        );
    }
    else {
        strBody =
            L"{"
            L"\"adapter_type\": null,"
            L"\"data\": {"
            L"\"cccd\": \"Chưa xác thực\","
            L"\"cmnd\": null,"
            L"\"full_name\": \"Khách vãng lai\","
            L"\"date_of_birth\": null,"
            L"\"sex\": null,"
            L"\"address\": null,"
            L"\"date_of_issue\": null"
            L"}"
            L"}";
    }

    CString strResponse = SendPostRequest(strUrl, strBody);

    if (!strResponse.IsEmpty()) {
        response.data = ParseTicketNumberJson(strResponse);
        if (!response.data.IsEmpty()) response.success = true;
        else response.message = L"Server không trả về số vé";
    }
    else response.message = L"Lỗi kết nối";

    return response;
}
// add end issue ticket logic (POST) with Unicode support NTTai 20260123
    
CString ApiService::GetNewQRCodeUrl() {
    CString strBaseUrl = ReadApiBaseUrl();
    if (strBaseUrl.IsEmpty()) return L"";
    CString strUrl = strBaseUrl + L"/api/ticket/generate-new-qr";
    CInternetSession session(_T("AgribankKioskSession"), 1, INTERNET_OPEN_TYPE_DIRECT);
    CHttpFile* pFile = nullptr;
    CString strResponse = L"";
    try {
        pFile = (CHttpFile*)session.OpenURL(strUrl, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);
        if (pFile) {
            CStringA strRaw; 
            char buffer[4096]; 
            UINT nRead = 0;
            while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) { 
                buffer[nRead] = '\0'; 
                strRaw += buffer; 
            }
            strResponse = CA2W(strRaw, CP_UTF8);
            pFile->Close(); 
            delete pFile;
        }
    }
    catch (CInternetException* pEx) {
        TCHAR szErr[1024]; pEx->GetErrorMessage(szErr, 1024);
        DWORD dwErrorCode = pEx->m_dwError;
        TRACE(L"[API QR ERROR] URL: %s | Code: %d | Msg: %s\n", strUrl, dwErrorCode, szErr);
        pEx->Delete();
        if (pFile) delete pFile;
    }
    session.Close();
    return ParseUrlFromJson(strResponse);
}
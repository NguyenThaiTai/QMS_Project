// File: ApiService.h
#pragma once
#include <afxinet.h>
#include <vector>
#include <string>
#include "Common.h"

struct ApiResponse {
    bool success;
    CString message;
    CString data;
};

class ApiService
{
public:
    static ApiService& GetInstance();
    // add start API methods declarations NTTai 20260123
    bool FetchActiveServices(std::vector<ServiceData>& outList);
    ApiResponse IssueTicket(int serviceID, const CitizenCardData* pAuthData = nullptr);
    CString GetNewQRCodeUrl();
    // add end API methods declarations NTTai 20260123

private:
    ApiService();
    ~ApiService();

    ApiService(const ApiService&) = delete;
    void operator=(const ApiService&) = delete;

    CInternetSession* m_pSession;

    CString SendGetRequest(CString url);
    CString SendPostRequest(CString url, CString jsonBodyW); // add updated param name to jsonBodyW NTTai 20260123

    // add start helper functions declarations NTTai 20260123
    void ParseServiceListJson(CString strJson, std::vector<ServiceData>& outList);
    CString ParseTicketNumberJson(CString strJson);
    CString ParseUrlFromJson(CString strJson);
    CString DecodeJsonString(CString strRaw);
    CString MapGenderToAPI(CString strGender);
    CString MapDateToAPI(CString strDate);
    // add end helper functions declarations NTTai 20260123

protected:
	CString ReadApiBaseUrl();
};
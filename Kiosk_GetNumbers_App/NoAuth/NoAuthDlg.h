// File: NoAuthDlg.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include <vector>
#include "../resource.h" 
#include "../Common/ButtonUI.h"
#include "../Common/Common.h" 
#include "../Common/HeaderUI.h"
#include "../Common/AppMessages.h"
#include "../API/ApiService.h"
#include "../DisplayNumbers/DisplayNumbersDlg.h"
#include "../MainScreen/AuthScreen.h"

struct ServiceUIItem {
    ServiceData data;
    Gdiplus::RectF rect;
    bool isPressed;
    ServiceUIItem() { isPressed = false; }
};

struct TicketProcessParam {
    HWND hWnd;
    int serviceID;
    bool isAuthenticated;
    CitizenCardData authData;
};

struct TicketProcessResult {
    bool success;
    CString ticketNumber;
    CString errorMessage;
    int serviceID;
};

// add start struct for service list thread result NTTai 20260123
struct ServiceListResult {
    bool success;
    std::vector<ServiceData> list;
};
// add end struct for service list thread result NTTai 20260123

class NoAuthDlg : public CDialogEx
{
    DECLARE_DYNAMIC(NoAuthDlg)

public:
    NoAuthDlg(CWnd* pParent = nullptr);
    virtual ~NoAuthDlg();
    void SetAuthenticatedData(const CitizenCardData& data);

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_NO_AUTH_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

    afx_msg LRESULT OnApiResult(WPARAM wParam, LPARAM lParam);

    // add start handler for async list result NTTai 20260123
    afx_msg LRESULT OnApiListResult(WPARAM wParam, LPARAM lParam);
    // add end handler for async list result NTTai 20260123

    DECLARE_MESSAGE_MAP()

private:
    void DrawHeaderTitle(Gdiplus::Graphics& g, int cx, int cy);
    void DrawServiceList(Gdiplus::Graphics& g, int cx, int cy);
    void DrawChevron(Gdiplus::Graphics& g, Gdiplus::RectF cardRect);

    void StartTicketProcess(int serviceID);
    static UINT __cdecl WorkerThreadProc(LPVOID pParam);

    // add start async list fetching variables and functions NTTai 20260123
    void UpdateServiceList();
    static UINT __cdecl ListFetcherThreadProc(LPVOID pParam);
    bool m_bIsFetchingList = false;
    // add end async list fetching variables and functions NTTai 20260123

    std::vector<ServiceUIItem> m_uiItems;
    Gdiplus::RectF m_rectCancelBtn;

    bool m_bIsLoading = false;
    bool m_bIsAuthenticated = false;
    CitizenCardData m_authData;

	ApiService *m_pApiService;
};
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "resource.h" 
#include "ButtonUI.h"
#include <iostream>
#include <vector>
#include <afxinet.h>
#include "Common.h" // add include shared data structure NTTai 20260114

// add start service item structure definition NTTai 20260114
struct ServiceItem {
    std::wstring title;
    std::wstring description;
    Gdiplus::RectF rect;
    int id;
    bool isPressed = false;

    // Operator to compare two service items
    bool operator==(const ServiceItem& other) const {
        return (id == other.id) && (title == other.title);
    }

    bool operator!=(const ServiceItem& other) const {
        return !(*this == other);
    }
};
// add end service item structure definition NTTai 20260114

class NoAuthDlg : public CDialogEx
{
    DECLARE_DYNAMIC(NoAuthDlg)
public:
    NoAuthDlg(CWnd* pParent = nullptr);
    virtual ~NoAuthDlg();

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
    DECLARE_MESSAGE_MAP()

private:
    void DrawServiceList(Gdiplus::Graphics& g, int cx, int cy); // add draw service list grid NTTai 20260114
    void DrawHeaderTitle(Gdiplus::Graphics& g, int cx, int cy); // add draw header title NTTai 20260114
    void DrawChevron(Gdiplus::Graphics& g, Gdiplus::RectF cardRect); // add draw chevron icon NTTai 20260114

    void FetchServicesFromServer(); // add fetch active services from API NTTai 20260114
    void ParseJSONAndPopulate(CString strJSON, std::vector<ServiceItem>& outList); // add parse JSON response NTTai 20260114
    CString DecodeJsonString(CString strRaw); // add decode unicode JSON string NTTai 20260114
    CString RequestTicketFromServer(int serviceID); // add request ticket for walk-in guest NTTai 20260114

    std::vector<ServiceItem> m_services;    
    Gdiplus::RectF m_rectCancelBtn;
    bool m_bCancelPressed = false;

    CString RequestAuthenticatedTicket(int serviceID); // add request ticket for auth customer NTTai 20260114
    CitizenCardData m_authData; // add authenticated customer data NTTai 20260114
    bool m_bIsAuthenticated = false; // add authentication flag NTTai 20260114

public:
    void SetAuthenticatedData(const CitizenCardData& data); // add set auth data method NTTai 20260114
};
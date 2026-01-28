// File: AuthCorrect.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h" 
#include "../Common/ButtonUI.h"
#include "../Common/HeaderUI.h"
#include "../Common/Common.h"

class AuthCorrect : public CDialogEx
{
    DECLARE_DYNAMIC(AuthCorrect)

public:
    AuthCorrect(CString strFullName, bool bIsNewCustomer, CWnd* pParent = nullptr); // add constructor with customer info NTTai 20260114
    virtual ~AuthCorrect(); // add destructor NTTai 20260114

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_AUTH_FINGER_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog(); // add dialog initialization NTTai 20260114
    afx_msg void OnPaint(); // add paint event handler NTTai 20260114
    afx_msg BOOL OnEraseBkgnd(CDC* pDC); // add background erase handler NTTai 20260114
    afx_msg void OnTimer(UINT_PTR nIDEvent); // add timer event handler NTTai 20260114
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point); // add left button up handler NTTai 20260114
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point); // add left button down handler NTTai 20260114
    DECLARE_MESSAGE_MAP()

private:
    void DrawSuccessIcon(Gdiplus::Graphics& g, int cx, int cy); // add draw success icon NTTai 20260114
    void DrawMessage(Gdiplus::Graphics& g, int cx, int cy); // add draw message text NTTai 20260114
    void DrawCustomerBox(Gdiplus::Graphics& g, int cx, int cy); // add draw customer info box NTTai 20260114
    void DrawActionBtn(Gdiplus::Graphics& g, int cx, int cy); // add draw action button NTTai 20260114
    void DrawFooter(Gdiplus::Graphics& g, int cx, int cy); // add draw footer text NTTai 20260114
    CString ConvertToTitleCase(CString strName); // add convert string to title case NTTai 20260114

    CString m_strFullName; // add customer full name variable NTTai 20260114
    int m_nCountdown; // add countdown timer variable NTTai 20260114
    Gdiplus::RectF m_rectContinueBtn; // add continue button rect variable NTTai 20260114
    bool m_bContinuePressed = false; // add continue button pressed state NTTai 20260114
    bool m_bIsNewCustomer; // add new customer flag NTTai 20260114
    CitizenCardData m_authData; // add authenticated data storage NTTai 20260114

public:
    void SetAuthData(const CitizenCardData& data) { m_authData = data; } // add set authenticated data method NTTai 20260114
};
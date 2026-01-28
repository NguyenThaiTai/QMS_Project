// File: AuthQRCodeDlg.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h" 
#include "../Common/ButtonUI.h"
#include "../API/ApiService.h" // add include api service NTTai 20260123
#include "../Common/AppMessages.h" // add include common messages NTTai 20260123
#include "../Common/HeaderUI.h"
#include "qrcodegen.hpp"

using qrcodegen::QrCode;

// add start struct for QR thread result NTTai 20260123
struct QrThreadResult {
    bool success;
    CString url;
};
// add end struct for QR thread result NTTai 20260123

class AuthQRCodeDlg : public CDialogEx
{
    DECLARE_DYNAMIC(AuthQRCodeDlg)
public:
    AuthQRCodeDlg(CWnd* pParent = nullptr);
    virtual ~AuthQRCodeDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_AUTH_QRCODE_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    // add start handler for async QR result NTTai 20260123
    afx_msg LRESULT OnApiQrResult(WPARAM wParam, LPARAM lParam);
    // add end handler for async QR result NTTai 20260123

    DECLARE_MESSAGE_MAP()

private:
    void DrawInstructions(Gdiplus::Graphics& g, int cx, int cy);
    void DrawQRCodeGraphic(Gdiplus::Graphics& g, int cx, int cy);
    void DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy);

    Gdiplus::RectF m_rectCancelBtn;
    int m_nCountdown;

    // add start logic for Async QR Refresh NTTai 20260123
    CString m_strQRData; 
    bool m_bIsLoading;
    void RefreshQRCode();
    static UINT __cdecl QrFetcherThreadProc(LPVOID pParam);
    // add end logic for Async QR Refresh NTTai 20260123
};
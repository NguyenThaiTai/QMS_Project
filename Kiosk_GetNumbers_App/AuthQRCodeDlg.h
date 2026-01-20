// AuthQRCodeDlg.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "resource.h" 
#include "ButtonUI.h"

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
    DECLARE_MESSAGE_MAP()

private:
    // add start drawing modules for QR UI components NTTai 20260108
    void DrawInstructions(Gdiplus::Graphics& g, int cx, int cy);
    void DrawQRCodeGraphic(Gdiplus::Graphics& g, int cx, int cy);
    void DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy);
    // add end drawing modules for QR UI components NTTai 20260108

    Gdiplus::RectF m_rectCancelBtn;
    int m_nCountdown;

    // add start logic for QR Refresh NTTai 20260108
    CString m_strQRData;    // Variable to store current QR code content NTTai 20260108
    void RefreshQRCode();   // Function to perform data refresh NTTai 20260108
    CString GetQRCodeUrlFromServer(); // add fetch URL from API NTTai 20260115
    CString ParseUrlFromJson(CString strJson); // add parse JSON response NTTai 20260115
    // add end logic for QR Refresh NTTai 20260108
};
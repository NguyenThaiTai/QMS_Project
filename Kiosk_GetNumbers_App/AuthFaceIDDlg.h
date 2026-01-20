// AuthFaceIDDlg.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "resource.h" 
#include "ButtonUI.h"

class AuthFaceIDDlg : public CDialogEx
{
    DECLARE_DYNAMIC(AuthFaceIDDlg) // add corrected class name NTTai 20260115

public:
    AuthFaceIDDlg(CWnd* pParent = nullptr);
    virtual ~AuthFaceIDDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_AUTH_FACEID_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()

private:
    // add start drawing modules for FaceID UI NTTai 20260115
    void DrawInstructions(Gdiplus::Graphics& g, int cx, int cy);
    void DrawFaceScannerGraphic(Gdiplus::Graphics& g, int cx, int cy);
    void DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy);
    // add end drawing modules for FaceID UI NTTai 20260115

    Gdiplus::RectF m_rectCancelBtn;

    // add start animation variables for scanning beam NTTai 20260115
    float m_fScanPos;    // Current Y-axis position of the scanning beam
    bool  m_bScanDown;   // Movement direction (true = down, false = up)
    // add end animation variables for scanning beam NTTai 20260115

};
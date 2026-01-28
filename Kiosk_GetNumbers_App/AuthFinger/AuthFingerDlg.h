// DlgFingerAuth.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h"
#include "../Common/ButtonUI.h"
#include "../Common/HeaderUI.h"
#include <iostream>

class CButtonUI;
class AuthFingerDlg : public CDialogEx
{
    DECLARE_DYNAMIC(AuthFingerDlg)

public:
    AuthFingerDlg(CWnd* pParent = nullptr);
    ~AuthFingerDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_AUTH_FINGER_DIALOG };
#endif
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor); // add set text color NTTai 20251231 
    afx_msg void OnTimer(UINT_PTR nIDEvent); // add set date-time NTTai 20250106
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    // --- 5 DRAWING MODULES
    void DrawInstructions(Gdiplus::Graphics& g, int cx, int cy); 
    void DrawPulseCircle(Gdiplus::Graphics& g, int cx, int cy); 
    void DrawMainCircle(Gdiplus::Graphics& g, int cx, int cy);
    void DrawFingerIcon(Gdiplus::Graphics& g, int cx, int cy);
    void DrawStatusLabel(Gdiplus::Graphics& g, int cx, int cy);
    
    Gdiplus::Image* LoadPNGFromResource(UINT nIDResource);

    float m_fPulseAlpha;
    bool  m_bPulseGrowing;

	// add start member variable NTTai 20260106
	AuthFingerDlg* m_pAuthFingerDlg;
    Gdiplus::Image* m_pIconFinger;
    CFont m_fontTitle;
    CFont m_fontSub;
    CFont m_fontStatus;
    Gdiplus::RectF m_rectCancelBtn;
	// add end member variable NTTai 20260106
protected:

};


// DlgFingerAuth.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "resource.h" 
#include "ButtonUI.h"

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
    

    // --- PRIVATE HELPER FUNCTIONS (Hàm tiện ích nội bộ) ---
    Gdiplus::Image* LoadPNGFromResource(UINT nIDResource);

    // --- ANIMATION STATE ---
    float m_fPulseAlpha;   // Độ trong suốt (0-255)
    bool  m_bPulseGrowing; // Trạng thái đang to ra hay nhỏ lại

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


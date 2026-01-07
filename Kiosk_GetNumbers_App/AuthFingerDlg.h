// DlgFingerAuth.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "resource.h" 

class CKioskGetNumbersAppDlg;
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
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    // --- 5 DRAWING MODULES (Mô-đun vẽ giao diện) ---
    void DrawInstructions(Gdiplus::Graphics& g, int cx, int cy); // 1. Vẽ chữ hướng dẫn
    void DrawPulseCircle(Gdiplus::Graphics& g, int cx, int cy);  // 2. Vẽ vòng tròn tỏa sáng
    void DrawMainCircle(Gdiplus::Graphics& g, int cx, int cy);   // 3. Vẽ vòng tròn trắng
    void DrawFingerIcon(Gdiplus::Graphics& g, int cx, int cy);   // 4. Vẽ icon vân tay
    void DrawStatusLabel(Gdiplus::Graphics& g, int cx, int cy);  // 5. Vẽ nhãn trạng thái bo góc
    void DrawCancelButton(Gdiplus::Graphics& g, int cx, int cy);

    // --- PRIVATE HELPER FUNCTIONS (Hàm tiện ích nội bộ) ---
    Gdiplus::Image* LoadPNGFromResource(UINT nIDResource);
    void AddRoundedRectToPath(Gdiplus::GraphicsPath& path, Gdiplus::RectF rect, float radius);

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


#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "resource.h" 
#include "ButtonUI.h"
#include "HeaderUI.h"
#include "Common.h" // add include shared data structure NTTai 20260114

#define WM_USER_SCAN_COMPLETE (WM_USER + 100)

// add define auth states for hardware integration NTTai 20260114
enum AuthState {
    STATE_WAITING_CARD, // Waiting for card placement (Default)
    STATE_PROCESSING    // Reading chip/processing (Loading)
};

class AuthIDCardDlg : public CDialogEx
{
    DECLARE_DYNAMIC(AuthIDCardDlg)

public:
    AuthIDCardDlg(CWnd* pParent = nullptr);
    virtual ~AuthIDCardDlg();

    void SetAuthState(AuthState state); // add set state method for hardware adapter NTTai 20260114

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_AUTH_ID_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg LRESULT OnScanComplete(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

private:
    // add start drawing methods based on state NTTai 20260114
    void DrawInstructions(Gdiplus::Graphics& g, int cx, int cy);
    void DrawIDCardGraphic(Gdiplus::Graphics& g, int cx, int cy);
    void DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy);
    void DrawProgressBar(Gdiplus::Graphics& g, int cx, int cy);
    void DrawLoadingFooter(Gdiplus::Graphics& g, int cx, int cy);
    // add end drawing methods based on state NTTai 20260114
    void StartScanProcess(); // add start scanning process (Test/Real) NTTai 20260114
    static UINT __cdecl ScanThreadProc(LPVOID pParam);
    void DrawTestButton(Gdiplus::Graphics& g, int cx, int cy);
    void DrawDeleteButton(Gdiplus::Graphics& g, int cx, int cy);

    AuthState m_eCurrentState; // add current state variable NTTai 20260114
    float m_fProgressVal;      // add progress bar value (0.0 -> 100.0) NTTai 20260114
    Gdiplus::RectF m_rectCancelBtn;
    Gdiplus::Image* m_pIconIDCard;
    float m_fScanPos;
    bool  m_bScanDown;
    CitizenCardData m_scannedData;
	// add start button variables for test data NTTai 20260114
    Gdiplus::RectF m_rectTestBtn;
    bool m_bTestBtnPressed = false;
    Gdiplus::RectF m_rectDeleteBtn;
    bool m_bDeleteBtnPressed = false;
    bool m_bProgressIncreasing;
	// add end button variables for test data NTTai 20260114
    
public:
    CitizenCardData GetScannedData() const { return m_scannedData; } // add getter for scanned data NTTai 20260114
};
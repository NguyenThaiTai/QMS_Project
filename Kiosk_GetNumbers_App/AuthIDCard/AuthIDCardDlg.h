#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h" 
#include "../Common/ButtonUI.h"
#include "../Common/HeaderUI.h"
#include "../Common/Common.h" // add include shared data structure NTTai 20260114
#include "../DatabaseManager/DatabaseManager.h"
#include "../Common/AuthSuccess/AuthCorrect.h"
#include "../NoAuth/NoAuthDlg.h"
//#include "FakeCCCDReader.h" // add include fake reader for simulation NTTai 20260114
// add start include hardware adapter classes NTTai 20260130
#include "../Interface/Common/InterfaceAdapterDevice.h"
#include "../Interface/Common/DeviceFactory.h"
// add end include hardware adapter classes NTTai 20260130

#define WM_USER_SCAN_COMPLETE (WM_USER + 100)

// add define auth states for hardware integration NTTai 20260114
enum AuthState {
    STATE_WAITING_CARD, // Waiting for card placement (Default)
    STATE_PROCESSING    // Reading chip/processing (Loading)
};

class AuthIDCardDlg : public CDialogEx, public IDeviceListener
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
    afx_msg void OnDestroy();
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
    //static UINT __cdecl ScanThreadProc(LPVOID pParam);
    void DrawTestButton(Gdiplus::Graphics& g, int cx, int cy);
    void DrawDeleteButton(Gdiplus::Graphics& g, int cx, int cy);
	bool ValidateCCCD(const CString& strCCCD); // add implementation of CCCD validation logic NTTai 20260126

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

	IDeviceAdapter* m_pDevice; // add device adapter for hardware integration NTTai 20260130
    
public:
    CitizenCardData GetScannedData() const { return m_scannedData; } // add getter for scanned data NTTai 20260114

	// add start IDeviceListener implementation NTTai 20260130
    virtual void OnDeviceConnected() override;
    virtual void OnDeviceDisconnected() override;
    virtual void OnScanSuccess(const CitizenCardData& data) override;
    virtual void OnScanError(CString strError) override;
	// add end IDeviceListener implementation NTTai 20260130
};
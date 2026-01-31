// DlgFingerAuth.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h"
#include "../Common/ButtonUI.h"
#include "../Common/HeaderUI.h"
#include <iostream>
// add start include hardware adapter classes NTTai 20260131
#include "../Interface/Common/InterfaceAdapterDevice.h"
#include "../Interface/Common/DeviceFactory.h"
// add end include hardware adapter classes NTTai 20260131

#include "../DatabaseManager/DatabaseManager.h"
#include "../Common/AuthSuccess/AuthCorrect.h"

#define WM_USER_FINGER_SCAN_COMPLETE (WM_USER + 102)

class CButtonUI;
class AuthFingerDlg : public CDialogEx, public IDeviceListener
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
    afx_msg void OnDestroy();
    afx_msg LRESULT OnScanComplete(WPARAM wParam, LPARAM lParam);
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

    IDeviceAdapter* m_pDevice; // add start device adapter instance NTTai 20260131
protected:
    // add start implement IDeviceListener interface NTTai 20260131
    virtual void OnDeviceConnected() override;
    virtual void OnDeviceDisconnected() override;
    virtual void OnScanSuccess(const CitizenCardData& data) override;
    virtual void OnScanError(CString strError) override;
    // add end implement IDeviceListener interface NTTai 20260131
};


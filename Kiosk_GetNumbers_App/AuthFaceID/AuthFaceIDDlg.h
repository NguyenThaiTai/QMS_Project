// AuthFaceIDDlg.h
#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h"
#include "../Common/ButtonUI.h"
#include "../Common/HeaderUI.h"
// add start include hardware adapter classes NTTai 20260131
#include "../Interface/Common/InterfaceAdapterDevice.h"
#include "../Interface/Common/DeviceFactory.h"
// add end include hardware adapter classes NTTai 20260131
#include "../Common/AuthSuccess/AuthCorrect.h" // add include auth success dialog NTTai 20260131
#include "../DatabaseManager/DatabaseManager.h" // add include database manager NTTai 20260131

#define WM_USER_FACEID_SCAN_COMPLETE (WM_USER + 101)

class AuthFaceIDDlg : public CDialogEx, public IDeviceListener
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
	afx_msg void OnDestroy();
    afx_msg LRESULT OnScanComplete(WPARAM wParam, LPARAM lParam); // add message handler for scan complete NTTai 20260131
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

	IDeviceAdapter* m_pDevice; // add device adapter instance NTTai 20260131

protected:
	// add start implement IDeviceListener interface NTTai 20260131
    virtual void OnDeviceConnected() override;
    virtual void OnDeviceDisconnected() override;
    virtual void OnScanSuccess(const CitizenCardData& data) override;
    virtual void OnScanError(CString strError) override;
	// add end implement IDeviceListener interface NTTai 20260131
};
// Kiosk_GetNumbers_AppDlg.h : header file
#pragma once

// add start include authentication dialog headers NTTai 20260123
#include "AuthScreen.h"
#include "AuthFingerDlg.h" 
#include "AuthIDCardDlg.h" 
#include "AuthFaceIDDlg.h" 
#include "AuthQRCodeDlg.h" 
#include "NoAuthDlg.h"     
// add end include authentication dialog headers NTTai 20260123

// CKioskGetNumbersAppDlg dialog
class CKioskGetNumbersAppDlg : public CDialogEx
{
	// Construction
public:
	CKioskGetNumbersAppDlg(CWnd* pParent = nullptr);	// standard constructor

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_KIOSK_GETNUMBERS_APP_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();

	// add start event handlers for UI logic NTTai 20260123
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnButtonAuthClicked(UINT nID);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	// add end event handlers for UI logic NTTai 20260123

	DECLARE_MESSAGE_MAP()

	// add start UI resources and font objects NTTai 20260123
	CStatic m_ctrlLogo;
	CBitmap m_bitmapLogo;
	CFont	m_fontTitle;
	CFont	m_fontAgribank;
	CStatic* m_pStaticAgribank;
	// add end UI resources and font objects NTTai 20260123

	// add start authentication screen controller NTTai 20260123
	AuthScreen* m_pAuthScreen;
	CStatic* m_pStaticDateTime;
	CFont    m_fontDateTime;
	// add end authentication screen controller NTTai 20260123
};
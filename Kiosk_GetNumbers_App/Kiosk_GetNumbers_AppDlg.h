
// Kiosk_GetNumbers_AppDlg.h : header file
#pragma once
// add start NTTai
#include "AuthScreen.h"
#include "AuthFingerDlg.h" // add include auth finger dialog NTTai 20260105
#include "AuthIDCardDlg.h" // add include auth ID card dialog NTTai 20260107
#include "AuthFaceIDDlg.h" // add include auth Face ID dialog NTTai 20260107
#include "AuthQRCodeDlg.h" // add include auth QR Code dialog NTTai 20260107
#include "NoAuthDlg.h"     // add include no auth dialog NTTai 20260107
// add end NTTai

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
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor); // add set text color NTTai 20251231 
	afx_msg void OnTimer(UINT_PTR nIDEvent); // add set date-time NTTai 20250102
	afx_msg void OnButtonAuthClicked(UINT nID); 
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

	// add start NTTai 20251231
	CStatic m_ctrlLogo;
	CBitmap m_bitmapLogo;
	CFont	m_fontTitle;
	CFont	m_fontAgribank;
	CStatic* m_pStaticAgribank;
	// add end NTTai 20251231

	// add start NTTai 20260201
	AuthScreen* m_pAuthScreen;
	CStatic* m_pStaticDateTime; // add Display Date-Time control
	CFont    m_fontDateTime; // add Date-Time font
	// add end NTTai 20260201
};

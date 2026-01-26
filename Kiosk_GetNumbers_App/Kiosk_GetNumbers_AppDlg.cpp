// Kiosk_GetNumbers_AppDlg.cpp : implementation file
#include "pch.h"
#include "framework.h"
#include "Kiosk_GetNumbers_App.h"
#include "Kiosk_GetNumbers_AppDlg.h"
#include "afxdialogex.h"
#include "HeaderUI.h" 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// add start define authentication command IDs NTTai 20260123
#define ID_AUTH_IDCARD   2000
#define ID_AUTH_FINGER   2001
#define ID_AUTH_FACEID   2002
#define ID_AUTH_QRCODE   2003
#define ID_AUTH_NORMAL   2004
// add end define authentication command IDs NTTai 20260123

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CKioskGetNumbersAppDlg::CKioskGetNumbersAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KIOSK_GETNUMBERS_APP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAuthScreen = nullptr;
	m_pStaticAgribank = nullptr;
	m_pStaticDateTime = nullptr;
}

void CKioskGetNumbersAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKioskGetNumbersAppDlg, CDialogEx)

	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(2000, 2004, &CKioskGetNumbersAppDlg::OnButtonAuthClicked)
END_MESSAGE_MAP()

// add start prevent background erase flickering NTTai 20260123
BOOL CKioskGetNumbersAppDlg::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
}
// add end prevent background erase flickering NTTai 20260123

BOOL CKioskGetNumbersAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// add start initialize screen settings NTTai 20260123
	CHeaderUI::SetFullScreen(this);
	SetTimer(1, 1000, NULL);
	// add end initialize screen settings NTTai 20260123

	// add start Init Auth Screen Component NTTai 20260123
	CRect l_rectDlg;
	GetClientRect(&l_rectDlg);
	m_pAuthScreen = new AuthScreen(this);
	m_pAuthScreen->CreateUI(l_rectDlg);
	// add end Init Auth Screen Component NTTai 20260123

	Invalidate();
	UpdateWindow();
	return TRUE;
}

void CKioskGetNumbersAppDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);

	if (IsIconic())
	{
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		// add start Double Buffering Implementation NTTai 20260123
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		CBitmap* pOldBmp = memDC.SelectObject(&bmp);

		// Initialize GDI+ on Memory DC
		Gdiplus::Graphics g(memDC.GetSafeHdc());
		g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

		// Clear background with theme color
		g.Clear(Gdiplus::Color(255, 235, 235, 235));

		// Draw Shared Header (Logo + Clock) to Memory DC
		CHeaderUI::DrawSharedHeader(&memDC, rect);

		// Copy from Memory DC to Screen
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		// Cleanup
		memDC.SelectObject(pOldBmp);
		// add end Double Buffering Implementation NTTai 20260123
	}
}

HCURSOR CKioskGetNumbersAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// add start handle text color and background for controls NTTai 20260123
HBRUSH CKioskGetNumbersAppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int l_nID = pWnd->GetDlgCtrlID();

	// Check for Auth buttons (ID 4000-4004)
	if (l_nID >= 4000 && l_nID <= 4004)
	{
		if (l_nID == 4004) // Special color for Cancel/Normal button
		{
			pDC->SetTextColor(RGB(255, 255, 255));
			pDC->SetBkColor(RGB(162, 32, 45));
			static CBrush s_brushRed(RGB(162, 32, 45));
			return s_brushRed;
		}
		else // Standard white buttons
		{
			pDC->SetTextColor(RGB(94, 107, 126));
			pDC->SetBkColor(RGB(255, 255, 255));
			static CBrush s_brushWhite(RGB(255, 255, 255));
			return s_brushWhite;
		}
	}
	// Transparent background for title labels
	if (l_nID == IDC_STATIC_TITLE ||
		l_nID == 9992 ||
		l_nID == 9993)
	{
		pDC->SetTextColor(RGB(147, 30, 31));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return hbr;
}
// add end handle text color and background for controls NTTai 20260123

// add start Display Date-time update timer NTTai 20260123
void CKioskGetNumbersAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		// Invalidate only the clock area to optimize performance
		CRect rectTime(rectClient.Width() - 400, 140, rectClient.Width(), 210);
		InvalidateRect(&rectTime, FALSE);
	}
	CDialogEx::OnTimer(nIDEvent);
}
// add end Display Date-time update timer NTTai 20260123

// add start handle authentication button clicks NTTai 20260123
void CKioskGetNumbersAppDlg::OnButtonAuthClicked(UINT nID)
{
	switch (nID)
	{
	case ID_AUTH_FINGER:
	{
		AuthFingerDlg authFinger_dlg(this);
		authFinger_dlg.DoModal();
		break;
	}

	case ID_AUTH_IDCARD:
	{
		AuthIDCardDlg authCard_dlg(this);
		authCard_dlg.DoModal();
		break;
	}

	case ID_AUTH_FACEID:
	{
		AuthFaceIDDlg authFace_dlg(this);
		authFace_dlg.DoModal();
		break;
	}

	case ID_AUTH_QRCODE:
	{
		AuthQRCodeDlg authQR_dlg(this);
		authQR_dlg.DoModal();
		break;
	}

	case ID_AUTH_NORMAL:
	{
		NoAuthDlg authNormal_dlg(this);
		authNormal_dlg.DoModal();
		break;
	}
	default:
		break;
	}
}
// add end handle authentication button clicks NTTai 20260123
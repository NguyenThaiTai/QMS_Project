
// Kiosk_GetNumbers_AppDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "Kiosk_GetNumbers_App.h"
#include "Kiosk_GetNumbers_AppDlg.h"
#include "afxdialogex.h"
#include "HeaderUI.h" // add include header UI NTTai 20260106

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation 
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CKioskGetNumbersAppDlg dialog
CKioskGetNumbersAppDlg::CKioskGetNumbersAppDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_KIOSK_GETNUMBERS_APP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKioskGetNumbersAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CKioskGetNumbersAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CTLCOLOR() // add set text color NTTai 20251231
	ON_STN_CLICKED(IDC_STATIC_LOGO, &CKioskGetNumbersAppDlg::OnStnClickedStaticLogo)
	ON_WM_TIMER() // add display date-time NTTai 20260102
	ON_COMMAND_RANGE(2000, 2004, &CKioskGetNumbersAppDlg::OnButtonAuthClicked) // add click button event NTTai 20260105
END_MESSAGE_MAP()


// CKioskGetNumbersAppDlg message handlers
BOOL CKioskGetNumbersAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// add start draw header UI NTTai 20260106
	CHeaderUI::SetFullScreen(this); // add set full screen NTTai 20260601
	SetTimer(1, 1000, NULL);
	// add end draw header UI NTTai 20260106
	
	// add start Init Auth Screen NTTai 20260102
	CRect l_rectDlg;
	GetClientRect(&l_rectDlg);
	m_pAuthScreen = new AuthScreen(this);
	m_pAuthScreen->CreateUI(l_rectDlg);
	// add end Init Auth Screen NTTai 20260102
	Invalidate();  
	UpdateWindow();
	return TRUE; 
}


void CKioskGetNumbersAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CKioskGetNumbersAppDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);
	if (IsIconic())
	{
		 // device context for painting
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		CHeaderUI::DrawSharedHeader(&dc, rect); // add draw header UI NTTai 20260105
	}
}

HCURSOR CKioskGetNumbersAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CKioskGetNumbersAppDlg::OnStnClickedStaticLogo()
{
	
}

// add set text color NTTai 20251231
HBRUSH CKioskGetNumbersAppDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int l_nID = pWnd->GetDlgCtrlID();
	if (l_nID >= 4000 && l_nID <= 4004)
	{
		if (l_nID == 4004) 
		{
			pDC->SetTextColor(RGB(255, 255, 255)); 
			pDC->SetBkColor(RGB(162, 32, 45));   
			static CBrush s_brushRed(RGB(162, 32, 45));
			return s_brushRed;
		}
		else
		{
			pDC->SetTextColor(RGB(94, 107, 126));
			pDC->SetBkColor(RGB(255, 255, 255)); 
			static CBrush s_brushWhite(RGB(255, 255, 255));
			return s_brushWhite; 
		}
	}
	if (l_nID == IDC_STATIC_TITLE	|| 
		l_nID == 9992				||
		l_nID == 9993)
	{
		pDC->SetTextColor(RGB(147, 30, 31));
		pDC->SetBkMode(TRANSPARENT);
		return (HBRUSH)GetStockObject(NULL_BRUSH);
	}
	return hbr;
}
// add end set text color NTTai 20251231

// add start Display Date-time NTTai 20260106
void CKioskGetNumbersAppDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		CRect rectTime(rectClient.Width() - 400, 140, rectClient.Width(), 210);
		InvalidateRect(&rectTime, TRUE);
	}
	CDialogEx::OnTimer(nIDEvent);
}
// add end Display Date-time NTTai 20260106

// add start handle button click event NTTai 20260105
void CKioskGetNumbersAppDlg::OnButtonAuthClicked(UINT nID)
{
	int nIndex = nID - 2000;
	if (nIndex == 1)
	{
		AuthFingerDlg authFinger_dlg(this);
		authFinger_dlg.DoModal(); 
	}

	//if (nIndex == 0) AfxMessageBox(_T("Bạn chọn CCCD"));
}
// add end handle button click event NTTai 20260105

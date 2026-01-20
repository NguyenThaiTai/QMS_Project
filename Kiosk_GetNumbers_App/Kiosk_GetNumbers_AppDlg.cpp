
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

// add start define authentication IDs NTTai 20260107
#define ID_AUTH_IDCARD   2000
#define ID_AUTH_FINGER   2001
#define ID_AUTH_FACEID   2002
#define ID_AUTH_QRCODE   2003
#define ID_AUTH_NORMAL   2004
// add end define authentication IDs NTTai 20260107

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

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX){}

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
	m_pAuthScreen = nullptr;
	m_pStaticAgribank = nullptr;
	m_pStaticDateTime = nullptr;
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
	ON_WM_TIMER() // add display date-time NTTai 20260102
	ON_COMMAND_RANGE(2000, 2004, &CKioskGetNumbersAppDlg::OnButtonAuthClicked) // add click button event NTTai 20260105
END_MESSAGE_MAP()

// add start prevent background erase flickering NTTai 20260106
BOOL CKioskGetNumbersAppDlg::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
}
// add end prevent background erase flickering NTTai 20260106

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
	CPaintDC dc(this); // device context for painting
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
		// [FIX START] Áp dụng Double Buffering để sửa lỗi hiển thị giờ bị chồng hình NTTai 20260112

		// 1. Tạo Memory DC (Vùng vẽ đệm)
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		CBitmap* pOldBmp = memDC.SelectObject(&bmp);

		// 2. Khởi tạo GDI+ trên Memory DC
		Gdiplus::Graphics g(memDC.GetSafeHdc());
		g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

		// 3. XÓA SẠCH NỀN CŨ (Quan trọng nhất để không bị đè chữ)
		// Dùng màu xám nhạt đồng bộ với các màn hình khác
		g.Clear(Gdiplus::Color(255, 235, 235, 235));

		// 4. Vẽ Header (Logo + Đồng hồ) lên Memory DC
		// Hàm DrawSharedHeader của bạn cần nhận CDC* hoặc Graphics*, 
		// ở đây code cũ bạn truyền &dc, giờ truyền &memDC để vẽ lên bộ đệm.
		CHeaderUI::DrawSharedHeader(&memDC, rect);

		// 5. Copy từ Memory DC ra màn hình thật (BitBlt)
		dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

		// Dọn dẹp
		memDC.SelectObject(pOldBmp);

	}
}

HCURSOR CKioskGetNumbersAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
		InvalidateRect(&rectTime, FALSE);
	}
	CDialogEx::OnTimer(nIDEvent);
}
// add end Display Date-time NTTai 20260106

// add start handle button click event NTTai 20260105
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
// add end handle button click event NTTai 20260105

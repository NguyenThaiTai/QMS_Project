#include "pch.h"
#include "AuthFingerDlg.h"
//#include "Kiosk_GetNumbers_AppDlg.h"
#include "HeaderUI.h"
#include <iostream>


IMPLEMENT_DYNAMIC(AuthFingerDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthFingerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR() // add set text color NTTai 20251231
	ON_WM_TIMER() // add display date-time NTTai 20260106
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// add start prevent background erase flickering NTTai 20260106
BOOL AuthFingerDlg::OnEraseBkgnd(CDC* pDC) {
	return TRUE;
}
// add end prevent background erase flickering NTTai 20260106

AuthFingerDlg::AuthFingerDlg(CWnd* pParent)
	: CDialogEx(IDD_AUTH_FINGER_DIALOG, pParent)
{
	m_pIconFinger = nullptr;
	m_fPulseAlpha = 50.0f;
	m_bPulseGrowing = true;
}

AuthFingerDlg::~AuthFingerDlg()
{
	if (m_pIconFinger)
		delete m_pIconFinger;
}

void AuthFingerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
  
}

BOOL AuthFingerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// add start set font NTTai 20260107
	m_fontTitle.CreatePointFont(280, _T("Arial Bold"));
	m_fontSub.CreatePointFont(120, _T("Arial"));
	m_fontStatus.CreatePointFont(110, _T("Arial Bold"));
	// add end set font NTTai 20260107

	m_pIconFinger = LoadPNGFromResource(IDB_PNG_FINGER); // add load finger icon NTTai 20260106

	// add start draw header UI NTTai 20260501
	CHeaderUI::SetFullScreen(this); // add set full screen NTTai 20260601
	SetTimer(1, 1000, NULL);
	// add end draw header UI NTTai 20260501
	SetTimer(2, 40, NULL);   // add start set effect timer (25 FPS) NTTai 20260106
    return TRUE;
}

void AuthFingerDlg::OnPaint()
{
	CPaintDC dc(this);
	CRect rect;
	GetClientRect(&rect);

	// add start Initialize Memory DC and Bitmap for double buffering NTTai 20260106
 	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap memBitmap;
	memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);
	// add end Initialize Memory DC and Bitmap for Double Buffering NTTai 20260106

	// add start setup GDI+ Graphics object linked to the Memory DC  NTTai 20260106
	Gdiplus::Graphics g(memDC.GetSafeHdc());
	g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
	g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
	g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
	g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
	// add end setup GDI+ Graphics object linked to the Memory DC  NTTai 20260106

	// add start clear background NTTai 20260106
	g.Clear(Gdiplus::Color(255, 235, 235, 235));
	// add end clear background NTTai 20260106
	
	// add start draw UI components to the memory buffer NTTai 20260106
	CHeaderUI::DrawSharedHeader(&memDC, rect); //draw the shared Header(Logo, Agribank, Clock)
	int cx = rect.Width() / 2;
	int cy = rect.Height() / 2 + 80;
	DrawInstructions(g, cx, cy);
	DrawPulseCircle(g, cx, cy);
	DrawMainCircle(g, cx, cy);
	DrawFingerIcon(g, cx, cy);
	DrawStatusLabel(g, cx, cy);
	CButtonUI::DrawCancelButton(g, cx, cy, m_rectCancelBtn);
	// add end draw UI components to the memory buffer NTTai 20260106

	// add start copy the entire buffer to the screen in one atomic operation NTTai 20260106
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	// add end copy the entire buffer to the screen in one atomic operation NTTai 20260106
	
	memDC.SelectObject(pOldBitmap); // Cleanup GDI objects
}

// add start set text color NTTai 20251231
HBRUSH AuthFingerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	int l_nID = pWnd->GetDlgCtrlID();
	return hbr;
}
// add end set text color NTTai 20251231


void AuthFingerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// add start display Date-time NTTai 20260102
	if (nIDEvent == 1)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		CRect rectTime(rectClient.Width() - 400, 140, rectClient.Width(), 210);
		InvalidateRect(&rectTime, FALSE);
	}
	// add end display Date-time NTTai 20260102

	// add start set effect timer NTTai 20260106
	if (nIDEvent == 2)
	{
		float step = 5.0f; // increase speed for smoother effect
		if (m_bPulseGrowing) {
			m_fPulseAlpha += step;
			if (m_fPulseAlpha >= 150.0f) m_bPulseGrowing = false;
		}
		else {
			m_fPulseAlpha -= step;
			if (m_fPulseAlpha <= 20.0f) m_bPulseGrowing = true;
		}
		CRect rect; 
		GetClientRect(&rect);
		int cx = rect.Width() / 2;
		int cy = rect.Height() / 2 + 80;
		CRect rInvalid(cx - 180, cy - 180, cx + 180, cy + 180);
		InvalidateRect(&rInvalid, FALSE);
	}
	// add end set effect timer NTTai 20260106
	CDialogEx::OnTimer(nIDEvent);
}

Gdiplus::Image* AuthFingerDlg::LoadPNGFromResource(UINT nIDResource)
{
	HRSRC hRes = FindResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(nIDResource), _T("PNG"));
	if (!hRes) return nullptr;

	DWORD dwSize = SizeofResource(AfxGetInstanceHandle(), hRes);
	HGLOBAL hResData = LoadResource(AfxGetInstanceHandle(), hRes);
	if (!hResData) return nullptr;

	void* pBuffer = LockResource(hResData);
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, dwSize);
	if (!hMem) return nullptr;

	void* pData = GlobalLock(hMem);
	memcpy(pData, pBuffer, dwSize);
	GlobalUnlock(hMem);

	IStream* pStream = nullptr;
	Gdiplus::Image* pFinalImg = nullptr;

	if (CreateStreamOnHGlobal(hMem, TRUE, &pStream) == S_OK)
	{
		Gdiplus::Image* pTempImg = Gdiplus::Image::FromStream(pStream);
		if (pTempImg && pTempImg->GetLastStatus() == Gdiplus::Ok)
		{
			pFinalImg = pTempImg->Clone();
			delete pTempImg;
		}
		pStream->Release();
	}
	return pFinalImg;
}



// add start draw instruction title NTTai 20260106
void AuthFingerDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy)
{
	Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 30, 30, 30));
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);

	Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
	g.DrawString(L"Vui lòng đặt vân tay của bạn", -1, &fontTitle,
		Gdiplus::PointF((float)cx, (float)cy - 300), &format, &blackBrush);

	Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);
	Gdiplus::SolidBrush grayBrush(Gdiplus::Color(255, 120, 120, 120));
	g.DrawString(L"Hệ thống đang quét... Giữ ngón tay ổn định.", -1, &fontSub,
		Gdiplus::PointF((float)cx, (float)cy - 200), &format, &grayBrush);
}
// add end draw instruction title NTTai 20260106

// add start draw pulse animation circle NTTai 20260106
void AuthFingerDlg::DrawPulseCircle(Gdiplus::Graphics& g, int cx, int cy)
{
	// Màu đỏ Agribank với độ trong suốt thay đổi (m_fPulseAlpha)
	Gdiplus::SolidBrush pulseBrush(Gdiplus::Color((int)m_fPulseAlpha, 162, 32, 45));

	int size = 280; // Kích thước vòng tỏa sáng
	g.FillEllipse(&pulseBrush, cx - size / 2, cy - size / 2, size, size);
}
// add end draw pulse animation circle NTTai 20260106

// add start draw main white circle NTTai 20260106
void AuthFingerDlg::DrawMainCircle(Gdiplus::Graphics& g, int cx, int cy)
{
	int size = 200;
	Gdiplus::SolidBrush whiteBrush(Gdiplus::Color::White);
	g.FillEllipse(&whiteBrush, cx - size / 2, cy - size / 2, size, size);

	// Viền đỏ mỏng xung quanh
	Gdiplus::Pen penBorder(Gdiplus::Color(255, 162, 32, 45), 2.0f);
	g.DrawEllipse(&penBorder, cx - size / 2, cy - size / 2, size, size);
}
// add end draw main white circle NTTai 20260106

// add start draw fingerprint icon NTTai 20260106
void AuthFingerDlg::DrawFingerIcon(Gdiplus::Graphics& g, int cx, int cy)
{
	if (!m_pIconFinger) return;

	float iconSize = 110.0f;
	float x = (float)cx - iconSize / 2.0f;
	float y = (float)cy - iconSize / 2.0f;

	// Agribank red matrix, preserve Alpha 1.0f for smooth edges
	Gdiplus::ColorMatrix colorMatrix = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.63f, 0.12f, 0.17f, 0.0f, 1.0f
	};

	Gdiplus::ImageAttributes imAttr;
	imAttr.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

	// Offset array for 3-pass drawing (Center + side offsets)
	Gdiplus::PointF offsets[] = {
		{0.0f, 0.0f},    // Center
		{-0.15f, 0.0f},  // Left offset
		{0.15f, 0.0f}    // Right offset
	};

	for (int i = 0; i < 3; i++) {
		g.DrawImage(m_pIconFinger,
			Gdiplus::RectF(x + offsets[i].X, y + offsets[i].Y, iconSize, iconSize),
			0, 0, (float)m_pIconFinger->GetWidth(), (float)m_pIconFinger->GetHeight(),
			Gdiplus::UnitPixel, &imAttr);
	}
}

// add start draw status label NTTai 20260106
void AuthFingerDlg::DrawStatusLabel(Gdiplus::Graphics& g, int cx, int cy)
{
	Gdiplus::RectF rectLabel((float)cx - 220, (float)cy + 180, 440.0f, 60.0f);
	Gdiplus::GraphicsPath path;
	CButtonUI::AddRoundedRectToPath(path, rectLabel, 30.0f);

	g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);

	Gdiplus::Font fontStatus(L"Segoe UI", 12, Gdiplus::FontStyleBold);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	g.DrawString(L"Đặt ngón tay lên cảm biến để xác thực", -1, &fontStatus,
		rectLabel, &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)));
}
// add end draw status label NTTai 20260106


// add start handle mouse click on cancel button NTTai 20260106
void AuthFingerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	Gdiplus::PointF mousePt((float)point.x, (float)point.y);
	if (m_rectCancelBtn.Contains(mousePt))
	{
		EndDialog(IDCANCEL);
	}
	CDialogEx::OnLButtonUp(nFlags, point);
}
// add end handle mouse click on cancel button NTTai 20260106
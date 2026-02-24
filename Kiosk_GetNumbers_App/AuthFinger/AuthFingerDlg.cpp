#include "pch.h"
#include "AuthFingerDlg.h"
//#include "Kiosk_GetNumbers_AppDlg.h"

#define DEVICE_NAME L"Finger_Device.dll"

IMPLEMENT_DYNAMIC(AuthFingerDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthFingerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR() // add set text color NTTai 20251231
	ON_WM_TIMER() // add display date-time NTTai 20260106
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONUP()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER_FINGER_SCAN_COMPLETE, &AuthFingerDlg::OnScanComplete)
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
	m_bRegisterMode = false; // add init register mode flag NTTai 20260203
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
	SetIcon(m_hIcon, TRUE);
	SetIcon(m_hIcon, FALSE);

	// add start set font NTTai 20260107
	m_fontTitle.CreatePointFont(280, _T("Arial Bold"));
	m_fontSub.CreatePointFont(120, _T("Arial"));
	m_fontStatus.CreatePointFont(110, _T("Arial Bold"));
	// add end set font NTTai 20260107

	m_pIconFinger = Common::LoadPNGFromResource(IDB_PNG_FINGER); // add load finger icon NTTai 20260106

	// add start draw header UI NTTai 20260501
	CHeaderUI::SetFullScreen(this); // add set full screen NTTai 20260601
	SetTimer(1, 1000, NULL);
	// add end draw header UI NTTai 20260501
	SetTimer(2, 40, NULL);   // add start set effect timer (25 FPS) NTTai 20260106

	CString strDllName = DEVICE_NAME;
	m_pDevice = DeviceFactory::CreateAdapterFromDLL(strDllName);

	if (m_pDevice) {
		m_pDevice->RegisterListener(this); // Đăng ký nhận sự kiện
		m_pDevice->Initialize();           // Khởi động thiết bị
		m_pDevice->StartScanning();        // Bắt đầu quét
	}
	else {
		AfxMessageBox(L"Lỗi: Không tìm thấy Module Fingerprint! (.dll file)");
	}
	// add start register auth state observer NTTai 20260211
	AuthStateManager::GetInstance()->RegisterObserver(this);
	AuthStateManager::GetInstance()->Reset();
	// add end register auth state observer NTTai 20260211
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
	int cy = rect.Height() / 2 + 60;
	AuthState currentState = AuthStateManager::GetInstance()->GetState(); // add get current auth state NTTai 20260211
	DrawInstructions(g, cx, cy, currentState);
	DrawPulseCircle(g, cx, cy, currentState);
	DrawMainCircle(g, cx, cy, currentState);
	DrawFingerIcon(g, cx, cy, currentState);
	DrawStatusLabel(g, cx, cy, currentState);
	// add end draw UI components to the memory buffer NTTai 20260106
	
	// add start draw cancel button only in specific states NTTai 20260211
	if (currentState == STATE_WAITING_SCAN || currentState == STATE_ERROR) {
		CButtonUI::DrawCancelButton(g, cx, cy, m_rectCancelBtn);
	}
	// add end draw cancel button only in specific states NTTai 20260211
	
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
	
	// add start handle SUCCESS and ERROR timers NTTai 20260211
	else if (nIDEvent == 3) { // Success Timer
		KillTimer(3);
		EndDialog(IDOK);
	}

	else if (nIDEvent == 4) { // Error Timer
		KillTimer(4);
		AuthStateManager::GetInstance()->Reset();
		if (m_pDevice) m_pDevice->StartScanning();
	}
	// add end handle SUCCESS and ERROR timers NTTai 20260211\

	CDialogEx::OnTimer(nIDEvent);
}

// add start draw instruction title NTTai 20260106
void AuthFingerDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
	// add start draw title and subtitle NTTai 20260203
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);

	Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
	Gdiplus::SolidBrush blackTitleBrush(Gdiplus::Color(255, 0, 0, 0));

	Gdiplus::Font fontSub(L"Segoe UI", 14, Gdiplus::FontStyleRegular);
	Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 80, 80, 80));

	CString strTitle;
	CString strSub;

	if (m_bRegisterMode)
	{
		strTitle = L"Đăng ký vân tay mới";
	}
	else
	{
		strTitle = L"Xác thực vân tay";
	}
	// add start set subtitle based on state NTTai 20260203
	if (state == STATE_ERROR) {
		strSub = L"Không nhận diện được, vui lòng thử lại";
	}
	else if (state == STATE_SUCCESS) {
		strSub = L"Hoàn tất!";
	}
	else {
		strSub = L"Vui lòng đặt ngón tay để hệ thống ghi nhận mẫu vân tay";
	}
	// add end set subtitle based on state NTTai 20260203
	g.DrawString(strTitle, -1, &fontTitle, Gdiplus::PointF((float)cx, (float)cy - 350), &format, &blackTitleBrush);
	g.DrawString(strSub, -1, &fontSub, Gdiplus::PointF((float)cx, (float)cy - 240), &format, &subBrush);
	// add end draw title and subtitle NTTai 20260203
}
// add end draw instruction title NTTai 20260106

// add start draw pulse animation circle NTTai 20260106
void AuthFingerDlg::DrawPulseCircle(Gdiplus::Graphics& g, int cx, int cy, AuthState state	)
{
	if (state != STATE_WAITING_SCAN && state != STATE_SCANNING) return; // add only draw pulse in waiting and scanning states

	Gdiplus::SolidBrush pulseBrush(Gdiplus::Color((int)m_fPulseAlpha, 162, 32, 45));
	int size = 280; 
	g.FillEllipse(&pulseBrush, cx - size / 2, cy - size / 2, size, size);
}
// add end draw pulse animation circle NTTai 20260106

// add start draw main white circle NTTai 20260106
void AuthFingerDlg::DrawMainCircle(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
	int size = 200;
	Gdiplus::SolidBrush whiteBrush(Gdiplus::Color::White);
	g.FillEllipse(&whiteBrush, cx - size / 2, cy - size / 2, size, size);
	// add start draw border color based on state NTTai 20260211
	Gdiplus::Color borderColor;
	switch (state) {
		case STATE_SUCCESS: borderColor = Gdiplus::Color(255, 40, 167, 69); break; // Success Green 
		case STATE_ERROR:   borderColor = Gdiplus::Color(255, 220, 53, 69); break; // Error Red
		case STATE_VERIFYING: borderColor = Gdiplus::Color(255, 0, 120, 215); break; // Verifying Blue
		default:            borderColor = Gdiplus::Color(255, 162, 32, 45); break; // Default Red
	}
	// add end draw border color based on state NTTai 20260211
	Gdiplus::Pen penBorder(borderColor, 4.0f);
	g.DrawEllipse(&penBorder, cx - size / 2, cy - size / 2, size, size);
}
// add end draw main white circle NTTai 20260106

// add start draw fingerprint icon NTTai 20260106
void AuthFingerDlg::DrawFingerIcon(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
	if (!m_pIconFinger) return;

	float iconSize = 110.0f;
	float x = (float)cx - iconSize / 2.0f;
	float y = (float)cy - iconSize / 2.0f;

	Gdiplus::ColorMatrix colorMatrix = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.63f, 0.12f, 0.17f, 0.0f, 1.0f
	};

	Gdiplus::ImageAttributes imAttr;
	imAttr.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

	Gdiplus::PointF offsets[] = {
		{0.0f, 0.0f},
		{-0.15f, 0.0f},
		{0.15f, 0.0f}
	};

	for (int i = 0; i < 3; i++) {
		g.DrawImage(m_pIconFinger,
					Gdiplus::RectF(x + offsets[i].X, y + offsets[i].Y, iconSize, iconSize),
					0,
					0, 
					(float)m_pIconFinger->GetWidth(), 
					(float)m_pIconFinger->GetHeight(),
					Gdiplus::UnitPixel, &imAttr);
	}
}
// add end draw fingerprint icon NTTai 20260106

// add start draw status label NTTai 20260106
void AuthFingerDlg::DrawStatusLabel(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{	
	Gdiplus::RectF rectLabel((float)cx - 220, (float)cy + 180, 440.0f, 60.0f);
	Gdiplus::GraphicsPath path;
	CButtonUI::AddRoundedRectToPath(path, rectLabel, 30.0f);

	g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);
	// add start fix status text and color NTTai 20260211
	Gdiplus::Color textColor;
	CString strStatus;
	switch (state) {
	case STATE_WAITING_SCAN:
		textColor = Gdiplus::Color(255, 162, 32, 45); // Red Agribank
		strStatus = L"Đang chờ lấy mẫu vân tay...";
		break;
	case STATE_SCANNING:
		textColor = Gdiplus::Color(255, 255, 140, 0); // Organge
		strStatus = L"Đang quét vân tay...";
		break;
	case STATE_VERIFYING:
		textColor = Gdiplus::Color(255, 0, 100, 200); // Blue
		strStatus = L"Đang kiểm tra dữ liệu...";
		break;
	case STATE_SUCCESS:
		textColor = Gdiplus::Color(255, 40, 167, 69); // Green
		strStatus = L"Xác thực thành công!";
		break;
	case STATE_ERROR:
		textColor = Gdiplus::Color(255, 220, 53, 69); // Red
		strStatus = AuthStateManager::GetInstance()->GetErrorMessage();
		if (strStatus.IsEmpty()) strStatus = L"Lỗi xác thực!";
		break;
	}
	Gdiplus::Font fontStatus(L"Segoe UI", 14, Gdiplus::FontStyleBold);
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	g.DrawString(strStatus, -1, &fontStatus, rectLabel, &format, &Gdiplus::SolidBrush(textColor));
	// add end fix status text and color NTTai 20260211
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

// add start handle clean up resources NTTai 20260131
void AuthFingerDlg::OnDestroy() {
	if (m_pDevice) {
		m_pDevice->Release();
		delete m_pDevice;
		m_pDevice = nullptr;
	}
	AuthStateManager::GetInstance()->UnregisterObserver(this); // add unregister auth state observer NTTai 20260211
	CDialogEx::OnDestroy();
}
// add end handle clean up resources NTTai 20260131

// add start handle scan completion logic (Thread) NTTai 20260131
LRESULT AuthFingerDlg::OnScanComplete(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 1) {
		CitizenCardData* pData = (CitizenCardData*)lParam;
		if (pData) {
			AuthStateManager::GetInstance()->SetState(STATE_VERIFYING); // add set state to verifying NTTai 20260211

			DatabaseManager db;
			bool bIsNew = false;
			bool bSaveOK = false; // add save customer data to database NTTai 20260211

			if (db.InitializeDB()) {
				if (!db.IsCustomerExist(pData->strIDNumber)) bIsNew = true;
				bSaveOK = db.SaveCustomer(*pData); // add save customer data to database NTTai 20260211
				db.CloseDB();
			}
			// add start show success or error dialog NTTai 20260211
			if (bSaveOK) {
				AuthStateManager::GetInstance()->SetState(STATE_SUCCESS); // add set state to success NTTai 20260211
				SetTimer(3, 500, NULL); // add delay before showing success dialog NTTai 20260211
				// add start show correct dialog NTTai 20260211
				AuthCorrect dlgCorrect(pData->strFullName, bIsNew, this);
				dlgCorrect.SetAuthData(*pData);
				dlgCorrect.DoModal();
				// add end show correct dialog NTTai 20260211
				EndDialog(IDOK);
			}
			else {
				AuthStateManager::GetInstance()->SetState(STATE_ERROR, L"Lỗi lưu dữ liệu!"); // add set state to error NTTai 20260211
			}
			// add end show success or error dialog NTTai 20260211
			delete pData;
		}
	}
	else if (wParam == 0) {
		CString* pStrError = (CString*)lParam;
		CString strMsg = (pStrError && !pStrError->IsEmpty())
			? *pStrError
			: L"Lỗi nhận diện: Không thể xác thực vân tay. Vui lòng thử lại.";

		AuthStateManager::GetInstance()->SetState(STATE_ERROR, strMsg); // add set state to error NTTai 20260211
		AfxMessageBox(strMsg, MB_ICONERROR);

		if (pStrError) delete pStrError;

		if (m_pDevice) {
			m_pDevice->StartScanning();
		}
	}
	return 0;
}
// add end handle scan completion logic (Thread) NTTai 20260131

// add start implement IDeviceListener interface (Worker Thread) NTTai 20260131
void AuthFingerDlg::OnDeviceConnected() {}

void AuthFingerDlg::OnDeviceDisconnected() {}

void AuthFingerDlg::OnScanSuccess(const CitizenCardData& data) {
	CitizenCardData* pDataCopy = new CitizenCardData(data);
	PostMessage(WM_USER_FINGER_SCAN_COMPLETE, (WPARAM)1, (LPARAM)pDataCopy);
}

void AuthFingerDlg::OnScanError(CString strError) {
	CString* pErrStr = new CString(strError);
	PostMessage(WM_USER_FINGER_SCAN_COMPLETE, (WPARAM)0, (LPARAM)pErrStr);
}
// add end implement IDeviceListener interface (Worker Thread) NTTai 20260131

// add implement auth state observer NTTai 20260211
void AuthFingerDlg::OnAuthStateChanged(AuthState newState, CString strMessage)
{
	KillTimer(3);
	KillTimer(4);

	switch (newState)
	{
	case STATE_WAITING_SCAN:
		m_fPulseAlpha = 50.0f;
		SetTimer(2, 40, NULL);
		break;

	case STATE_SUCCESS:
		break;

	case STATE_ERROR:
		SetTimer(4, 3000, NULL);
		break;
	}
	Invalidate(FALSE);
}
// add end implement auth state observer NTTai 20260211
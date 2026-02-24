// AuthFaceIDDlg.cpp
#include "pch.h"
#include "AuthFaceIDDlg.h"

#define DEVICE_NAME L"FaceID_Device.dll"

IMPLEMENT_DYNAMIC(AuthFaceIDDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthFaceIDDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
    ON_WM_DESTROY()
    ON_MESSAGE(WM_USER_FACEID_SCAN_COMPLETE, &AuthFaceIDDlg::OnScanComplete)
END_MESSAGE_MAP()

// add corrected constructor NTTai 20260115
AuthFaceIDDlg::AuthFaceIDDlg(CWnd* pParent)
    : CDialogEx(IDD_AUTH_FACEID_DIALOG, pParent)
{
    // add start initialize animation variables NTTai 20260115
    m_fScanPos = 0.0f;
    m_bScanDown = true;
    // add end initialize animation variables NTTai 20260115
	m_bRegisterMode = false; // add initialize register mode flag NTTai 20260203
}

AuthFaceIDDlg::~AuthFaceIDDlg()
{
}

BOOL AuthFaceIDDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; } // add disable background erasing to prevent flickering NTTai 20260115

BOOL AuthFaceIDDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    // add start initialize timers and screen mode NTTai 20260115
    CHeaderUI::SetFullScreen(this);
    SetTimer(1, 1000, NULL); // Clock timer
    SetTimer(2, 30, NULL);   // Animation timer (Reserved for FaceID)
    // add end initialize timers and screen mode NTTai 20260115

	// add start register auth state observer and start scanning NTTai 20260211
    AuthStateManager::GetInstance()->RegisterObserver(this);
    AuthStateManager::GetInstance()->Reset();
    AuthStateManager::GetInstance()->SetState(STATE_SCANNING);
	// add end register auth state observer and start scanning NTTai 20260211
    
    // add start initialize FaceID device adapter NTTai 20260131 
    CString strDllName = DEVICE_NAME;
    m_pDevice = DeviceFactory::CreateAdapterFromDLL(strDllName);
    if (m_pDevice) {
        m_pDevice->RegisterListener(this);
        m_pDevice->Initialize();
        m_pDevice->StartScanning();
    }
    else {
        AfxMessageBox(L"Lỗi: Không tìm thấy Module FaceID! (.dll file)");
    }
    return TRUE;
	// add end initialize FaceID device adapter NTTai 20260131
}

void AuthFaceIDDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);   

    // add start initialize memory DC and bitmap for double buffering NTTai 20260115
    CDC memDC;
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp;
    bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&bmp);
    // add end initialize memory DC and bitmap for double buffering NTTai 20260115

    // add start configure high quality GDI+ graphics NTTai 20260115
    Gdiplus::Graphics g(memDC.GetSafeHdc());
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    // add end configure high quality GDI+ graphics NTTai 20260115

    // add start draw background and shared header NTTai 20260115
    g.Clear(Gdiplus::Color(255, 235, 235, 235)); // Gray background matching theme
    CHeaderUI::DrawSharedHeader(&memDC, rect); // Draw Agribank Logo and Clock
    // add end draw background and shared header NTTai 20260115

    // add start placeholder for FaceID UI components NTTai 20260115
    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2 + 40; // Keep center balanced
	AuthState currentState = AuthStateManager::GetInstance()->GetState(); // add get current auth state NTTai 20260211
    DrawInstructions(g, cx, cy, currentState);
    DrawFaceScannerGraphic(g, cx, cy, currentState);
    DrawStatusBox(g, cx, cy, currentState); // Draw the bottom instruction note NTTai 20260115
	// add start draw cancel button NTTai 20260115
    if (currentState != STATE_SUCCESS) {
        CButtonUI::DrawCancelButton(g, cx, cy, m_rectCancelBtn);
    }
	// add end draw cancel button NTTai 20260115
    // add end placeholder for FaceID UI components NTTai 20260115

    // add copy buffer to screen and clean up NTTai 20260115
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void AuthFaceIDDlg::OnTimer(UINT_PTR nIDEvent)
{
    // add start handle clock updates NTTai 20260115
    if (nIDEvent == 1)
    {
        CRect rectClient;
        GetClientRect(&rectClient);
        CRect rectTime(rectClient.Width() - 400, 140, rectClient.Width(), 210);
        InvalidateRect(&rectTime, FALSE);
    }
    // add end handle clock updates NTTai 20260115

    // add start handle scanning beam animation logic NTTai 20260115
    else if (nIDEvent == 2)
    {
        float speed = 4.0f;
        float scannerSize = 320.0f;
        if (m_bScanDown) {
            m_fScanPos += speed;
            if (m_fScanPos >= scannerSize) m_bScanDown = false;
        }
        else {
            m_fScanPos -= speed;
            if (m_fScanPos <= 0.0f) m_bScanDown = true;
        }

        // add start fix invalidation area to match shifted coordinates NTTai 20260115
        CRect rect; GetClientRect(&rect);
        int cx = rect.Width() / 2;
        int cy = rect.Height() / 2 + 40;
        int circleCenterY = cy + 10;
        int radius = 210;

        CRect rInvalid(cx - radius, circleCenterY - radius, cx + radius, circleCenterY + radius);
        InvalidateRect(&rInvalid, FALSE);
        // add end fix invalidation area to match shifted coordinates NTTai 20260115
    }
    // add end handle scanning beam animation logic NTTai 20260115
	// add start handle state transition timers NTTai 20260211
    else if (nIDEvent == 3) { // Timer Success 
        KillTimer(3);
        if (!m_scannedData.strIDNumber.IsEmpty())
        {
            AuthCorrect dlgCorrect(m_scannedData.strFullName, false, this);
            dlgCorrect.SetAuthData(m_scannedData);
            dlgCorrect.DoModal();
            EndDialog(IDOK);
        }
    }

    else if (nIDEvent == 4) { // Timer Error -> Reset
        KillTimer(4);
        AuthStateManager::GetInstance()->Reset();
        AuthStateManager::GetInstance()->SetState(STATE_SCANNING);
        if (m_pDevice) m_pDevice->StartScanning();
    }
	// add end handle state transition timers NTTai 20260211
    CDialogEx::OnTimer(nIDEvent);
}

void AuthFaceIDDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_rectCancelBtn.Contains(Gdiplus::PointF((float)point.x, (float)point.y))) {
        EndDialog(IDCANCEL); // add close dialog when cancel button is clicked NTTai 20260115
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

void AuthFaceIDDlg::OnDestroy() {
    if (m_pDevice) {
        m_pDevice->Release();
        delete m_pDevice;
        m_pDevice = nullptr;
    }
	AuthStateManager::GetInstance()->UnregisterObserver(this); // add unregister auth state observer NTTai 20260211
    CDialogEx::OnDestroy();
}

void AuthFaceIDDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackTitleBrush(Gdiplus::Color(255, 0, 0, 0));
    Gdiplus::Font fontSub(L"Segoe UI", 14, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 80, 80, 80));

	// add start dynamic title and subtitle based on mode NTTai 20260203
    CString strTitle;
    CString strSub;
    if (m_bRegisterMode)
    {
        strTitle = L"Đăng ký khuôn mặt mới";
        strSub = L"Vui lòng nhìn thẳng vào camera để hệ thống ghi nhận khuôn mặt";    
    }
    else
    {
        strTitle = L"Xác thực FaceID";
        strSub = L"Vui lòng nhìn thẳng vào camera để xác thực";
    }
	// add start adjust title based on auth state NTTai 20260211
    if (state == STATE_SUCCESS) strTitle = L"Hoàn tất!";
    else if (state == STATE_ERROR) strTitle = L"Thử lại...";
	// add end adjust title based on auth state NTTai 20260211

    g.DrawString(strTitle, -1, &fontTitle, Gdiplus::PointF((float)cx, (float)cy - 350), &format, &blackTitleBrush);
    g.DrawString(strSub, -1, &fontSub, Gdiplus::PointF((float)cx, (float)cy - 270), &format, &subBrush);
	// add end dynamic title and subtitle based on mode NTTai 20260203
}

void AuthFaceIDDlg::DrawFaceScannerGraphic(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
    float scannerRadius = 160.0f;
    float scannerSize = scannerRadius * 2.0f;

    // add shift the entire scanner assembly down by 50px for better balance NTTai 20260115
    Gdiplus::RectF scanRect((float)cx - scannerRadius, (float)cy - 150.0f, scannerSize, scannerSize);

	// add start determine theme color based on auth state NTTai 20260115
    Gdiplus::Color themeColor;
	if (state == STATE_SUCCESS) themeColor = Gdiplus::Color(255, 40, 167, 69);          // Sucess Green
	else if (state == STATE_ERROR) themeColor = Gdiplus::Color(255, 220, 53, 69);       // Error Red
	else if (state == STATE_VERIFYING) themeColor = Gdiplus::Color(255, 0, 120, 215);   // Verifying Blue  
	else themeColor = Gdiplus::Color(255, 162, 32, 45);                                 // Red Orange (Scanning)
	// add end determine theme color based on auth state NTTai 20260115
    
    // add start draw brackets relative to the new scanRect position NTTai 20260115
    Gdiplus::Pen framePen(Gdiplus::Color(255, 162, 32, 45), 5.0f);
    framePen.SetStartCap(Gdiplus::LineCapRound);
    framePen.SetEndCap(Gdiplus::LineCapRound);

    float gap = 30.0f;
    float len = 40.0f;
    Gdiplus::RectF frameRect = scanRect;
    frameRect.Inflate(gap, gap);

    g.DrawLine(&framePen, frameRect.X, frameRect.Y, frameRect.X + len, frameRect.Y);
    g.DrawLine(&framePen, frameRect.X, frameRect.Y, frameRect.X, frameRect.Y + len);
    g.DrawLine(&framePen, frameRect.GetRight(), frameRect.Y, frameRect.GetRight() - len, frameRect.Y);
    g.DrawLine(&framePen, frameRect.GetRight(), frameRect.Y, frameRect.GetRight(), frameRect.Y + len);
    g.DrawLine(&framePen, frameRect.X, frameRect.GetBottom(), frameRect.X + len, frameRect.GetBottom());
    g.DrawLine(&framePen, frameRect.X, frameRect.GetBottom(), frameRect.X, frameRect.GetBottom() - len);
    g.DrawLine(&framePen, frameRect.GetRight(), frameRect.GetBottom(), frameRect.GetRight() - len, frameRect.GetBottom());
    g.DrawLine(&framePen, frameRect.GetRight(), frameRect.GetBottom(), frameRect.GetRight(), frameRect.GetBottom() - len);
    // add end draw brackets relative to the new scanRect position NTTai 20260115

	g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(20, themeColor.GetR(), themeColor.GetG(), themeColor.GetB())), scanRect); // add dark transparent fill to the circle NTTai 20260211

    Gdiplus::GraphicsPath clipPath;
    clipPath.AddEllipse(scanRect);
    g.SetClip(&clipPath);

    // add start draw silhouette adjusted to the new Y coordinate NTTai 20260115
    Gdiplus::SolidBrush silhouetteBrush(Gdiplus::Color(80, 180, 180, 180));
    float headRadius = headRadius = 70.0f;
    g.FillEllipse(&silhouetteBrush, cx - headRadius, scanRect.Y + 40.0f, headRadius * 2.0f, headRadius * 2.0f);
    float bodyWidth = 220.0f;
    float bodyHeight = 180.0f;
    g.FillEllipse(&silhouetteBrush, cx - bodyWidth / 2.0f, scanRect.Y + 170.0f, bodyWidth, bodyHeight);
    // add end draw silhouette NTTai 20260115

    // add start draw dual-gradient beam within the shifted scanRect NTTai 20260115
	if (state == STATE_SCANNING || state == STATE_VERIFYING) // add only draw beam when scanning or verifying NTTai 20260211
    {
        float beamHalfHeight = 35.0f;
        float currentY = scanRect.Y + m_fScanPos;
        Gdiplus::RectF topHalf(scanRect.X, currentY - beamHalfHeight, scanRect.Width, beamHalfHeight);
        Gdiplus::LinearGradientBrush topBrush(topHalf, Gdiplus::Color(0, 162, 32, 45), Gdiplus::Color(200, 162, 32, 45), Gdiplus::LinearGradientModeVertical);
        g.FillRectangle(&topBrush, topHalf);
        Gdiplus::RectF botHalf(scanRect.X, currentY, scanRect.Width, beamHalfHeight);
        Gdiplus::LinearGradientBrush botBrush(botHalf, Gdiplus::Color(200, 162, 32, 45), Gdiplus::Color(0, 162, 32, 45), Gdiplus::LinearGradientModeVertical);
        g.FillRectangle(&botBrush, botHalf);
		g.DrawLine(&Gdiplus::Pen(Gdiplus::Color(255, themeColor.GetR(), themeColor.GetG(), themeColor.GetB()), 2.0f), scanRect.X, currentY, scanRect.X + scanRect.Width, currentY); // add center line NTTai 20260211
    }
    // add end draw scanning beam NTTai 20260115

    g.ResetClip();

    g.DrawEllipse(&Gdiplus::Pen(Gdiplus::Color(255, 162, 32, 45), 5.0f), scanRect);
}


void AuthFaceIDDlg::DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
    // add start define box dimensions and position NTTai 20260115
    float boxWidth = 580.0f;
    float boxHeight = 50.0f;
    Gdiplus::RectF boxRect((float)cx - (boxWidth / 2.0f), (float)cy + 245.0f, boxWidth, boxHeight);
    // add end define box dimensions and position NTTai 20260115

    // add start draw white rounded container with light gray border NTTai 20260115
    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, boxRect, 25.0f); // Pill shape
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 229, 231, 235), 1.2f), &path);
    // add end draw white rounded container NTTai 20260115

	// add start determine text and icon color based on state NTTai 20260211
    CString strText;
    Gdiplus::Color iconColor;
    switch (state) {
    case STATE_SCANNING:
        strText = L"Giữ khuôn mặt trong khung hình để hệ thống nhận diện";
		iconColor = Gdiplus::Color(255, 162, 32, 45); // Red Orange
        break;
    case STATE_VERIFYING:
        strText = L"Đang kiểm tra dữ liệu khuôn mặt...";
		iconColor = Gdiplus::Color(255, 0, 120, 215); // Green Blue
        break;
    case STATE_SUCCESS:
        strText = L"Xác thực khuôn mặt thành công!";
        iconColor = Gdiplus::Color(255, 40, 167, 69); // Green 
        break;
    case STATE_ERROR:
        strText = AuthStateManager::GetInstance()->GetErrorMessage();
        if (strText.IsEmpty()) strText = L"Không nhận diện được khuôn mặt";
        iconColor = Gdiplus::Color(255, 220, 53, 69); // Red
        break;
    default:
        strText = L"Vui lòng nhìn vào Camera";
        iconColor = Gdiplus::Color::Gray;
    }
	// add end determine text and icon color based on state NTTai 20260211
    
    // add start draw small FaceID red icon NTTai 20260115
    float iconSize = 20.0f;
    float iconX = boxRect.X + 25.0f;
    float iconY = boxRect.Y + (boxHeight - iconSize) / 2.0f;
    Gdiplus::Pen iconPen(iconColor, 2.0f);
    g.DrawRectangle(&iconPen, (Gdiplus::REAL)iconX, (Gdiplus::REAL)iconY, (Gdiplus::REAL)iconSize, (Gdiplus::REAL)iconSize);
    g.DrawEllipse(&iconPen, (Gdiplus::REAL)(iconX + 4.0f), (Gdiplus::REAL)(iconY + 4.0f),
        12.0f, 12.0f);
    // add end draw small FaceID red icon NTTai 20260115

    // add start draw instruction text next to icon NTTai 20260115
    Gdiplus::Font fontDesc(L"Segoe UI", 11, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 30, 30, 30));
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentNear);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::RectF textRect(iconX + 35.0f, boxRect.Y, boxWidth - 60.0f, boxHeight);
    g.DrawString(strText, -1, &fontDesc, textRect, &format, &blackBrush);
    // add end draw instruction text NTTai 20260115
}

// add start handle scan completion logic NTTai 20260131
LRESULT AuthFaceIDDlg::OnScanComplete(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 1) {
        CitizenCardData* pData = (CitizenCardData*)lParam;
        if (pData) {
			AuthStateManager::GetInstance()->SetState(STATE_VERIFYING); // add set verifying state NTTai 20260211
			m_scannedData = *pData; // add store scanned data NTTai 20260211    
            DatabaseManager db;
            bool bIsNew = false;
            if (db.InitializeDB()) {
                if (!db.IsCustomerExist(pData->strIDNumber)) bIsNew = true;
                db.SaveCustomer(*pData);
                db.CloseDB();
            }
			AuthStateManager::GetInstance()->SetState(STATE_SUCCESS); // add set success state NTTai 20260211
			SetTimer(3, 1000, NULL); // add short delay before moving to next dialog NTTai 20260211
            EndDialog(IDOK);

            delete pData; 
        }
    }
    else if (wParam == 0) {
        CString* pStrError = (CString*)lParam;
        CString strMsg = (pStrError && !pStrError->IsEmpty()) ? *pStrError : L"Lỗi nhận diện: Không thể xác thực khuôn mặt. Vui lòng thử lại.";
        //AfxMessageBox(strMsg, MB_ICONERROR);
		AuthStateManager::GetInstance()->SetState(STATE_ERROR, strMsg); // add set error state NTTai 20260211
        if (pStrError) delete pStrError;
        /*if (m_pDevice) {
            m_pDevice->StartScanning();
        }*/
    }
    return 0;
}
// add end handle scan completion logic NTTai 20260131

// add start implement IDeviceListener interface NTTai 20260131
void AuthFaceIDDlg::OnDeviceConnected()  {
}

void AuthFaceIDDlg::OnDeviceDisconnected()  {
}

void AuthFaceIDDlg::OnScanSuccess(const CitizenCardData& data){
    CitizenCardData* pDataCopy = new CitizenCardData(data);
    PostMessage(WM_USER_FACEID_SCAN_COMPLETE, (WPARAM)1, (LPARAM)pDataCopy);
}

 void AuthFaceIDDlg::OnScanError(CString strError) {
     CString* pErrStr = new CString(strError);
     PostMessage(WM_USER_FACEID_SCAN_COMPLETE, (WPARAM)0, (LPARAM)pErrStr);
 }
 // add end implement IDeviceListener interface NTTai 20260131

 // add start implement auth state observer NTTai 20260211
 void AuthFaceIDDlg::OnAuthStateChanged(AuthState newState, CString strMessage)
 {
     KillTimer(3);
     KillTimer(4);

     switch (newState)
     {
     case STATE_SCANNING:
         SetTimer(2, 30, NULL);
         break;

     case STATE_SUCCESS:
         // Dừng hiệu ứng quét (hoặc để nó chạy tiếp tùy bạn)
         // KillTimer(2); 
         break;

     case STATE_ERROR:
         // KillTimer(2); // Dừng quét nếu lỗi
         SetTimer(4, 3000, NULL); // Tự reset sau 3s
         break;
     }
     Invalidate(FALSE);
 }
 // add end implement auth state observer NTTai 20260211
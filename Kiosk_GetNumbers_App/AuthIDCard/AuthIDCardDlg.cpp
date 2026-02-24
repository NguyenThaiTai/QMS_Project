#include "pch.h"
#include "AuthIDCardDlg.h"

#define DEVICE_NAME L"IDCard_Device.dll"

IMPLEMENT_DYNAMIC(AuthIDCardDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthIDCardDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_USER_SCAN_COMPLETE, &AuthIDCardDlg::OnScanComplete)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

AuthIDCardDlg::AuthIDCardDlg(CWnd* pParent)
    : CDialogEx(IDD_AUTH_ID_DIALOG, pParent)
{ 
    m_pIconIDCard = nullptr;
    m_fScanPos = 0.0f;
    m_bScanDown = true;
    m_bTestBtnPressed = false;
    m_bDeleteBtnPressed = false;
    m_fProgressVal = 0.0f;
    m_bProgressIncreasing = true;
	m_bForFingerRegister = false; // add init finger registration flag NTTai 20260203
	m_bForFaceRegister = false; // add init face ID registration flag NTTai 20260203
}

AuthIDCardDlg::~AuthIDCardDlg()
{
    if (m_pIconIDCard) delete m_pIconIDCard;
}

BOOL AuthIDCardDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL AuthIDCardDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);
    SetTimer(1, 1000, NULL); // add clock timer NTTai 20260114
    SetTimer(2, 30, NULL);   // add scan animation timer NTTai 20260114

	// add start device adapter initialization NTTai 20260130
    CString strDllName = DEVICE_NAME;

    m_pDevice = DeviceFactory::CreateAdapterFromDLL(strDllName);

    if (m_pDevice) {
        m_pDevice->RegisterListener(this);
        m_pDevice->Initialize();
    }
    else {
        AfxMessageBox(L"Lỗi: Không tìm thấy file Driver thiết bị (.dll)!");
    }

	// add start auth state manager registration NTTai 20260210
    AuthStateManager::GetInstance()->RegisterObserver(this);
    AuthStateManager::GetInstance()->Reset();
    // add end auth state manager registration NTTai 20260210
    
	//StartScanProcess(); For real device, scanning starts when device notifies connection
	// add end device adapter initialization NTTai 20260130

    return TRUE;
}

// add start set auth state helper NTTai 20260114
//void AuthIDCardDlg::SetAuthState(AuthState state)
//{
//    m_eCurrentState = state;
//    if (state == STATE_PROCESSING) {
//        m_fProgressVal = 0.0f;
//        m_bProgressIncreasing = true;
//        SetTimer(3, 40, NULL);
//    }
//    else {
//        KillTimer(3);
//    }
//    Invalidate(FALSE);
//}
// add end set auth state helper NTTai 20260114

void AuthIDCardDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);

    CDC memDC; 
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp; 
    bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&bmp);
    Gdiplus::Graphics g(memDC.GetSafeHdc());
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

    g.Clear(Gdiplus::Color(255, 235, 235, 235));
    CHeaderUI::DrawSharedHeader(&memDC, rect);

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2 + 60;

    AuthState currentState = AuthStateManager::GetInstance()->GetState(); // add get current auth state from manager NTTai 20260210

    DrawInstructions(g, cx, cy, currentState);
    DrawIDCardGraphic(g, cx, cy);

    if (currentState == STATE_SCANNING || currentState == STATE_VERIFYING) {
        DrawProgressBar(g, cx, cy);
    }
    DrawStatusBox(g, cx, cy, currentState);
    if (currentState == STATE_WAITING_SCAN) {
        CButtonUI::DrawCancelButton(g, cx, cy + 30, m_rectCancelBtn);
        DrawTestButton(g, cx, cy);
        DrawDeleteButton(g, cx, cy);
    }
    else {
        DrawLoadingFooter(g, cx, cy);
    }
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

// add start draw instructions based on state NTTai 20260210
void AuthIDCardDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);

    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0));        
	Gdiplus::SolidBrush redBrush(Gdiplus::Color(255, 162, 32, 45));
    Gdiplus::SolidBrush grayBrush(Gdiplus::Color(255, 80, 80, 80));

    CString strTitle;
    CString strSub;
    Gdiplus::Brush* pTitleBrush = &blackBrush;

    switch (state)
    {
    case STATE_WAITING_SCAN:
        strTitle = L"Vui lòng đặt căn cước công dân của bạn";
        strSub = L"Hệ thống sẽ tự động quét và nhận diện thông tin từ thẻ của quý khách";
        pTitleBrush = &blackBrush;
        break;

    case STATE_SCANNING:
        strTitle = L"Đang xác thực thông tin...";
        strSub = L"Vui lòng giữ nguyên thẻ trên đầu đọc và không di chuyển";
        pTitleBrush = &redBrush;
        break;

    case STATE_VERIFYING:
        strTitle = L"Đang kiểm tra dữ liệu...";
        strSub = L"Hệ thống đang kiểm tra tính hợp lệ và đối chiếu Căn cước công dân";
        pTitleBrush = &redBrush;
        break;

    case STATE_SUCCESS:
        strTitle = L"Xác thực thành công!";
        strSub = L"Thông tin hợp lệ. Đang chuyển hướng...";
        pTitleBrush = &redBrush;
        break;

    case STATE_ERROR:
        strTitle = L"Có lỗi xảy ra...";
        strSub = L"Không thể đọc thẻ hoặc dữ liệu không hợp lệ. Vui lòng thử lại.";
        pTitleBrush = &redBrush;
        break;

    default:
        strTitle = L"Vui lòng thử lại";
        strSub = L"Đã hết thời gian chờ, vui lòng thao tác lại.";
        pTitleBrush = &blackBrush;
        break;
    }           

    g.DrawString(strTitle, -1, &fontTitle,
        Gdiplus::PointF((float)cx, (float)cy - 340),
        &format, pTitleBrush);

    g.DrawString(strSub, -1, &fontSub,
        Gdiplus::PointF((float)cx, (float)cy - 250),
        &format, &grayBrush);
}
// add end draw instructions based on state NTTai 20260210

void AuthIDCardDlg::DrawIDCardGraphic(Gdiplus::Graphics& g, int cx, int cy)
{
    float cardW = 340.0f; 
    float cardH = 210.0f; 
    float cornerRadius = 15.0f;
    Gdiplus::RectF cardRect((float)cx - cardW / 2.0f, (float)cy - 110.0f, cardW, cardH);
    float frameW = 420.0f;
    float frameH = 300.0f;
    Gdiplus::RectF outerFrameRect((float)cx - frameW / 2.0f, (float)cy - 155.0f, frameW, frameH);

    Gdiplus::GraphicsPath outerPath; 
    CButtonUI::AddRoundedRectToPath(outerPath, outerFrameRect, 32.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 249, 250, 251)), &outerPath);
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 229, 231, 235), 2.0f), &outerPath);

    Gdiplus::Pen framePen(Gdiplus::Color(255, 162, 32, 45), 4.5f);
    framePen.SetStartCap(Gdiplus::LineCapRound); 
    framePen.SetEndCap(Gdiplus::LineCapRound);
    float len = 35.0f; 
    float gap = 20.0f;
    g.DrawLine(&framePen, cardRect.X - gap, cardRect.Y - gap, cardRect.X - gap + len, cardRect.Y - gap);
    g.DrawLine(&framePen, cardRect.X - gap, cardRect.Y - gap, cardRect.X - gap, cardRect.Y - gap + len);
    g.DrawLine(&framePen, cardRect.X + cardW + gap, cardRect.Y - gap, cardRect.X + cardW + gap - len, cardRect.Y - gap);
    g.DrawLine(&framePen, cardRect.X + cardW + gap, cardRect.Y - gap, cardRect.X + cardW + gap, cardRect.Y - gap + len);
    g.DrawLine(&framePen, cardRect.X - gap, cardRect.Y + cardH + gap, cardRect.X - gap + len, cardRect.Y + cardH + gap);
    g.DrawLine(&framePen, cardRect.X - gap, cardRect.Y + cardH + gap, cardRect.X - gap, cardRect.Y + cardH + gap - len);
    g.DrawLine(&framePen, cardRect.X + cardW + gap, cardRect.Y + cardH + gap, cardRect.X + cardW + gap - len, cardRect.Y + cardH + gap);
    g.DrawLine(&framePen, cardRect.X + cardW + gap, cardRect.Y + cardH + gap, cardRect.X + cardW + gap, cardRect.Y + cardH + gap - len);

    Gdiplus::RectF shadowRect = cardRect; 
    shadowRect.Y += 4.0f;
    Gdiplus::GraphicsPath shadowPath; 
    CButtonUI::AddRoundedRectToPath(shadowPath, shadowRect, cornerRadius);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(30, 0, 0, 0)), &shadowPath);
    Gdiplus::GraphicsPath cardPath; CButtonUI::AddRoundedRectToPath(cardPath, cardRect, cornerRadius);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &cardPath);
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 229, 231, 235), 1.2f), &cardPath);

    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(255, 240, 242, 245)), cardRect.X + 15.0f, cardRect.Y + 60.0f, 65.0f, 85.0f);
    Gdiplus::SolidBrush lineBrush(Gdiplus::Color(255, 210, 220, 235));
    g.FillRectangle(&lineBrush, cardRect.X + 95.0f, cardRect.Y + 70.0f, 120.0f, 8.0f);
    g.FillRectangle(&lineBrush, cardRect.X + 95.0f, cardRect.Y + 90.0f, 180.0f, 8.0f);
    g.FillRectangle(&lineBrush, cardRect.X + 95.0f, cardRect.Y + 110.0f, 150.0f, 8.0f);
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(255, 245, 215, 120)), cardRect.X + 95.0f, cardRect.Y + 135.0f, 25.0f, 20.0f);

    g.SetClip(&cardPath);
    Gdiplus::RectF beamRect(cardRect.X, cardRect.Y + m_fScanPos - 15, cardRect.Width, 30.0f);
    Gdiplus::LinearGradientBrush beamBrush(beamRect, Gdiplus::Color(0, 162, 32, 45), Gdiplus::Color(120, 162, 32, 45), Gdiplus::LinearGradientModeVertical);
    g.FillRectangle(&beamBrush, beamRect);
    Gdiplus::Pen scanLinePen(Gdiplus::Color(200, 162, 32, 45), 2.0f);
    g.DrawLine(&scanLinePen, cardRect.X, cardRect.Y + m_fScanPos, cardRect.X + cardRect.Width, cardRect.Y + m_fScanPos);
    g.ResetClip();
}

// add start draw progress bar NTTai 20260114
void AuthIDCardDlg::DrawProgressBar(Gdiplus::Graphics& g, int cx, int cy)
{
    float barW = 420.0f;
    float barH = 6.0f;
    float startX = cx - barW / 2.0f;
    float startY = (float)cy + 160.0f;

    // Draw background bar (Gray)
    Gdiplus::RectF bgRect(startX, startY, barW, barH);
    Gdiplus::GraphicsPath bgPath;
    CButtonUI::AddRoundedRectToPath(bgPath, bgRect, 3.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 230, 230, 230)), &bgPath);

    // Draw progress (Red)
    float currentW = (m_fProgressVal / 100.0f) * barW;
    if (currentW > barW) currentW = barW;
    if (currentW < 0) currentW = 0;

    Gdiplus::RectF fillRect(startX, startY, currentW, barH);

    if (currentW > 0) {
        Gdiplus::GraphicsPath fillPath;
        CButtonUI::AddRoundedRectToPath(fillPath, fillRect, 3.0f);
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)), &fillPath);
    }

    // Draw text "PROCESSING..."
    Gdiplus::Font font(L"Segoe UI", 9, Gdiplus::FontStyleBold);
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    g.DrawString(L"ĐANG XỬ LÝ DỮ LIỆU...",
        -1,
        &font,
        Gdiplus::PointF((float)cx, startY + 25.0f),
        &format,
        &Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)));
}
// add end draw progress bar NTTai 20260114

// add start draw status box NTTai 20260114
void AuthIDCardDlg::DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy, AuthState state)
{
    Gdiplus::RectF boxRect((float)cx - 240, (float)cy + 220, 480.0f, 100.0f);
    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, boxRect, 12.0f);

    Gdiplus::Font fontTitle(L"Segoe UI", 13, Gdiplus::FontStyleBold);
    Gdiplus::Font fontDesc(L"Segoe UI", 10, Gdiplus::FontStyleRegular);
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentNear);
    format.SetLineAlignment(Gdiplus::StringAlignmentNear);

    Gdiplus::RectF textTitleRect(boxRect.X + 65, boxRect.Y + 20, 395.0f, 30.0f);
    Gdiplus::RectF textDescRect(boxRect.X + 65, boxRect.Y + 50, 395.0f, 65.0f);

    Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 30, 30, 30));
    Gdiplus::SolidBrush descBrush(Gdiplus::Color(255, 100, 100, 100));

    Gdiplus::Color boxFillColor;
    Gdiplus::Color boxBorderColor;
    Gdiplus::Color iconColor;

    CString strTitle;
    CString strDesc;
    bool bShowLoadingIcon = false; // Cờ để vẽ icon xoay
    bool bShowCheckIcon = false;   // Cờ để vẽ dấu tích xanh

    switch (state)
    {
    case STATE_WAITING_SCAN:
        boxFillColor = Gdiplus::Color::White;
        boxBorderColor = Gdiplus::Color(255, 240, 240, 240);
        iconColor = Gdiplus::Color(255, 162, 32, 45); // Đỏ Agribank

        strTitle = L"Đặt thẻ CCCD lên đầu đọc";
        strDesc = L"Vui lòng giữ thẻ cố định cho đến khi đèn tín hiệu chuyển sang màu xanh.";
        break;

    case STATE_SCANNING:
        boxFillColor = Gdiplus::Color(255, 255, 250, 250);
        boxBorderColor = Gdiplus::Color(255, 250, 200, 200);
        iconColor = Gdiplus::Color(255, 255, 235, 235); // Nền icon nhạt

        strTitle = L"Đang đọc thông tin chip...";
        strDesc = L"Vui lòng không rút thẻ ra khỏi đầu đọc cho đến khi quá trình hoàn tất.";
        bShowLoadingIcon = true;
        break;

    case STATE_VERIFYING:
        boxFillColor = Gdiplus::Color(255, 245, 250, 255);
        boxBorderColor = Gdiplus::Color(255, 200, 220, 250);
        iconColor = Gdiplus::Color(255, 235, 240, 255);

        strTitle = L"Đang kiểm tra dữ liệu...";
        strDesc = L"Hệ thống đang đối soát thông tin của bạn với cơ sở dữ liệu.";
        bShowLoadingIcon = true;
        break;

    case STATE_SUCCESS:
        boxFillColor = Gdiplus::Color(255, 240, 255, 240);
        boxBorderColor = Gdiplus::Color(255, 150, 200, 150);
        iconColor = Gdiplus::Color(255, 40, 167, 69);

        strTitle = L"Đọc thẻ thành công!";
        strDesc = L"Dữ liệu hợp lệ. Đang chuyển hướng...";
        bShowCheckIcon = true;
        break;

    case STATE_ERROR:
        boxFillColor = Gdiplus::Color(255, 255, 245, 245);
        boxBorderColor = Gdiplus::Color(255, 255, 100, 100);
        iconColor = Gdiplus::Color(255, 220, 53, 69); // Đỏ Error

        strTitle = AuthStateManager::GetInstance()->GetErrorMessage();
        if (strTitle.IsEmpty()) strTitle = L"Không thể đọc thẻ";
        strDesc = L"Vui lòng kiểm tra lại thiết bị hoặc thẻ và thử lại.";
        break;

    default:
        boxFillColor = Gdiplus::Color::White;
        boxBorderColor = Gdiplus::Color::Gray;
        iconColor = Gdiplus::Color::Gray;
        strTitle = L"...";
        break;
    }

    g.FillPath(&Gdiplus::SolidBrush(boxFillColor), &path);
    g.DrawPath(&Gdiplus::Pen(boxBorderColor, 1.0f), &path);

    Gdiplus::RectF iconRect(boxRect.X + 20.0f, boxRect.Y + 35.0f, 30.0f, 30.0f);
    g.FillEllipse(&Gdiplus::SolidBrush(iconColor), iconRect);

    if (bShowLoadingIcon) {
        Gdiplus::Pen penRefresh(Gdiplus::Color(255, 162, 32, 45), 2.0f);
        g.DrawArc(&penRefresh, Gdiplus::REAL(iconRect.X + 8), Gdiplus::REAL(iconRect.Y + 8), 14.0f, 14.0f, 0, 270);
        g.DrawLine(&penRefresh, iconRect.X + 22, iconRect.Y + 8, iconRect.X + 22, iconRect.Y + 12);
    }
    else if (bShowCheckIcon) {
        Gdiplus::Pen penCheck(Gdiplus::Color::White, 2.5f);
        g.DrawLine(&penCheck, iconRect.X + 8, iconRect.Y + 15, iconRect.X + 13, iconRect.Y + 20);
        g.DrawLine(&penCheck, iconRect.X + 13, iconRect.Y + 20, iconRect.X + 22, iconRect.Y + 10);
    }

    g.DrawString(strTitle, -1, &fontTitle, textTitleRect, &format, &textBrush);
    g.DrawString(strDesc, -1, &fontDesc, textDescRect, &format, &descBrush);
}
// add end draw status box NTTai 20260210

// add start draw loading footer NTTai 20260114
void AuthIDCardDlg::DrawLoadingFooter(Gdiplus::Graphics& g, int cx, int cy)
{
    float btnW = 350.0f;
    float btnH = 50.0f;
    Gdiplus::RectF btnRect((float)cx - btnW / 2, (float)cy + 360, btnW, btnH);

    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, btnRect, 25.0f); // Rounded
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 200, 200, 200)), &path); // Gray border
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);    
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font font(L"Segoe UI", 12, Gdiplus::FontStyleBold);
    g.DrawString(L"Đang xác thực (Vui lòng đợi)", -1, &font, btnRect, &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 150, 150, 150))); // Gray text
}
// add end draw loading footer NTTai 20260114

void AuthIDCardDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1 || nIDEvent == 2) {
        if (nIDEvent == 1) {
            CRect rectClient;
            GetClientRect(&rectClient);
            CRect rectTime(rectClient.Width() - 400, 140, rectClient.Width(), 210);
            InvalidateRect(&rectTime, FALSE);
        }
        else if (nIDEvent == 2) {
            float speed = 3.5f;
            if (m_bScanDown) { 
                m_fScanPos += speed; 
                if (m_fScanPos >= 210.0f) m_bScanDown = false; 
            }
            else { 
                m_fScanPos -= speed; 
                if (m_fScanPos <= 0.0f) m_bScanDown = true; 
            }
            CRect rect; 
            GetClientRect(&rect);
            int cx = rect.Width() / 2; 
            int cy = rect.Height() / 2 + 60;
            CRect rInvalid(cx - 210, cy - 150, cx + 210, cy + 150);
            InvalidateRect(&rInvalid, FALSE);
        }
    }

    else if (nIDEvent == 3) {
        float step = 1.2f;
        if (m_bProgressIncreasing) {
            m_fProgressVal += step;
            if (m_fProgressVal >= 100.0f) {
                m_fProgressVal = 100.0f;
                m_bProgressIncreasing = false;
            }
        }
        else {
            m_fProgressVal -= step;
            if (m_fProgressVal <= 0.0f) {
                m_fProgressVal = 0.0f;
                m_bProgressIncreasing = true;
            }
        }
        CRect rect; 
        GetClientRect(&rect);
        int cx = rect.Width() / 2;
        int cy = rect.Height() / 2 + 60;
        CRect rProg(cx - 220, cy + 140, cx + 220, cy + 220);

        InvalidateRect(&rProg, FALSE);
    }

    else if (nIDEvent == 4) {
        KillTimer(4);
        AuthStateManager::GetInstance()->Reset(); // Gọi Singleton để reset
    }

    CDialogEx::OnTimer(nIDEvent);
}


// add start start scan process logic NTTai 20260114
void AuthIDCardDlg::StartScanProcess()
{
	AuthStateManager::GetInstance()->SetState(STATE_SCANNING); // add set global state NTTai 20260210
    //AfxBeginThread(ScanThreadProc, this);
	if (m_pDevice) m_pDevice->StartScanning(); // add start real scanning process NTTai 20260130
}
// add end start scan process logic NTTai 20260114

void AuthIDCardDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    AuthState state = AuthStateManager::GetInstance()->GetState();
    if (state == STATE_SCANNING || state == STATE_VERIFYING) return;

    Gdiplus::PointF p((float)point.x, (float)point.y);

    if (m_bTestBtnPressed && m_rectTestBtn.Contains(p)) {
        m_bTestBtnPressed = false;
        Invalidate(FALSE);
        StartScanProcess();
    }
    if (m_bDeleteBtnPressed && m_rectDeleteBtn.Contains(p)) {
        m_bDeleteBtnPressed = false; 
        Invalidate(FALSE);
        if (MessageBox(L"Xóa toàn bộ dữ liệu?", L"Cảnh báo", MB_YESNO | MB_ICONWARNING) == IDYES) {
            DatabaseManager db;
            if (db.InitializeDB()) {
                db.DeleteAllCustomers();
                db.CloseDB();
            }
        }
    }
    if (m_rectCancelBtn.Contains(p)) { EndDialog(IDCANCEL); }
    CDialogEx::OnLButtonUp(nFlags, point);
}

void AuthIDCardDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// add start prevent button press during scanning NTTai 20260210 
    AuthState state = AuthStateManager::GetInstance()->GetState();
    if (state == STATE_SCANNING || state == STATE_VERIFYING) return;
	// add end prevent button press during scanning NTTai 20260210

    Gdiplus::PointF p((float)point.x, (float)point.y);

    if (m_rectTestBtn.Contains(p)) {
        m_bTestBtnPressed = true;
        CRect rInv((int)m_rectTestBtn.X, (int)m_rectTestBtn.Y, (int)m_rectTestBtn.GetRight() + 1, (int)m_rectTestBtn.GetBottom() + 1);
        InvalidateRect(&rInv, FALSE);
    }

    if (m_rectDeleteBtn.Contains(p)) {
        m_bDeleteBtnPressed = true;
        CRect rInv((int)m_rectDeleteBtn.X, (int)m_rectDeleteBtn.Y, (int)m_rectDeleteBtn.GetRight() + 1, (int)m_rectDeleteBtn.GetBottom() + 1);
        InvalidateRect(&rInv, FALSE);
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

LRESULT AuthIDCardDlg::OnScanComplete(WPARAM wParam, LPARAM lParam)
{
    if (wParam == 1) {
        CitizenCardData* pData = (CitizenCardData*)lParam;
        if (pData) {
            // add start validate data before processing NTTai 20260126
            if (!ValidateCCCD(pData->strIDNumber)) {
                AfxMessageBox(L"Lỗi: Mã định danh không đúng định dạng chuẩn!");
                delete pData; 
                StartScanProcess();
                return 0;
            }
            // add end validate data before processing NTTai 20260126
            
            m_scannedData = *pData;
            DatabaseManager db;
            bool bIsNew = false;
            bool bSaveOK = false;
            if (db.InitializeDB()) {
                if (!db.IsCustomerExist(pData->strIDNumber)) bIsNew = true;
                bSaveOK = db.SaveCustomer(*pData);
                db.CloseDB();
            }

			AuthStateManager::GetInstance()->SetState(STATE_VERIFYING); // add set global state NTTai 20260210

            if (bSaveOK) {
				AuthStateManager::GetInstance()->SetState(STATE_SUCCESS); // add set global state NTTai 20260210

				// add start show correction dialog NTTai 20260203  
                AuthCorrect dlgCorrect(pData->strFullName, bIsNew, this);
                dlgCorrect.SetAuthData(*pData);
                if (m_bForFingerRegister || m_bForFaceRegister) {
                    dlgCorrect.DisableAutoRedirect();
                }
                dlgCorrect.DoModal();
                EndDialog(IDCANCEL);
				if (m_bForFingerRegister) { // add finger registration flow NTTai 20260203
                    //EndDialog(IDCANCEL);
                    AuthFingerDlg dlgFinger;
					dlgFinger.SetRegisterMode(true); // add set finger register mode NTTai 20260203
                    dlgFinger.DoModal();
                }
                else if (m_bForFaceRegister) { // add face ID registration flow NTTai 20260203
                    //EndDialog(IDCANCEL);
                    AuthFaceIDDlg dlg;
					dlg.SetRegisterMode(true); // add set face ID register mode NTTai 20260203
                    dlg.DoModal();
                }
                else {
                    EndDialog(IDOK);
                }
				// add end show correction dialog NTTai 20260203
            }
            else {
				AuthStateManager::GetInstance()->SetState(STATE_ERROR, L"Lỗi lưu DB"); // add set global state NTTai 20260210
                //AfxMessageBox(L"Lỗi: Không thể lưu thông tin vào hệ thống!");
            }
            delete pData;
        }
    }
    else {
        AuthStateManager::GetInstance()->SetState(STATE_ERROR, L"Lỗi đọc thẻ");
        //AfxMessageBox(L"Lỗi đọc thẻ! Vui lòng thử lại.");
    }
    return 0;
}

void AuthIDCardDlg::DrawTestButton(Gdiplus::Graphics& g, int cx, int cy)
{
    float btnW = 200.0f;
    float btnH = 50.0f;
    m_rectTestBtn = Gdiplus::RectF(((float)cx - btnW / 2) + 700, (float)cy + 280, btnW, btnH);

    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, m_rectTestBtn, 10.0f);

    Gdiplus::Color colorFill = m_bTestBtnPressed ? Gdiplus::Color(255, 0, 100, 200) : Gdiplus::Color(255, 0, 120, 215);
    g.FillPath(&Gdiplus::SolidBrush(colorFill), &path);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    g.DrawString(L"Giả lập Quét Thẻ",
                -1,
                &Gdiplus::Font(L"Segoe UI", 12, Gdiplus::FontStyleBold),
                m_rectTestBtn, 
                &format,
                &Gdiplus::SolidBrush(Gdiplus::Color::White));
}

void AuthIDCardDlg::DrawDeleteButton(Gdiplus::Graphics& g, int cx, int cy)
{
    float btnW = 200.0f;
    float btnH = 50.0f;
    m_rectDeleteBtn = Gdiplus::RectF(((float)cx - btnW / 2) + 700, (float)cy + 340, btnW, btnH);

    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, m_rectDeleteBtn, 10.0f);

    Gdiplus::Color colorFill = m_bDeleteBtnPressed ? Gdiplus::Color(255, 200, 60, 0) : Gdiplus::Color(255, 255, 80, 0);
    g.FillPath(&Gdiplus::SolidBrush(colorFill), &path);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    g.DrawString(L"Xóa sạch Database",
                -1,
                &Gdiplus::Font(L"Segoe UI", 12, Gdiplus::FontStyleBold),
                m_rectDeleteBtn,
                &format,
                &Gdiplus::SolidBrush(Gdiplus::Color::White));
}

//// add start worker thread for scanning process NTTai 20260114
//UINT __cdecl AuthIDCardDlg::ScanThreadProc(LPVOID pParam)
//{
//    AuthIDCardDlg* pDlg = (AuthIDCardDlg*)pParam;
//    if (pDlg == nullptr || !pDlg->GetSafeHwnd()) return 1;
//
//    CitizenCardData* pData = new CitizenCardData();
//    if (FakeCCCDReader::ScanCard_Simulation(*pData)) {
//        pDlg->PostMessage(WM_USER_SCAN_COMPLETE, (WPARAM)1, (LPARAM)pData);
//    }
//    else {
//        delete pData;
//        pDlg->PostMessage(WM_USER_SCAN_COMPLETE, (WPARAM)0, 0);
//    }
//    return 0;
//}
//// add end worker thread for scanning process NTTai 20260114

// add start implementation of CCCD validation logic NTTai 20260126
bool AuthIDCardDlg::ValidateCCCD(const CString& strCCCD)
{
    if (strCCCD.IsEmpty()) return false;

    if (strCCCD.GetLength() != 12) return false;

    for (int i = 0; i < strCCCD.GetLength(); i++) {
        if (!iswdigit(strCCCD[i])) 
        {
            return false;
        }
    }
    return true;
}
// add end implementation of CCCD validation logic NTTai 20260126

// add start device listener implementations NTTai 20260130
void AuthIDCardDlg::OnDestroy() {
    if (m_pDevice) {
        m_pDevice->Release();
        delete m_pDevice;
        m_pDevice = nullptr;
    }
	AuthStateManager::GetInstance()->UnregisterObserver(this); // add unregister auth state manager NTTai 20260210
    CDialogEx::OnDestroy();
}

void AuthIDCardDlg::OnScanSuccess(const CitizenCardData& data)
{
    CitizenCardData* pDataCopy = new CitizenCardData(data);
    PostMessage(WM_USER_SCAN_COMPLETE, (WPARAM)1, (LPARAM)pDataCopy);
}

void AuthIDCardDlg::OnScanError(CString strError)
{
    PostMessage(WM_USER_SCAN_COMPLETE, (WPARAM)0, 0);
}

void AuthIDCardDlg::OnDeviceConnected() {

}

void AuthIDCardDlg::OnDeviceDisconnected() {

}
// add end device listener implementations NTTai 20260130

void AuthIDCardDlg::OnAuthStateChanged(AuthState newState, CString strMessage)
{
    // Xử lý Timer giao diện dựa trên State mới
    KillTimer(3); // Progress bar
    KillTimer(4); // Error reset

    switch (newState)
    {
    case STATE_WAITING_SCAN:
        m_fProgressVal = 0.0f;
        break;
    case STATE_SCANNING:
        m_fProgressVal = 0.0f;
        m_bProgressIncreasing = true;
        SetTimer(3, 40, NULL);
        break;
    case STATE_VERIFYING:
        m_fProgressVal = 100.0f;
        break;
    case STATE_ERROR:
        SetTimer(4, 3000, NULL); // Auto reset sau 3s
        break;
    }
    Invalidate(FALSE);
}
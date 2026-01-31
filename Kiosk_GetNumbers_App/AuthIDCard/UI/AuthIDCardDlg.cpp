#include "pch.h"
#include "AuthIDCardDlg.h"

#define DEVICE_NAME L"modules//IDCard_Device.dll"

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
    m_eCurrentState = STATE_WAITING_CARD; // add init default state to waiting NTTai 20260114
    m_fProgressVal = 0.0f;
    m_bProgressIncreasing = true;
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

    StartScanProcess();
	// add end device adapter initialization NTTai 20260130

    return TRUE;
}

// add start set auth state helper NTTai 20260114
void AuthIDCardDlg::SetAuthState(AuthState state)
{
    m_eCurrentState = state;
    if (state == STATE_PROCESSING) {
        m_fProgressVal = 0.0f;
        m_bProgressIncreasing = true;
        SetTimer(3, 40, NULL);
    }
    else {
        KillTimer(3);
    }
    Invalidate(FALSE);
}
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

    DrawInstructions(g, cx, cy);
    DrawIDCardGraphic(g, cx, cy);
    if (m_eCurrentState == STATE_PROCESSING) {
        DrawProgressBar(g, cx, cy);
    }
    DrawStatusBox(g, cx, cy);
    if (m_eCurrentState == STATE_WAITING_CARD) {
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

// add start draw instructions based on state NTTai 20260114
void AuthIDCardDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);

    // Dark red brush for processing title
    Gdiplus::SolidBrush titleBrush(Gdiplus::Color(255, 162, 32, 45));
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 0, 0, 0));

    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);

    if (m_eCurrentState == STATE_WAITING_CARD) {
        g.DrawString(L"Vui lòng đặt căn cước công dân của bạn", 
                    -1, 
                    &fontTitle, 
                    Gdiplus::PointF((float)cx, (float)cy - 340), 
                    &format, 
                    &blackBrush);
    }
    else {
        g.DrawString(L"Đang xác thực thông tin...", 
                    -1, 
                    &fontTitle, 
                    Gdiplus::PointF((float)cx, (float)cy - 340), 
                    &format, 
                    &titleBrush);
    }

    Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 80, 80, 80));

    if (m_eCurrentState == STATE_WAITING_CARD) {
        g.DrawString(L"Hệ thống sẽ tự động quét và nhận diện thông tin từ thẻ của quý khách", 
            -1, 
            &fontSub, 
            Gdiplus::PointF((float)cx, (float)cy - 250), 
            &format, 
            &subBrush);
    }
    else {
        g.DrawString(L"Hệ thống đang kiểm tra tính hợp lệ của Căn cước công dân",
                    -1, 
                    &fontSub, 
                    Gdiplus::PointF((float)cx, (float)cy - 250), 
                    &format, 
                    &subBrush);
    }
}
// add end draw instructions based on state NTTai 20260114

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
void AuthIDCardDlg::DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::RectF boxRect((float)cx - 240, (float)cy + 220, 480.0f, 100.0f);
    Gdiplus::GraphicsPath path; CButtonUI::AddRoundedRectToPath(path, boxRect, 12.0f);

    if (m_eCurrentState == STATE_WAITING_CARD) {
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);
        g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 240, 240, 240), 1.0f), &path);
    }
    else {
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 250, 250)), &path);
        g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 250, 200, 200), 1.0f), &path);
    }

    // Change Left Icon
    Gdiplus::RectF iconRect(boxRect.X + 20.0f, boxRect.Y + 35.0f, 30.0f, 30.0f);
    if (m_eCurrentState == STATE_WAITING_CARD) {
        Gdiplus::SolidBrush agriRedBrush(Gdiplus::Color(255, 162, 32, 45));
        g.FillEllipse(&agriRedBrush, iconRect);
    }
    else {
        // Loading Icon
        Gdiplus::SolidBrush lightRedBrush(Gdiplus::Color(255, 255, 235, 235));
        g.FillEllipse(&lightRedBrush, iconRect);

        Gdiplus::Pen penRefresh(Gdiplus::Color(255, 162, 32, 45), 2.0f);
        g.DrawArc(&penRefresh,
            Gdiplus::REAL(iconRect.X + 8),
            Gdiplus::REAL(iconRect.Y + 8),
            Gdiplus::REAL(14),
            Gdiplus::REAL(14), 0, 270);
        g.DrawLine(&penRefresh, iconRect.X + 22, iconRect.Y + 8, iconRect.X + 22, iconRect.Y + 12);
    }

    // Change Content Text
    Gdiplus::RectF textTitleRect(boxRect.X + 65, boxRect.Y + 20, 395.0f, 30.0f);
    Gdiplus::RectF textDescRect(boxRect.X + 65, boxRect.Y + 50, 395.0f, 65.0f);
    Gdiplus::Font fontTitle(L"Segoe UI", 13, Gdiplus::FontStyleBold);
    Gdiplus::Font fontDesc(L"Segoe UI", 10, Gdiplus::FontStyleRegular);
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentNear); format.SetLineAlignment(Gdiplus::StringAlignmentNear);

    if (m_eCurrentState == STATE_WAITING_CARD) {
        g.DrawString(L"Đặt thẻ CCCD lên đầu đọc", -1, &fontTitle, textTitleRect, &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 30, 30, 30)));
        g.DrawString(L"Vui lòng giữ thẻ cố định cho đến khi đèn tín hiệu chuyển sang màu xanh.", -1, &fontDesc, textDescRect, &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 100, 100, 100)));
    }
    else {
        g.DrawString(L"Đang đọc thông tin chip",
            -1,
            &fontTitle,
            textTitleRect,
            &format,
            &Gdiplus::SolidBrush(Gdiplus::Color(255, 30, 30, 30)));
        g.DrawString(L"Vui lòng không rút thẻ ra khỏi đầu đọc cho đến khi quá trình hoàn tất",
            -1,
            &fontDesc,
            textDescRect,
            &format,
            &Gdiplus::SolidBrush(Gdiplus::Color(255, 100, 100, 100)));
    }
}
// add end draw status box NTTai 20260114

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

    if (nIDEvent == 3) {
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
    CDialogEx::OnTimer(nIDEvent);
}


// add start start scan process logic NTTai 20260114
void AuthIDCardDlg::StartScanProcess()
{
    SetAuthState(STATE_PROCESSING);
    //AfxBeginThread(ScanThreadProc, this);
	if (m_pDevice) m_pDevice->StartScanning(); // add start real scanning process NTTai 20260130
}
// add end start scan process logic NTTai 20260114

void AuthIDCardDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_eCurrentState == STATE_PROCESSING) return;

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
    if (m_eCurrentState == STATE_PROCESSING) return;

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
    SetAuthState(STATE_WAITING_CARD);

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

            if (bSaveOK) {
                AuthCorrect dlgCorrect(pData->strFullName, false, this);
                dlgCorrect.SetAuthData(m_scannedData);
                dlgCorrect.DoModal();
                EndDialog(IDOK);
            }
            else {
                AfxMessageBox(L"Lỗi: Không thể lưu thông tin vào hệ thống!");
            }
            delete pData;
        }
    }
    else {
        AfxMessageBox(L"Lỗi đọc thẻ! Vui lòng thử lại.");
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
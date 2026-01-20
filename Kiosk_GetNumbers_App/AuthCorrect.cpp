// File: AuthCorrect.cpp
#include "pch.h"
#include "AuthCorrect.h"
#include "NoAuthDlg.h"

IMPLEMENT_DYNAMIC(AuthCorrect, CDialogEx)

BEGIN_MESSAGE_MAP(AuthCorrect, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

AuthCorrect::AuthCorrect(CString strFullName, bool bIsNewCustomer, CWnd* pParent)
    : CDialogEx(IDD_AUTH_FINGER_DIALOG, pParent)
{
    m_strFullName = strFullName;
    m_bIsNewCustomer = bIsNewCustomer;
    m_nCountdown = 5;
}

AuthCorrect::~AuthCorrect() {}

BOOL AuthCorrect::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL AuthCorrect::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);

    // add start initialize timers for clock and auto-redirect NTTai 20260114
    SetTimer(1, 1000, NULL); // Clock & Countdown timer (1 second interval)
    // add end initialize timers for clock and auto-redirect NTTai 20260114

    return TRUE;
}

void AuthCorrect::OnPaint()
{
    CPaintDC dc(this);
    CRect rect; 
    GetClientRect(&rect);

    // add start setup GDI+ double buffering to prevent screen flickering NTTai 20260114
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
    int cy = rect.Height() / 2;

    DrawSuccessIcon(g, cx, cy);
    DrawMessage(g, cx, cy);
    DrawCustomerBox(g, cx, cy);
    DrawActionBtn(g, cx, cy);
    DrawFooter(g, cx, cy);

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
    // add end setup GDI+ double buffering to prevent screen flickering NTTai 20260114
}

// add start draw success verification icon (green checkmark) NTTai 20260114
void AuthCorrect::DrawSuccessIcon(Gdiplus::Graphics& g, int cx, int cy)
{
    float iconSize = 130.0f;
    float startY = cy - 300.0f;
    Gdiplus::RectF circleRect(cx - iconSize / 2, startY, iconSize, iconSize);

    Gdiplus::SolidBrush greenBrush(Gdiplus::Color(255, 0, 192, 88));

    Gdiplus::GraphicsPath shadowPath;
    shadowPath.AddEllipse(circleRect);
    Gdiplus::Matrix m;
    m.Translate(0, 10);
    shadowPath.Transform(&m);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(20, 0, 192, 88)), &shadowPath);

    g.FillEllipse(&greenBrush, circleRect);

    Gdiplus::Pen glowPen(Gdiplus::Color(100, 0, 192, 88), 10.0f);
    g.DrawEllipse(&glowPen, circleRect);

    Gdiplus::Pen checkPen(Gdiplus::Color::White, 8.0f);
    checkPen.SetStartCap(Gdiplus::LineCapRound);
    checkPen.SetEndCap(Gdiplus::LineCapRound);
    checkPen.SetLineJoin(Gdiplus::LineJoinRound);

    Gdiplus::PointF p1(cx - 25.0f, startY + 60.0f);
    Gdiplus::PointF p2(cx - 5.0f, startY + 80.0f);
    Gdiplus::PointF p3(cx + 35.0f, startY + 40.0f);

    g.DrawLine(&checkPen, p1, p2);
    g.DrawLine(&checkPen, p2, p3);
}
// add end draw success verification icon (green checkmark) NTTai 20260114

// add start draw success status messages and instructions NTTai 20260114
void AuthCorrect::DrawMessage(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format; 
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    CString strTitle;
    if (m_bIsNewCustomer) {
        strTitle = L"Đăng ký xác thực thành công";
    }
    else {
        strTitle = L"Xác thực thành công";
    }

    // Title (Green color)
    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    g.DrawString(strTitle, 
                -1, 
                &fontTitle,
                Gdiplus::PointF((float)cx, (float)cy - 150.0f), 
                &format,
                &Gdiplus::SolidBrush(Gdiplus::Color(255, 0, 192, 88)));

    Gdiplus::Font fontSub(L"Segoe UI", 12, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush brushGray(Gdiplus::Color(255, 108, 117, 125));

    CString strSub = L"Thông tin Căn cước công dân của quý khách đã được ghi nhận.\nHệ thống đang chuyển sang bước tiếp theo.";
    g.DrawString(strSub, 
                -1, 
                &fontSub,
                Gdiplus::PointF((float)cx, (float)cy - 60.0f), 
                &format, 
                &brushGray);
}
// add end draw success status messages and instructions NTTai 20260114

CString AuthCorrect::ConvertToTitleCase(CString strName)
{
    if (strName.IsEmpty()) return strName;
    ::CharLowerW(strName.GetBuffer());
    strName.ReleaseBuffer();

    wchar_t* buf = strName.GetBuffer();
    int len = strName.GetLength();
    bool bNextIsCap = true;

    for (int i = 0; i < len; i++) {
        if (iswspace(buf[i]) || iswpunct(buf[i])) {
            bNextIsCap = true;
        }
        else if (bNextIsCap) {
            ::CharUpperBuffW(&buf[i], 1);
            bNextIsCap = false;
        }
    }
    strName.ReleaseBuffer();

    return strName;
}

// add start draw authenticated customer information card NTTai 20260114
void AuthCorrect::DrawCustomerBox(Gdiplus::Graphics& g, int cx, int cy)
{
    float boxW = 650.0f;
    float boxH = 110.0f;
    float startY = cy + 40.0f;
    Gdiplus::RectF boxRect(cx - boxW / 2, startY, boxW, boxH);

    // Rounded background
    Gdiplus::GraphicsPath path; CButtonUI::AddRoundedRectToPath(path, boxRect, 12.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 230, 230, 230)), &path);

    // User Icon
    float iconSize = 50.0f;
    Gdiplus::RectF iconRect(boxRect.X + 20, boxRect.Y + 20, iconSize, iconSize);
    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 240, 242, 245)), iconRect);

    // Draw user shape inside icon
    Gdiplus::SolidBrush brushIcon(Gdiplus::Color(255, 180, 185, 190));
    g.FillEllipse(&brushIcon, iconRect.X + 15, iconRect.Y + 10, 20.0f, 20.0f); // Head
    g.FillPie(&brushIcon, iconRect.X + 5, iconRect.Y + 32, 40.0f, 40.0f, 180.0f, 180.0f); // Body

    // Customer Name Label
    Gdiplus::Font fontLabel(L"Segoe UI", 9, Gdiplus::FontStyleRegular);
    g.DrawString(L"KHÁCH HÀNG", -1, &fontLabel,
        Gdiplus::PointF(boxRect.X + 85, boxRect.Y + 22), NULL,
        &Gdiplus::SolidBrush(Gdiplus::Color(255, 120, 120, 120)));

    Gdiplus::Font fontName(L"Segoe UI", 18, Gdiplus::FontStyleBold);
    g.DrawString(ConvertToTitleCase(m_strFullName), 
                -1, 
                &fontName,
                Gdiplus::PointF(boxRect.X + 85, boxRect.Y + 42), 
                NULL,
                &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

    // "Verified" Badge
    Gdiplus::RectF badgeRect(boxRect.GetRight() - 140, boxRect.Y + 30, 130, 50);
    Gdiplus::GraphicsPath badgePath; CButtonUI::AddRoundedRectToPath(badgePath, badgeRect, 15.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 220, 252, 231)), &badgePath); 

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font fontBadge(L"Segoe UI", 9, Gdiplus::FontStyleBold);
    g.DrawString(L"✔ Đã xác thực", 
                -1, 
                &fontBadge, 
                badgeRect, 
                &format, 
                &Gdiplus::SolidBrush(Gdiplus::Color(255, 25, 135, 84)));
}
// add end draw authenticated customer information card NTTai 20260114

// add start draw continue navigation button NTTai 20260114
void AuthCorrect::DrawActionBtn(Gdiplus::Graphics& g, int cx, int cy)
{
    float btnW = 400.0f;
    float btnH = 55.0f;
    float startY = cy + 190.0f;
    m_rectContinueBtn = Gdiplus::RectF(cx - btnW / 2, startY, btnW, btnH);

    Gdiplus::GraphicsPath path; CButtonUI::AddRoundedRectToPath(path, m_rectContinueBtn, 27.5f);

    Gdiplus::Color btnColor = m_bContinuePressed ? Gdiplus::Color(200, 30, 30) : Gdiplus::Color(255, 162, 32, 45);
    g.FillPath(&Gdiplus::SolidBrush(btnColor), &path);

    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font fontBtn(L"Segoe UI", 13, Gdiplus::FontStyleBold);
    g.DrawString(L"Tiếp tục chọn loại giao dịch  ➜", 
                -1, 
                &fontBtn, 
                m_rectContinueBtn, 
                &format, 
                &Gdiplus::SolidBrush(Gdiplus::Color::White));
}
// add end draw continue navigation button NTTai 20260114

// add start draw auto-redirect countdown text NTTai 20260114
void AuthCorrect::DrawFooter(Gdiplus::Graphics& g, int cx, int cy)
{
    wchar_t szBuf[100];
    swprintf_s(szBuf, L"Tự động chuyển trang sau %d giây...", m_nCountdown);

    Gdiplus::Font font(L"Segoe UI", 11, Gdiplus::FontStyleRegular);
    Gdiplus::StringFormat format; 
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    float startY = cy + 270.0f;
    g.DrawString(szBuf, 
                -1, 
                &font, 
                Gdiplus::PointF((float)cx, startY), 
                &format, 
                &Gdiplus::SolidBrush(Gdiplus::Color(255, 108, 117, 125)));
}
// add end draw auto-redirect countdown text NTTai 20260114

void AuthCorrect::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        // Update Header Clock
        CRect rect; 
        GetClientRect(&rect);
        CRect rectTime(rect.Width() - 400, 140, rect.Width(), 210);
        InvalidateRect(&rectTime, FALSE);

        if (m_nCountdown > 0) {
            m_nCountdown--;
            int cx = rect.Width() / 2;
            int cy = rect.Height() / 2;
            CRect rectFooter(cx - 250, cy + 250, cx + 250, cy + 320);
            InvalidateRect(&rectFooter, FALSE);
        }
        else {
            // add start stop timer and transition to service selection NTTai 20260114
            KillTimer(1); // Stop timer to prevent recursive dialog calls
            NoAuthDlg dlgService(this);
            dlgService.SetAuthenticatedData(m_authData);
            dlgService.DoModal();
            EndDialog(IDOK);
            // add end stop timer and transition to service selection NTTai 20260114
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

void AuthCorrect::OnLButtonDown(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);
    if (m_rectContinueBtn.Contains(p)) {
        m_bContinuePressed = true;
        InvalidateRect( CRect((int)m_rectContinueBtn.X, 
                        (int)m_rectContinueBtn.Y, 
                        (int)m_rectContinueBtn.GetRight(), 
                        (int)m_rectContinueBtn.GetBottom()), 
                        FALSE );
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

void AuthCorrect::OnLButtonUp(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);
    if (m_rectContinueBtn.Contains(p)) {

        // add start handle manual continue button click NTTai 20260114
		KillTimer(1); //add ensure timer is stopped before manual transition NTTai 20260114
        NoAuthDlg dlgService(this);
        dlgService.SetAuthenticatedData(m_authData);
        dlgService.DoModal();

        EndDialog(IDOK);
        return;
        // add end handle manual continue button click NTTai 20260114
    }
    m_bContinuePressed = false;
    Invalidate(FALSE);
    CDialogEx::OnLButtonUp(nFlags, point);
}
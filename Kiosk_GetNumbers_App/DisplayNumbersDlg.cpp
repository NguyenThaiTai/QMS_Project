#include "pch.h"
#include "DisplayNumbersDlg.h"

IMPLEMENT_DYNAMIC(DisplayNumbersDlg, CDialogEx)

BEGIN_MESSAGE_MAP(DisplayNumbersDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// add start initialize ticket data NTTai 20260112
DisplayNumbersDlg::DisplayNumbersDlg(int serviceID, CString serviceTitle, CString ticketNumber, CWnd* pParent)
    : CDialogEx(IDD_DISPLAY_NUMBERS_DIALOG, pParent), m_serviceID(serviceID), m_serviceTitle(serviceTitle)
{
    m_strTicketNumber = ticketNumber;
}
// add end initialize ticket data NTTai 20260112

DisplayNumbersDlg::~DisplayNumbersDlg() {}

BOOL DisplayNumbersDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL DisplayNumbersDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);
    SetTimer(1, 1000, NULL);
    return TRUE;
}

void DisplayNumbersDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect; GetClientRect(&rect);

    CDC memDC; memDC.CreateCompatibleDC(&dc);
    CBitmap bmp; bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&bmp);
    Gdiplus::Graphics g(memDC.GetSafeHdc());
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

    g.Clear(Gdiplus::Color(255, 235, 235, 235));
    CHeaderUI::DrawSharedHeader(&memDC, rect);

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2;

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    // Main Title
    Gdiplus::Font fontTitle(L"Segoe UI", 33, Gdiplus::FontStyleBold);
    g.DrawString(L"Số thứ tự của bạn", -1, &fontTitle, Gdiplus::PointF((float)cx, (float)cy - 380), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

    // Sub Title
    Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);
    g.DrawString(L"Vui lòng lưu lại số thứ tự và theo dõi màn hình hiển thị", -1, &fontSub, Gdiplus::PointF((float)cx, (float)cy - 300), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 108, 117, 125)));

    DrawMainTicket(g, cx, cy);
    DrawFooterButtons(g, cx, cy);

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void DisplayNumbersDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        CRect rect; 
        GetClientRect(&rect);
        CRect rectTime(rect.Width() - 400, 140, rect.Width(), 210);
        InvalidateRect(&rectTime, FALSE);
    }
    CDialogEx::OnTimer(nIDEvent);
}

void DisplayNumbersDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);

    if (m_rectPrintBtn.Contains(p)) {
        m_bPrintPressed = true;
        InvalidateRect(CRect((int)m_rectPrintBtn.X, (int)m_rectPrintBtn.Y, (int)m_rectPrintBtn.GetRight(), (int)m_rectPrintBtn.GetBottom()), FALSE);
    }
    if (m_rectFinishBtn.Contains(p)) {
        m_bFinishPressed = true;
        InvalidateRect(CRect((int)m_rectFinishBtn.X, (int)m_rectFinishBtn.Y, (int)m_rectFinishBtn.GetRight(), (int)m_rectFinishBtn.GetBottom()), FALSE);
    }

    CDialogEx::OnLButtonDown(nFlags, point);
}

void DisplayNumbersDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);

    if (m_rectFinishBtn.Contains(p)) {
        EndDialog(IDOK);
    }

    if (m_rectPrintBtn.Contains(p)) {
        MessageBox(L"Đang in phiếu ...");
    }
    m_bPrintPressed = false;
    m_bFinishPressed = false;
    Invalidate(FALSE);
    CDialogEx::OnLButtonUp(nFlags, point);
}

void DisplayNumbersDlg::DrawMainTicket(Gdiplus::Graphics& g, int cx, int cy)
{
    float w = 550.0f;
    float h = 610.0f;
    Gdiplus::RectF cardRect((float)cx - w / 2, (float)cy - 250, w, h);

	// add start Soft Shadow for ticket card NTTai 20260114
    for (int i = 1; i <= 5; i++) {
        Gdiplus::RectF sRect = cardRect; sRect.Offset(0, (float)i);
        Gdiplus::GraphicsPath shadowPath;
        CButtonUI::AddRoundedRectToPath(shadowPath, sRect, 35.0f);
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(15, 0, 0, 0)), &shadowPath);
    }
	// add end Soft Shadow for ticket card NTTai 20260114
     
    // add start draw main ticket card visual NTTai 20260112
    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, cardRect, 35.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0)), &path);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color::White), &path);

    // Accent Bar
    Gdiplus::RectF accentRect(cardRect.X, cardRect.Y, cardRect.Width, 15.0f);
    Gdiplus::GraphicsPath accentPath;
    CButtonUI::AddRoundedRectToPath(accentPath, accentRect, 35.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)), &accentPath);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    // Label "SỐ THỨ TỰ"
    Gdiplus::Font fontLabel(L"Segoe UI", 12, Gdiplus::FontStyleBold);
    g.DrawString(L"SỐ THỨ TỰ", -1, &fontLabel, Gdiplus::PointF((float)cx, cardRect.Y + 40), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 150, 155, 160)));

    // Ticket Number
    Gdiplus::Font fontNum(L"Segoe UI", 85, Gdiplus::FontStyleBold);
    g.DrawString(m_strTicketNumber, -1, &fontNum, Gdiplus::PointF((float)cx, cardRect.Y + 50), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)));

    // Service Name
    Gdiplus::Font fontService(L"Segoe UI", 20, Gdiplus::FontStyleBold);
    g.DrawString(m_serviceTitle, -1, &fontService, Gdiplus::PointF((float)cx + 5, cardRect.Y + 240), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

    // Dashed Line
    Gdiplus::Pen dashPen(Gdiplus::Color(255, 162, 32, 45), 2.0f);
    dashPen.SetDashStyle(Gdiplus::DashStyleDash);
    g.DrawLine(&dashPen, cardRect.X + 30, cardRect.Y + 320, cardRect.GetRight() - 30, cardRect.Y + 320);

    // Circles
    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 255, 255)), (Gdiplus::REAL)(cardRect.X - 15), (Gdiplus::REAL)(cardRect.Y + 310), 30.0f, 30.0f);
    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 245, 247, 250)), (Gdiplus::REAL)(cardRect.GetRight() - 15), (Gdiplus::REAL)(cardRect.Y + 310), 30.0f, 30.0f);

    DrawInfoBoxes(g, cardRect);
    DrawInstructionBar(g, cardRect, cx);
    // add end draw main ticket card visual NTTai 20260112
}

void DisplayNumbersDlg::DrawInstructionBar(Gdiplus::Graphics& g, Gdiplus::RectF cardRect, int cx)
{
    float barW = cardRect.Width - 60.0f; 
    float barH = 100.0f;
    Gdiplus::RectF barRect(cardRect.X + 30, cardRect.GetBottom() - 120, barW, barH);
    Gdiplus::GraphicsPath path; 
    CButtonUI::AddRoundedRectToPath(path, barRect, 22.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 253, 246, 246)), &path);

    Gdiplus::REAL iconSize = 32.0f;
    Gdiplus::REAL iconY = barRect.Y + (barRect.Height - iconSize) / 2.0f;
    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)),
                (Gdiplus::REAL)(barRect.X + 22),
                iconY,
                iconSize,
                iconSize);

    g.DrawString(L"i", 
                -1, 
                &Gdiplus::Font(L"Times New Roman", 16, Gdiplus::FontStyleBold), 
                Gdiplus::PointF(barRect.X + 30, barRect.Y + 32), 
                NULL, 
                &Gdiplus::SolidBrush(Gdiplus::Color::White));

    CString strMsg;
    strMsg.Format(L"Vui lòng đến quầy giao dịch khi số %s của quý khách được gọi trên loa và màn hình hiển thị.", m_strTicketNumber);
    Gdiplus::RectF textRect(barRect.X + 65, barRect.Y + 5, barRect.Width - 85, barRect.Height - 10);
    Gdiplus::StringFormat textFormat;
    textFormat.SetAlignment(Gdiplus::StringAlignmentNear);
    textFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    g.DrawString(strMsg, -1, &Gdiplus::Font(L"Segoe UI", 11.5f), textRect, &textFormat, &Gdiplus::SolidBrush(Gdiplus::Color(255, 80, 85, 90)));
}

void DisplayNumbersDlg::DrawInfoBoxes(Gdiplus::Graphics& g, Gdiplus::RectF cardRect)
{

    float boxW = 230.0f; float boxH = 120.0f; float gap = 20.0f;
    float startX = cardRect.X + (cardRect.Width - (boxW * 2 + gap)) / 2;
    float boxY = cardRect.Y + 345;
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::RectF leftBox(startX, boxY, boxW, boxH);
    Gdiplus::GraphicsPath p1; CButtonUI::AddRoundedRectToPath(p1, leftBox, 20.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 240, 240)), &p1);
    g.DrawString(L"3", -1, &Gdiplus::Font(L"Segoe UI", 22, Gdiplus::FontStyleBold), Gdiplus::PointF(leftBox.X + boxW / 2, boxY + 60), &format, &Gdiplus::SolidBrush(Gdiplus::Color::Black));
    g.DrawString(L"Người chờ trước bạn", -1, &Gdiplus::Font(L"Segoe UI", 10), Gdiplus::PointF(leftBox.X + boxW / 2, boxY + 30), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 100, 100, 100)));

    Gdiplus::RectF rightBox(startX + boxW + gap, boxY, boxW, boxH);
    Gdiplus::GraphicsPath p2; CButtonUI::AddRoundedRectToPath(p2, rightBox, 20.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 235, 245, 255)), &p2);
    g.DrawString(L"5 phút", -1, &Gdiplus::Font(L"Segoe UI", 22, Gdiplus::FontStyleBold), Gdiplus::PointF(rightBox.X + boxW / 2, boxY + 60), &format, &Gdiplus::SolidBrush(Gdiplus::Color::Black));
    g.DrawString(L"Thời gian chờ dự kiến", -1, &Gdiplus::Font(L"Segoe UI", 10), Gdiplus::PointF(rightBox.X + boxW / 2, boxY + 30), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 100, 100, 100)));
}

void DisplayNumbersDlg::DrawFooterButtons(Gdiplus::Graphics& g, int cx, int cy)
{
    float btnW = 360.0f; float btnH = 80.0f; float gap = 30.0f;
    float startX = (float)cx - (btnW + gap / 2 + 30);
    float btnY = (float)cy + 430.0f;
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter); format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    // add start draw print button NTTai 20260112
    m_rectPrintBtn = Gdiplus::RectF(startX, btnY, btnW, btnH);
    Gdiplus::GraphicsPath p1; CButtonUI::AddRoundedRectToPath(p1, m_rectPrintBtn, 18.0f);
    Gdiplus::Color colorPrint = m_bPrintPressed ? Gdiplus::Color(255, 230, 235, 240) : Gdiplus::Color::White;
    g.FillPath(&Gdiplus::SolidBrush(colorPrint), &p1);
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 210, 215, 220), 1.2f), &p1);
    g.DrawString(L"      In phiếu số", -1, &Gdiplus::Font(L"Segoe UI", 17, Gdiplus::FontStyleBold), m_rectPrintBtn, &format, &Gdiplus::SolidBrush(Gdiplus::Color::Black));
    // add end draw print button NTTai 20260112

    // Icon Printer
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::Black), (Gdiplus::REAL)(startX + 90), (Gdiplus::REAL)(btnY + 32), 28.0f, 18.0f);
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::White), (Gdiplus::REAL)(startX + 96), (Gdiplus::REAL)(btnY + 35), 16.0f, 5.0f);

    // add start draw finish button NTTai 20260112
    m_rectFinishBtn = Gdiplus::RectF(startX + btnW + gap, btnY, btnW + 60, btnH);
    Gdiplus::GraphicsPath p2; CButtonUI::AddRoundedRectToPath(p2, m_rectFinishBtn, 18.0f);
    Gdiplus::Color colorFinish = m_bFinishPressed ? Gdiplus::Color(255, 130, 20, 30) : Gdiplus::Color(255, 162, 32, 45);
    g.FillPath(&Gdiplus::SolidBrush(colorFinish), &p2);
    g.DrawString(L"Hoàn tất & Về trang chủ", -1, &Gdiplus::Font(L"Segoe UI", 17, Gdiplus::FontStyleBold), m_rectFinishBtn, &format, &Gdiplus::SolidBrush(Gdiplus::Color::White));
    // add end draw finish button NTTai 20260112
}
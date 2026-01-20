// AuthFaceIDDlg.cpp
#include "pch.h"
#include "AuthFaceIDDlg.h"
#include "HeaderUI.h"

IMPLEMENT_DYNAMIC(AuthFaceIDDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthFaceIDDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
END_MESSAGE_MAP()

// add corrected constructor NTTai 20260115
AuthFaceIDDlg::AuthFaceIDDlg(CWnd* pParent)
    : CDialogEx(IDD_AUTH_FACEID_DIALOG, pParent)
{
    // add start initialize animation variables NTTai 20260115
    m_fScanPos = 0.0f;
    m_bScanDown = true;
    // add end initialize animation variables NTTai 20260115
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
    return TRUE;
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
    DrawInstructions(g, cx, cy);
    DrawFaceScannerGraphic(g, cx, cy);
    DrawStatusBox(g, cx, cy); // Draw the bottom instruction note NTTai 20260115
    // Draw shared cancel button NTTai 20260115
    CButtonUI::DrawCancelButton(g, cx, cy, m_rectCancelBtn);
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
    CDialogEx::OnTimer(nIDEvent);
}

void AuthFaceIDDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_rectCancelBtn.Contains(Gdiplus::PointF((float)point.x, (float)point.y))) {
        EndDialog(IDCANCEL); // add close dialog when cancel button is clicked NTTai 20260115
    }

    CDialogEx::OnLButtonUp(nFlags, point);
}

// MODULE 1: Draw instruction texts for FaceID NTTai 20260115
void AuthFaceIDDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackTitleBrush(Gdiplus::Color(255, 0, 0, 0));
    g.DrawString(L"Vui lòng nhìn thẳng vào camera", -1, &fontTitle,
        Gdiplus::PointF((float)cx, (float)cy - 350), &format, &blackTitleBrush);

    Gdiplus::Font fontSub(L"Segoe UI", 14, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush subBrush(Gdiplus::Color(255, 80, 80, 80));
    g.DrawString(L"Giữ khuôn mặt của bạn nằm trong khung tròn để xác thực", -1, &fontSub,
        Gdiplus::PointF((float)cx, (float)cy - 270), &format, &subBrush);
}

// MODULE 2: Draw circular face scanner with animated beam NTTai 20260115
void AuthFaceIDDlg::DrawFaceScannerGraphic(Gdiplus::Graphics& g, int cx, int cy)
{
    float scannerRadius = 160.0f;
    float scannerSize = scannerRadius * 2.0f;

    // add shift the entire scanner assembly down by 50px for better balance NTTai 20260115
    Gdiplus::RectF scanRect((float)cx - scannerRadius, (float)cy - 150.0f, scannerSize, scannerSize);

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

    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(20, 162, 32, 45)), scanRect);

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
    float beamHalfHeight = 35.0f;
    float currentY = scanRect.Y + m_fScanPos;
    Gdiplus::RectF topHalf(scanRect.X, currentY - beamHalfHeight, scanRect.Width, beamHalfHeight);
    Gdiplus::LinearGradientBrush topBrush(topHalf, Gdiplus::Color(0, 162, 32, 45), Gdiplus::Color(200, 162, 32, 45), Gdiplus::LinearGradientModeVertical);
    g.FillRectangle(&topBrush, topHalf);
    Gdiplus::RectF botHalf(scanRect.X, currentY, scanRect.Width, beamHalfHeight);
    Gdiplus::LinearGradientBrush botBrush(botHalf, Gdiplus::Color(200, 162, 32, 45), Gdiplus::Color(0, 162, 32, 45), Gdiplus::LinearGradientModeVertical);
    g.FillRectangle(&botBrush, botHalf);
    g.DrawLine(&Gdiplus::Pen(Gdiplus::Color(220, 162, 32, 45), 2.0f), scanRect.X, currentY, scanRect.X + scanRect.Width, currentY);
    // add end draw scanning beam NTTai 20260115

    g.ResetClip();

    g.DrawEllipse(&Gdiplus::Pen(Gdiplus::Color(255, 162, 32, 45), 5.0f), scanRect);
}

// MODULE 3: Draw bottom status note for FaceID NTTai 20260115
void AuthFaceIDDlg::DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy)
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

    // add start draw small FaceID red icon NTTai 20260115
    float iconSize = 20.0f;
    float iconX = boxRect.X + 25.0f;
    float iconY = boxRect.Y + (boxHeight - iconSize) / 2.0f;
    Gdiplus::Pen iconPen(Gdiplus::Color(255, 162, 32, 45), 2.0f);
    g.DrawRectangle(&iconPen, (Gdiplus::REAL)iconX, (Gdiplus::REAL)iconY,
        (Gdiplus::REAL)iconSize, (Gdiplus::REAL)iconSize);
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
    g.DrawString(L"Giữ khuôn mặt trong khung hình để hệ thống nhận diện", -1,
        &fontDesc, textRect, &format, &blackBrush);
    // add end draw instruction text NTTai 20260115
}
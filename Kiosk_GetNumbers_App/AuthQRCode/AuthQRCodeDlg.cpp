// File: AuthQRCodeDlg.cpp
#include "pch.h"
#include "AuthQRCodeDlg.h"
#include <afxinet.h>

IMPLEMENT_DYNAMIC(AuthQRCodeDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthQRCodeDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
    // add start register message handler for QR result NTTai 20260123
    ON_MESSAGE(WM_API_QR_RESULT, &AuthQRCodeDlg::OnApiQrResult)
    // add end register message handler for QR result NTTai 20260123
END_MESSAGE_MAP()

AuthQRCodeDlg::AuthQRCodeDlg(CWnd* pParent)
    : CDialogEx(IDD_AUTH_QRCODE_DIALOG, pParent)
{
    m_nCountdown = 29;
    m_bIsLoading = false; // add init loading state NTTai 20260123
    m_strQRData = L"";
}

AuthQRCodeDlg::~AuthQRCodeDlg() {}

BOOL AuthQRCodeDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL AuthQRCodeDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);

    // add start trigger async QR fetch NTTai 20260123
    RefreshQRCode();
    SetTimer(1, 1000, NULL);
    // add end trigger async QR fetch NTTai 20260123

    return TRUE;
}

void AuthQRCodeDlg::RefreshQRCode()
{
    if (m_bIsLoading) return;
    m_bIsLoading = true;
    m_strQRData = L"";
    Invalidate(); 
    AfxBeginThread(QrFetcherThreadProc, this->GetSafeHwnd());
}

// add start worker thread to fetch QR URL NTTai 20260123
UINT __cdecl AuthQRCodeDlg::QrFetcherThreadProc(LPVOID pParam)
{
    HWND hWnd = (HWND)pParam;
    if (!hWnd) return 1;

    QrThreadResult* pResult = new QrThreadResult();
    pResult->success = false;

    CString url = ApiService::GetInstance()->GetNewQRCodeUrl();

    if (!url.IsEmpty()) {
        pResult->success = true;
        pResult->url = url;
    }

    ::PostMessage(hWnd, WM_API_QR_RESULT, 0, (LPARAM)pResult);
    return 0;
}
// add end worker thread to fetch QR URL NTTai 20260123

// add start handler for QR result message NTTai 20260123
LRESULT AuthQRCodeDlg::OnApiQrResult(WPARAM wParam, LPARAM lParam)
{
    m_bIsLoading = false;
    QrThreadResult* pResult = (QrThreadResult*)lParam;

    if (pResult && pResult->success) {
        m_strQRData = pResult->url;
        m_nCountdown = 29;
    }
    else {
        TRACE(L"Failed to fetch QR Code URL\n");
    }

    if (pResult) delete pResult;
    Invalidate();
    return 0;
}
// add end handler for QR result message NTTai 20260123

void AuthQRCodeDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect; GetClientRect(&rect);

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
    int cy = rect.Height() / 2 + 40;

    DrawInstructions(g, cx, cy);
    DrawQRCodeGraphic(g, cx, cy);
    DrawStatusBox(g, cx, cy);
    CButtonUI::DrawCancelButton(g, cx, cy + 90, m_rectCancelBtn); // Adjusted Y to match old UI

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void AuthQRCodeDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 30, 30, 30));
    g.DrawString(L"Vui lòng quét mã QR để xác thực", -1, &fontTitle, Gdiplus::PointF((float)cx, (float)cy - 350), &format, &blackBrush);
    Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush grayBrush(Gdiplus::Color(255, 80, 80, 80));
    g.DrawString(L"Sau khi quét mã, quý khách vui lòng chọn loại giao dịch trực tiếp trên điện thoại của mình.", -1, &fontSub, Gdiplus::PointF((float)cx, (float)cy - 270), &format, &grayBrush);
}

// add start Draw QR Code graphic (KEEPING ORIGINAL UI LOGIC) NTTai 20260123
void AuthQRCodeDlg::DrawQRCodeGraphic(Gdiplus::Graphics& g, int cx, int cy)
{
    float qrAreaSize = 320.0f;
    Gdiplus::RectF qrAreaRect((float)cx - qrAreaSize / 2.0f, (float)cy - 180.0f, qrAreaSize, qrAreaSize);
    Gdiplus::GraphicsPath areaPath; 
    CButtonUI::AddRoundedRectToPath(areaPath, qrAreaRect, 20.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 218, 185)), &areaPath);

    // Draw 4 red L-shaped corners
    Gdiplus::Pen framePen(Gdiplus::Color(255, 162, 32, 45), 6.0f);
    framePen.SetStartCap(Gdiplus::LineCapRound); 
    framePen.SetEndCap(Gdiplus::LineCapRound);
    float len = 45.0f; float gap = 20.0f;
    Gdiplus::RectF outer = qrAreaRect; outer.Inflate(gap, gap);
    g.DrawLine(&framePen, outer.X, outer.Y, outer.X + len, outer.Y);
    g.DrawLine(&framePen, outer.X, outer.Y, outer.X, outer.Y + len);
    g.DrawLine(&framePen, outer.GetRight(), outer.Y, outer.GetRight() - len, outer.Y);
    g.DrawLine(&framePen, outer.GetRight(), outer.Y, outer.GetRight(), outer.Y + len);
    g.DrawLine(&framePen, outer.X, outer.GetBottom(), outer.X + len, outer.GetBottom());
    g.DrawLine(&framePen, outer.X, outer.GetBottom(), outer.X, outer.GetBottom() - len);
    g.DrawLine(&framePen, outer.GetRight(), outer.GetBottom(), outer.GetRight() - len, outer.GetBottom());
    g.DrawLine(&framePen, outer.GetRight(), outer.GetBottom(), outer.GetRight(), outer.GetBottom() - len);

    // Check loading state  
    if (m_bIsLoading) {
        Gdiplus::Font fontLoad(L"Segoe UI", 12);
        Gdiplus::SolidBrush brushLoad(Gdiplus::Color(255, 100, 100, 100));
        Gdiplus::StringFormat fmt; fmt.SetAlignment(Gdiplus::StringAlignmentCenter); fmt.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(L"Đang tạo mã...", -1, &fontLoad, qrAreaRect, &fmt, &brushLoad);
        return;
    }

    if (m_strQRData.IsEmpty()) {
        return;
    }

    // Draw White Card
    float cardSize = 260.0f; 
    Gdiplus::RectF cardRect((float)cx - cardSize / 2.0f, qrAreaRect.Y + (qrAreaSize - cardSize) / 2.0f, cardSize, cardSize);
    Gdiplus::RectF shadow = cardRect; 
    shadow.Offset(5, 5);
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(40, 0, 0, 0)), shadow);
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::White), cardRect);

    try {
        CStringA strUrlUTF8 = CW2A(m_strQRData, CP_UTF8); // Convert to UTF8 for qrcodegen
        QrCode qr = QrCode::encodeText((LPCSTR)strUrlUTF8, QrCode::Ecc::HIGH); // Use HIGH ECC for center logo

        float qrSize = 210.0f;
        float startX = (float)cx - qrSize / 2.0f;
        float startY = cardRect.Y + (cardSize - qrSize) / 2.0f;

        int modules = qr.getSize();
        float mSize = qrSize / modules;

        Gdiplus::SolidBrush qrBrush(Gdiplus::Color(255, 162, 32, 45)); // Agribank Red
        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color::White);

        for (int row = 0; row < modules; row++) {
            for (int col = 0; col < modules; col++) {
                // Skip center for logo
                if (row > modules / 2 - 3 && row < modules / 2 + 3 && col > modules / 2 - 3 && col < modules / 2 + 3) {
                    continue;
                }
                if (qr.getModule(col, row)) {
                    float posX = startX + col * mSize;
                    float posY = startY + row * mSize;

                    // Check position patterns (Large corners)
                    bool isPositionPattern = (row < 7 && col < 7) || (row < 7 && col >= modules - 7) || (row >= modules - 7 && col < 7);

                    if (isPositionPattern) {
                        g.FillRectangle(&qrBrush, (Gdiplus::REAL)posX, (Gdiplus::REAL)posY, (Gdiplus::REAL)mSize, (Gdiplus::REAL)mSize);
                    }
                    else {
                        // Draw round dots
                        float dotSize = mSize * 0.85f;
                        float offset = (mSize - dotSize) / 2.0f;
                        g.FillEllipse(&qrBrush, (Gdiplus::REAL)(posX + offset), (Gdiplus::REAL)(posY + offset), (Gdiplus::REAL)dotSize, (Gdiplus::REAL)dotSize);
                    }
                }
            }
        }

        // Draw Center Logo Placeholder
        float logoSpaceSize = mSize * 6.0f;
        Gdiplus::RectF logoRect((float)cx - logoSpaceSize / 2.0f, startY + (qrSize - logoSpaceSize) / 2.0f, logoSpaceSize, logoSpaceSize);
        g.FillEllipse(&whiteBrush, logoRect);
        g.FillEllipse(&qrBrush, (float)cx - 5, logoRect.Y + (logoSpaceSize / 2.0f) - 5, 10.0f, 10.0f);
    }
    catch (...) {}
}
// add end Draw QR Code graphic (KEEPING ORIGINAL UI LOGIC) NTTai 20260123

void AuthQRCodeDlg::DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);
    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(30, 162, 32, 45)), (Gdiplus::REAL)(cx - 15), (Gdiplus::REAL)(cy + 180), 30.0f, 30.0f);
    Gdiplus::Font fontBold(L"Segoe UI", 13, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 30, 30, 30));
    g.DrawString(L"Sử dụng camera để quét mã và thực hiện giao dịch", -1, &fontBold, Gdiplus::PointF((float)cx, (float)cy + 225), &format, &blackBrush);

    wchar_t szBuf[64];
    swprintf_s(szBuf, L"Mã QR sẽ tự động làm mới sau %ds", m_nCountdown);
    Gdiplus::Font fontReg(L"Segoe UI", 11, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush redBrush(Gdiplus::Color(255, 162, 32, 45));
    g.DrawString(szBuf, -1, &fontReg, Gdiplus::PointF((float)cx, (float)cy + 255), &format, &redBrush);
}

void AuthQRCodeDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        if (m_nCountdown > 0) {
            m_nCountdown--;
            CRect rect; 
            GetClientRect(&rect);
            CRect rectTime(rect.Width() - 400, 140, rect.Width(), 210);
            InvalidateRect(&rectTime, FALSE);
            int cx = rect.Width() / 2; 
            int cy = rect.Height() / 2 + 40;
            CRect rectCountdown(cx - 250, cy + 200, cx + 250, cy + 300);
            InvalidateRect(&rectCountdown, FALSE);
        }
        else {
            RefreshQRCode();
        }
    }
    CDialogEx::OnTimer(nIDEvent);
}

void AuthQRCodeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_rectCancelBtn.Contains(Gdiplus::PointF((float)point.x, (float)point.y))) {
        EndDialog(IDCANCEL);
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}
// AuthQRCodeDlg.cpp
#include "pch.h"
#include "AuthQRCodeDlg.h"
#include "HeaderUI.h"
#include "qrcodegen.hpp"
#include <afxinet.h>
using qrcodegen::QrCode;

IMPLEMENT_DYNAMIC(AuthQRCodeDlg, CDialogEx)

BEGIN_MESSAGE_MAP(AuthQRCodeDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_TIMER()
END_MESSAGE_MAP()

AuthQRCodeDlg::AuthQRCodeDlg(CWnd* pParent)
    : CDialogEx(IDD_AUTH_QRCODE_DIALOG, pParent)
{
    m_nCountdown = 59;
}

AuthQRCodeDlg::~AuthQRCodeDlg() {}

// add disable background erasing to prevent flickering NTTai 20260107
BOOL AuthQRCodeDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL AuthQRCodeDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // add start initialize QR code and timers NTTai 20260108
    RefreshQRCode();
    CHeaderUI::SetFullScreen(this);
    SetTimer(1, 1000, NULL); // add clock and countdown timer NTTai 20260108
    // add end initialize QR code and timers NTTai 20260108
    return TRUE;
}

void AuthQRCodeDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect; GetClientRect(&rect);

    // add start initialize double buffering NTTai 20260108
    CDC memDC; 
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp; 
    bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOldBmp = memDC.SelectObject(&bmp);
    Gdiplus::Graphics g(memDC.GetSafeHdc());
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    // add end initialize double buffering NTTai 20260108

    // add draw background and shared header NTTai 20260108
    g.Clear(Gdiplus::Color(255, 235, 235, 235));
    CHeaderUI::DrawSharedHeader(&memDC, rect);

    // add start execute QR UI drawing modules NTTai 20260108
    int cx = rect.Width() / 2; 
    int cy = rect.Height() / 2 + 40;
    DrawInstructions(g, cx, cy);
    DrawQRCodeGraphic(g, cx, cy);
    DrawStatusBox(g, cx, cy);
    CButtonUI::DrawCancelButton(g, cx, cy, m_rectCancelBtn);
    // add end execute QR UI drawing modules NTTai 20260108

    // add copy buffer to screen and clean up NTTai 20260108
    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void AuthQRCodeDlg::OnTimer(UINT_PTR nIDEvent)
{
    // add start handle 1-second updates NTTai 20260108
    if (nIDEvent == 1)
    {
        // add update QR countdown NTTai 20260108
        if (m_nCountdown > 0) m_nCountdown--;
        else RefreshQRCode();

        // add start invalidate UI areas for update NTTai 20260108
        CRect rect; 
        GetClientRect(&rect);
        CRect rectTime(rect.Width() - 400, 140, rect.Width(), 210);
        InvalidateRect(&rectTime, FALSE);
        int cx = rect.Width() / 2; 
        int cy = rect.Height() / 2 + 40;
        CRect rectCountdown(cx - 250, cy + 200, cx + 250, cy + 300);
        InvalidateRect(&rectCountdown, FALSE);
        // add end invalidate UI areas NTTai 20260108
    }
    // add end handle 1-second updates NTTai 20260108
    CDialogEx::OnTimer(nIDEvent);
}

void AuthQRCodeDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_rectCancelBtn.Contains(Gdiplus::PointF((float)point.x, (float)point.y))) {
        EndDialog(IDCANCEL);// add close dialog when cancel button is clicked NTTai 20260107
    }
    CDialogEx::OnLButtonUp(nFlags, point);
}

// add start Draw QR authentication instructions NTTai 20260108
void AuthQRCodeDlg::DrawInstructions(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format; 
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 30, 30, 30));
    g.DrawString(L"Vui lòng quét mã QR để xác thực", -1, &fontTitle, Gdiplus::PointF((float)cx, (float)cy - 350), &format, &blackBrush);
    Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush grayBrush(Gdiplus::Color(255, 80, 80, 80));
    g.DrawString(L"Sử dụng ứng dụng Agribank E-Mobile Banking trên điện thoại của bạn để quét mã bên dưới.", -1, &fontSub, Gdiplus::PointF((float)cx, (float)cy - 270), &format, &grayBrush);
}
// add end Draw QR instructions NTTai 20260108

// add start Draw QR Code graphic NTTai 20260108
void AuthQRCodeDlg::DrawQRCodeGraphic(Gdiplus::Graphics& g, int cx, int cy)
{
    float qrAreaSize = 320.0f; 
    Gdiplus::RectF qrAreaRect((float)cx - qrAreaSize / 2.0f, (float)cy - 180.0f, qrAreaSize, qrAreaSize);
    Gdiplus::GraphicsPath areaPath; CButtonUI::AddRoundedRectToPath(areaPath, qrAreaRect, 20.0f);
    g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 255, 218, 185)), &areaPath);

    // add start draw 4 red L-shaped corners NTTai 20260108
    Gdiplus::Pen framePen(Gdiplus::Color(255, 162, 32, 45), 6.0f);
    framePen.SetStartCap(Gdiplus::LineCapRound); 
    framePen.SetEndCap(Gdiplus::LineCapRound);
    float len = 45.0f; 
    float gap = 20.0f; 
    Gdiplus::RectF outer = qrAreaRect; 
    outer.Inflate(gap, gap);
    g.DrawLine(&framePen, outer.X, outer.Y, outer.X + len, outer.Y); 
    g.DrawLine(&framePen, outer.X, outer.Y, outer.X, outer.Y + len);
    g.DrawLine(&framePen, outer.GetRight(), outer.Y, outer.GetRight() - len, outer.Y); 
    g.DrawLine(&framePen, outer.GetRight(), outer.Y, outer.GetRight(), outer.Y + len);
    g.DrawLine(&framePen, outer.X, outer.GetBottom(), outer.X + len, outer.GetBottom()); 
    g.DrawLine(&framePen, outer.X, outer.GetBottom(), outer.X, outer.GetBottom() - len);
    g.DrawLine(&framePen, outer.GetRight(), outer.GetBottom(), outer.GetRight() - len, outer.GetBottom()); 
    g.DrawLine(&framePen, outer.GetRight(), outer.GetBottom(), outer.GetRight(), outer.GetBottom() - len);
    // add end draw 4 red L-shaped corners NTTai 20260108

    // add start draw white card and QR content NTTai 20260108
    float cardSize = 260.0f; Gdiplus::RectF cardRect((float)cx - cardSize / 2.0f, qrAreaRect.Y + (qrAreaSize - cardSize) / 2.0f, cardSize, cardSize);
    Gdiplus::RectF shadow = cardRect; shadow.Offset(5, 5); 
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color(40, 0, 0, 0)), shadow);
    g.FillRectangle(&Gdiplus::SolidBrush(Gdiplus::Color::White), cardRect);
    // add end draw white card and QR content NTTai 20260108
 
    // add start draw real QR code from data NTTai 20260108
    CT2A asciiData(m_strQRData);
    QrCode qr = QrCode::encodeText(asciiData, QrCode::Ecc::HIGH);// add use HIGH ECC to allow center logo NTTai 20260108

    float qrSize = 210.0f;
    float startX = (float)cx - qrSize / 2.0f;
    float startY = cardRect.Y + (cardSize - qrSize) / 2.0f;

    int modules = qr.getSize();
    float mSize = qrSize / modules;

    // add start initialize Agribank theme brushes NTTai 20260108
    Gdiplus::SolidBrush qrBrush(Gdiplus::Color(255, 162, 32, 45)); // Agribank Red
    Gdiplus::SolidBrush whiteBrush(Gdiplus::Color::White);
    // add end initialize brushes NTTai 20260108

    for (int row = 0; row < modules; row++) {
        for (int col = 0; col < modules; col++) {
            if (row > modules / 2 - 3 && row < modules / 2 + 3 && col > modules / 2 - 3 && col < modules / 2 + 3) {
                continue; // add start skip center area (approx 6x6 modules) for brand logo NTTai 20260108
            }
            if (qr.getModule(col, row)) {
                float posX = startX + col * mSize;
                float posY = startY + row * mSize;

                // add start check if this is part of the 3 large corner patterns NTTai 20260108
                bool isPositionPattern = (row < 7 && col < 7) || (row < 7 && col >= modules - 7) || (row >= modules - 7 && col < 7);

                if (isPositionPattern) {                   
                    g.FillRectangle(&qrBrush, (Gdiplus::REAL)posX, (Gdiplus::REAL)posY, (Gdiplus::REAL)mSize, (Gdiplus::REAL)mSize);  // add draw corner squares as standard modules NTTai 20260108
                }
                else {
                    // add start draw data modules as modern rounded dots NTTai 20260108
                    float dotSize = mSize * 0.85f;
                    float offset = (mSize - dotSize) / 2.0f;
                    g.FillEllipse(&qrBrush, (Gdiplus::REAL)(posX + offset), (Gdiplus::REAL)(posY + offset), (Gdiplus::REAL)dotSize, (Gdiplus::REAL)dotSize);
                    // add end draw data modules as modern rounded dots NTTai 20260108
                }
                // add end check if this is part of the 3 large corner patterns NTTai 20260108
            }
        }
    }

    // add start draw a small white circle in the center for Brand Identity NTTai 20260108
    float logoSpaceSize = mSize * 6.0f;
    Gdiplus::RectF logoRect((float)cx - logoSpaceSize / 2.0f, startY + (qrSize - logoSpaceSize) / 2.0f, logoSpaceSize, logoSpaceSize);
    g.FillEllipse(&whiteBrush, logoRect);
    g.FillEllipse(&qrBrush, (float)cx - 5, logoRect.Y + (logoSpaceSize / 2.0f) - 5, 10.0f, 10.0f); // add draw a tiny red dot representing Agribank placeholder NTTai 20260108
    // add end draw a small white circle in the center for Brand Identity NTTai 20260108
    
    // add end draw real QR code from data NTTai 20260108
    
}
// add end Draw QR Code graphic NTTai 20260108

// add start Draw status box and countdown NTTai 20260108
void AuthQRCodeDlg::DrawStatusBox(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);
    g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(30, 162, 32, 45)), cx - 15, cy + 180, 30, 30); // add small icon NTTai 20260108
    Gdiplus::Font fontBold(L"Segoe UI", 13, Gdiplus::FontStyleBold);
    Gdiplus::SolidBrush blackBrush(Gdiplus::Color(255, 30, 30, 30));
    g.DrawString(L"Mở ứng dụng ngân hàng và quét mã QR này để xác thực", -1, &fontBold, Gdiplus::PointF((float)cx, (float)cy + 225), &format, &blackBrush);
    wchar_t szBuf[64]; swprintf_s(szBuf, L"Mã QR sẽ tự động làm mới sau %ds", m_nCountdown);
    Gdiplus::Font fontReg(L"Segoe UI", 11, Gdiplus::FontStyleRegular);
    Gdiplus::SolidBrush redBrush(Gdiplus::Color(255, 162, 32, 45));
    g.DrawString(szBuf, -1, &fontReg, Gdiplus::PointF((float)cx, (float)cy + 255), &format, &redBrush);
}
// add end Draw status box NTTai 20260108

// add start refresh logic using API NTTai 20260115
void AuthQRCodeDlg::RefreshQRCode()
{
    BeginWaitCursor();
    CString strUrl = GetQRCodeUrlFromServer(); // Fetch new URL
    EndWaitCursor();

    if (!strUrl.IsEmpty()) {
        m_strQRData = strUrl;
        m_nCountdown = 29;
        Invalidate(FALSE);
    }
}
// add end refresh logic using API NTTai 20260115

// add start fetch URL from API implementation NTTai 20260115
CString AuthQRCodeDlg::GetQRCodeUrlFromServer()
{
    CString strUrl = L"";
    CInternetSession session(_T("AgribankKioskSession"), 1, INTERNET_OPEN_TYPE_DIRECT);
    CHttpFile* pFile = nullptr;
    try {
        // GET Request to generate new QR
        // URL: http://10.10.1.21:8081/api/ticket/generate-new-qr
        pFile = (CHttpFile*)session.OpenURL(_T("http://10.10.1.21:8081/api/ticket/generate-new-qr"),
                                            1,
                                            INTERNET_FLAG_TRANSFER_BINARY |
                                            INTERNET_FLAG_RELOAD |
                                            INTERNET_FLAG_DONT_CACHE |
                                            INTERNET_FLAG_PRAGMA_NOCACHE);
        if (pFile) {
            CStringA strRawResponse;
            char buffer[4096];
            UINT nRead = 0;
            while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) {
                buffer[nRead] = '\0';
                strRawResponse += buffer;
            }
            pFile->Close();
            delete pFile;

            // Convert to Unicode
            CString strResponse = CA2W(strRawResponse, CP_UTF8);

            // Parse JSON to get "url"
            strUrl = ParseUrlFromJson(strResponse);
        }
    }
    catch (CInternetException* pEx) {
        pEx->Delete();
    }
    session.Close();
    return strUrl;
}
// add end fetch URL from API implementation NTTai 20260115

// add start parse JSON response helper NTTai 20260115
CString AuthQRCodeDlg::ParseUrlFromJson(CString strJson)
{
    CString strUrl = L"";

    int posKey = strJson.Find(L"\"url\"");
    if (posKey != -1) {
        int posColon = strJson.Find(L":", posKey);
        if (posColon != -1) {
            int startQuote = strJson.Find(L"\"", posColon);
            if (startQuote != -1) {
                int endQuote = strJson.Find(L"\"", startQuote + 1);
                if (endQuote != -1) {
                    strUrl = strJson.Mid(startQuote + 1, endQuote - startQuote - 1);
                    strUrl.Replace(L"\\/", L"/");
                }
            }
        }
    }
    return strUrl;
}// add end parse JSON response helper NTTai 20260115
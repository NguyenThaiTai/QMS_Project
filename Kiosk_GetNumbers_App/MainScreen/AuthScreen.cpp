#include "pch.h"
#include "AuthScreen.h"

AuthScreen::AuthScreen(CWnd* pParent) : m_pParent(pParent)
{
    m_fontGreeting.CreatePointFont(350, _T("Arial Bold"));
    m_fontDesc.CreatePointFont(140, _T("Arial"));
    m_fontCardTitle.CreatePointFont(120, _T("Arial Bold"));
    m_fontCardSubDesc.CreatePointFont(95, _T("Arial"));
}

// add start load PNG (icon for button) from resource NTTai 20250103
Gdiplus::Image* LoadPNGFromResource(UINT nIDResource)
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
            Gdiplus::Bitmap* pBmp = new Gdiplus::Bitmap(pTempImg->GetWidth(), pTempImg->GetHeight(), PixelFormat32bppARGB);
            Gdiplus::Graphics g(pBmp);
            g.DrawImage(pTempImg, 0, 0, pTempImg->GetWidth(), pTempImg->GetHeight());

            pFinalImg = pBmp;
            delete pTempImg;
        }
        pStream->Release();
    }
    return pFinalImg;
}
// add end load PNG (icon for button) from resource NTTai 20250103

void AuthScreen::CreateUI(CRect rectClient)
{
    CStatic* pGreeting = new CStatic();
    pGreeting->Create(_T("Chào mừng quý khách"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        CRect(0, 250, rectClient.Width(), 320), m_pParent);
    pGreeting->SetFont(&m_fontGreeting);
    m_vStatics.push_back(pGreeting);

    CStatic* pDesc = new CStatic();
    pDesc->Create(_T("Vui lòng chọn phương thức xác thực danh tính để được ưu tiên phục vụ\nhoặc lấy số thứ tự thông thường"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        CRect(0, 330, rectClient.Width(), 400), m_pParent);
    pDesc->SetFont(&m_fontDesc);
    m_vStatics.push_back(pDesc);

    int nCards = 5;
    int cardW = 220;
    int cardH = 300;
    int gap = 30;
    int totalW = (nCards * cardW) + ((nCards - 1) * gap);
    int startX = (rectClient.Width() - totalW) / 2;
    int startY = 420;

    CString labels[] = { _T("Căn cước công dân"), 
                         _T("Vân tay"),
                         _T("FaceID"), 
                         _T("Mã QR"), 
                         _T("Lấy số thường") };

    CString descs[] = {
    _T("Đặt thẻ CCCD gắn chip vào đầu đọc để xác thực nhanh"),
    _T("Sử dụng vân tay đã đăng ký để xác thực sinh trắc học"),
    _T("Xác thực nhanh chóng bằng nhận diệnx khuôn mặt"),
    _T("Mở ứng dụng ngân hàng và quét mã QR trên màn hình"),
    _T("Dành cho khách hàng chưa đăng ký hoặc khách vãng lai")
    };

	// add start assign icon for button NTTai 20260103
    UINT resIDs[] = { IDB_PNG_CCCD, IDB_PNG_FINGER, IDB_PNG_FACE, IDB_PNG_QR, IDB_PNG_TICKET };
    for (auto img : m_vImages) if (img) delete img;
    m_vImages.clear();
    for (int i = 0; i < 5; i++)
    {
        Gdiplus::Image* pImg = LoadPNGFromResource(resIDs[i]);
        if (pImg) m_vImages.push_back(pImg);
    }
	// add end assign icon for button NTTai 20260103

    COLORREF colorAgriRed = RGB(162, 32, 45);   
    COLORREF colorWhite = RGB(255, 255, 255); 
    COLORREF colorDark = RGB(13, 18, 27);  

    for (int i = 0; i < nCards; i++)
    {
        int currentCardX = startX + i * (cardW + gap);

        CCardButton* pBtn = new CCardButton();
        pBtn->Create(labels[i], WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | BS_MULTILINE,
            CRect(currentCardX, startY, currentCardX + cardW, startY + cardH),
            m_pParent, 2000 + i);

        pBtn->m_strDesc = descs[i];
        pBtn->m_pFontDesc = &m_fontCardSubDesc;

        pBtn->m_bDontUseWinXPTheme = TRUE;
        pBtn->SetFont(&m_fontCardTitle);

        if (i == 4) {
            pBtn->SetFaceColor(colorAgriRed);
            pBtn->SetTextColor(colorWhite);
            pBtn->m_clrDesc = colorWhite;
        }
        else {
            pBtn->SetFaceColor(colorWhite);
            pBtn->SetTextColor(colorAgriRed);
            pBtn->m_clrDesc = RGB(94, 107, 126);
        }

        m_vButtons.push_back(pBtn);

		// add start assign icon for button NTTai 20260103
        if (i < m_vImages.size()) {
            pBtn->m_pIcon = m_vImages[i];
        }
        // add end assign icon for button NTTai 20260103
    }

}

void AuthScreen::CleanUp()
{
    for (auto b : m_vButtons) { if (b) { b->DestroyWindow(); delete b; } }
    for (auto s : m_vStatics) { if (s) { s->DestroyWindow(); delete s; } }
    m_vButtons.clear(); m_vStatics.clear();
}

AuthScreen::~AuthScreen() { CleanUp(); }

void AddRoundedRectToPath(Gdiplus::GraphicsPath& path, Gdiplus::Rect rect, int radius) {
    int diameter = radius * 2;
    path.AddArc(rect.X, rect.Y, diameter, diameter, 180, 90);
    path.AddArc(rect.X + rect.Width - diameter, rect.Y, diameter, diameter, 270, 90);
    path.AddArc(rect.X + rect.Width - diameter, rect.Y + rect.Height - diameter, diameter, diameter, 0, 90);
    path.AddArc(rect.X, rect.Y + rect.Height - diameter, diameter, diameter, 90, 90);
    path.CloseFigure();
}

void CCardButton::OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState)
{
    Gdiplus::Graphics graphics(pDC->GetSafeHdc());
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

    //DRAW SHADOW - Create floating effect
    Gdiplus::Rect gdiRect(rect.left, rect.top, rect.Width(), rect.Height());

    //DRAW ROUNDED CARD BACKGROUND
    Gdiplus::GraphicsPath cardPath;
    AddRoundedRectToPath(cardPath, gdiRect, 30); // Rounded corner 30px

    COLORREF backColor = (m_clrRegular == RGB(255, 255, 255)) ? RGB(162, 32, 45) : RGB(255, 255, 255);
    Gdiplus::SolidBrush backBrush(Gdiplus::Color(255, GetRValue(backColor), GetGValue(backColor), GetBValue(backColor)));
    graphics.FillPath(&backBrush, &cardPath);

    //DRAW ICON (Coloring and alignment)
    if (m_pIcon) {
        int iconSize = 70; // Shrink icon slightly for refinement
        int iconX = rect.left + (rect.Width() - iconSize) / 2;
        int iconY = rect.top + 55;

        Gdiplus::ImageAttributes imAttr;
        Gdiplus::ColorMatrix colorMatrix = { 1,0,0,0,0, 0,1,0,0,0, 0,0,1,0,0, 0,0,0,1,0, 0,0,0,0,1 };

        if (m_clrRegular == RGB(255, 255, 255)) { // Red button -> White icon
            colorMatrix.m[4][0] = colorMatrix.m[4][1] = colorMatrix.m[4][2] = 1.0f;
        }
        else { // White button -> Gray brown icon
            colorMatrix.m[0][0] = 0.5f; colorMatrix.m[1][1] = 0.3f; colorMatrix.m[2][2] = 0.3f;
        }
        imAttr.SetColorMatrix(&colorMatrix);
        graphics.DrawImage(m_pIcon, Gdiplus::Rect(iconX, iconY, iconSize, iconSize),
            0, 0, m_pIcon->GetWidth(), m_pIcon->GetHeight(), Gdiplus::UnitPixel, &imAttr);
    }

    //DRAW TITLE (Bold)
    pDC->SetBkMode(TRANSPARENT);
    pDC->SelectObject(GetFont());
    pDC->SetTextColor(m_clrRegular);
    CRect rectTitle = rect; rectTitle.top += 165;
    pDC->DrawText(strText, rectTitle, DT_CENTER | DT_SINGLELINE);

    //DRAW DESCRIPTION (Alignment and Spacing)
    if (m_pFontDesc && !m_strDesc.IsEmpty()) {
        pDC->SelectObject(m_pFontDesc);
        pDC->SetTextColor(m_clrDesc);
        CRect rectDesc = rect; rectDesc.top += 215; rectDesc.left += 20; rectDesc.right -= 20;
        pDC->DrawText(m_strDesc, rectDesc, DT_CENTER | DT_WORDBREAK);
    }
}
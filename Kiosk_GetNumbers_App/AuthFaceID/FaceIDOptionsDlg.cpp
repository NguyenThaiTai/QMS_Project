#include "pch.h"
#include "FaceIDOptionsDlg.h"

IMPLEMENT_DYNAMIC(FaceOptionDlg, CDialogEx)

BEGIN_MESSAGE_MAP(FaceOptionDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_WM_TIMER()
END_MESSAGE_MAP()

FaceOptionDlg::FaceOptionDlg(CWnd* pParent)
    : CDialogEx(IDD_FACEID_OPTIONS_DIALOG, pParent) // Dùng chung ID Dialog resource
{
    m_bNewPressed = false;
    m_bExistPressed = false;
    m_pIconFace = nullptr;
}

FaceOptionDlg::~FaceOptionDlg()
{
    if (m_pIconFace) delete m_pIconFace;
}

BOOL FaceOptionDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);

    m_pIconFace = Common::LoadPNGFromResource(IDB_PNG_FACE);

    SetTimer(1, 1000, NULL);
    return TRUE;
}

BOOL FaceOptionDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

void FaceOptionDlg::OnPaint()
{
    CPaintDC dc(this);
    CRect rect; 
    GetClientRect(&rect);

    CDC memDC; 
    memDC.CreateCompatibleDC(&dc);
    CBitmap bmp; 
    bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
    CBitmap* pOld = memDC.SelectObject(&bmp);

    Gdiplus::Graphics g(memDC.GetSafeHdc());
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);

    g.Clear(Gdiplus::Color(255, 235, 235, 235));
    CHeaderUI::DrawSharedHeader(&memDC, rect);

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2 + 60;

    DrawTitle(g, cx, cy);

    float cardW = 380.0f;
    float cardH = 320.0f;
    float gap = 40.0f;

    m_rectOptNew = Gdiplus::RectF(cx - cardW - gap / 2, cy - 150, cardW, cardH);
    m_rectOptExist = Gdiplus::RectF(cx + gap / 2, cy - 150, cardW, cardH);

    DrawOptionCard(g, m_rectOptNew, nullptr, L"Chưa đăng ký", L"Đăng ký khuôn mặt mới để nhận diện nhanh chóng", m_bNewPressed);

    DrawOptionCard(g, m_rectOptExist, m_pIconFace, L"Đã đăng ký", L"Sử dụng khuôn mặt đã đăng ký để xác thực", m_bExistPressed);

    CButtonUI::DrawBackButton(g, cx, rect.Height() - 200, m_rectBack);

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOld);
}

void FaceOptionDlg::DrawTitle(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::Font fontTitle(L"Segoe UI", 32, Gdiplus::FontStyleBold);
    Gdiplus::Font fontSub(L"Segoe UI", 13, Gdiplus::FontStyleRegular);
    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);

    g.DrawString(L"Xác thực FaceID", -1, &fontTitle, Gdiplus::PointF(cx, cy - 350), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));
    g.DrawString(L"Vui lòng chọn trạng thái đăng ký của bạn để tiếp tục", -1, &fontSub, Gdiplus::PointF(cx, cy - 250), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 108, 117, 125)));
}

void FaceOptionDlg::DrawOptionCard(Gdiplus::Graphics& g, Gdiplus::RectF rect, Gdiplus::Image* pIcon, CString strTitle, CString strDesc, bool bIsPressed)
{
    Gdiplus::GraphicsPath path;
    CButtonUI::AddRoundedRectToPath(path, rect, 20.0f);
    Gdiplus::Color bgColor = bIsPressed ? Gdiplus::Color(255, 245, 245, 245) : Gdiplus::Color::White;
    g.FillPath(&Gdiplus::SolidBrush(bgColor), &path);
    g.DrawPath(&Gdiplus::Pen(Gdiplus::Color(255, 230, 230, 230), 2.0f), &path);

    float iconBgSize = 100.0f;
    float iconCx = rect.X + rect.Width / 2;
    float iconCy = rect.Y + 80.0f;
    Gdiplus::SolidBrush bgIconBrush(Gdiplus::Color(255, 252, 235, 236));
    g.FillEllipse(&bgIconBrush, iconCx - iconBgSize / 2, iconCy - iconBgSize / 2, iconBgSize, iconBgSize);

    if (pIcon) {
        float iconSize = 50.0f;
        Gdiplus::RectF iconRect(iconCx - iconSize / 2, iconCy - iconSize / 2, iconSize, iconSize);

        Gdiplus::ImageAttributes imAttr;
        Gdiplus::ColorMatrix colorMatrix = {
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.63f, 0.12f, 0.17f, 0.0f, 1.0f
        };
        imAttr.SetColorMatrix(&colorMatrix, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

        g.DrawImage(pIcon, iconRect, 0, 0, (float)pIcon->GetWidth(), (float)pIcon->GetHeight(), Gdiplus::UnitPixel, &imAttr);
    }
    else {
        DrawAddUserIcon(g, iconCx, iconCy, 60.0f, Gdiplus::Color(255, 162, 32, 45));
    }

    Gdiplus::StringFormat format; format.SetAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font fontTitle(L"Segoe UI", 16, Gdiplus::FontStyleBold);
    g.DrawString(strTitle, -1, &fontTitle, Gdiplus::PointF(iconCx, rect.Y + 160), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

    Gdiplus::Font fontDesc(L"Segoe UI", 11, Gdiplus::FontStyleRegular);
    Gdiplus::RectF textRect(rect.X + 30, rect.Y + 200, rect.Width - 60, 100);
    g.DrawString(strDesc, -1, &fontDesc, textRect, &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 108, 117, 125)));
}

void FaceOptionDlg::DrawAddUserIcon(Gdiplus::Graphics& g, float x, float y, float size, Gdiplus::Color color)
{
    Gdiplus::SolidBrush brush(color);
    float personShiftX = size * 0.15f;
    float headSize = size * 0.42f;
    float headX = x + personShiftX - (headSize / 2.0f);
    float headY = y - size * 0.35f;
    g.FillEllipse(&brush, headX, headY, headSize, headSize);
    float bodyW = size * 0.75f;
    float bodyH = size * 0.45f;
    float bodyX = x + personShiftX - (bodyW / 2.0f);
    float bodyY = y + size * 0.12f;
    Gdiplus::GraphicsPath pathBody;
    pathBody.AddArc(bodyX, bodyY, bodyW, bodyH, 180.0f, 180.0f);
    pathBody.CloseFigure();
    g.FillPath(&brush, &pathBody);
    float plusSize = size * 0.35f;
    float plusThickness = plusSize * 0.32f;
    float plusCX = x - size * 0.28f;
    float plusCY = y + size * 0.05f;
    float gapSize = plusThickness * 1.5f;
    Gdiplus::Pen penEraser(Gdiplus::Color::White, gapSize);
    g.DrawLine(&penEraser, plusCX, plusCY - plusSize / 1.5f, plusCX, plusCY + plusSize / 1.5f);
    g.DrawLine(&penEraser, plusCX - plusSize / 1.5f, plusCY, plusCX + plusSize / 1.5f, plusCY);
    g.FillRectangle(&brush, plusCX - plusSize / 2.0f, plusCY - plusThickness / 2.0f, plusSize, plusThickness);
    g.FillRectangle(&brush, plusCX - plusThickness / 2.0f, plusCY - plusSize / 2.0f, plusThickness, plusSize);
}

void FaceOptionDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);
    if (m_rectOptNew.Contains(p)) m_bNewPressed = true;
    if (m_rectOptExist.Contains(p)) m_bExistPressed = true;
    if (m_bNewPressed || m_bExistPressed) Invalidate(FALSE);
    CDialogEx::OnLButtonDown(nFlags, point);
}

void FaceOptionDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);

    if (m_rectOptNew.Contains(p) && m_bNewPressed) {
        AuthIDCardDlg dlg;
        dlg.SetFaceRegisterMode(true);
        dlg.DoModal();
        //EndDialog(IDCANCEL);
    }
    else if (m_rectOptExist.Contains(p) && m_bExistPressed) {   
        AuthFaceIDDlg dlg; 
        dlg.DoModal();
        //EndDialog(IDCANCEL);
    }
    else if (m_rectBack.Contains(p)) {
        EndDialog(IDCANCEL);
    }

    m_bNewPressed = false;
    m_bExistPressed = false;
    Invalidate(FALSE);
    CDialogEx::OnLButtonUp(nFlags, point);
}

void FaceOptionDlg::OnTimer(UINT_PTR nIDEvent) {
    if (nIDEvent == 1) Invalidate(FALSE);
    CDialogEx::OnTimer(nIDEvent);
}
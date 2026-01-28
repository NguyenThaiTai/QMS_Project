// File: NoAuthDlg.cpp
#include "pch.h"
#include "NoAuthDlg.h"

IMPLEMENT_DYNAMIC(NoAuthDlg, CDialogEx)

BEGIN_MESSAGE_MAP(NoAuthDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
    ON_MESSAGE(WM_API_TICKET_RESULT, &NoAuthDlg::OnApiResult)
    // add start register message handler for list loading NTTai 20260123
    ON_MESSAGE(WM_API_LIST_RESULT, &NoAuthDlg::OnApiListResult)
    // add end register message handler for list loading NTTai 20260123
END_MESSAGE_MAP()

NoAuthDlg::NoAuthDlg(CWnd* pParent)
    : CDialogEx(IDD_NO_AUTH_DIALOG, pParent)
{
    m_uiItems.clear();
}

NoAuthDlg::~NoAuthDlg() {}

BOOL NoAuthDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL NoAuthDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);

    // add start init async loading and timers NTTai 20260123
    UpdateServiceList();       // Call immediately
    SetTimer(1, 1000, NULL);   // Clock timer
    SetTimer(2, 5000, NULL);   // Refresh list timer
    // add end init async loading and timers NTTai 20260123

    return TRUE;
}

// add start async service list updating logic NTTai 20260123
void NoAuthDlg::UpdateServiceList()
{
    if (m_bIsFetchingList || m_bIsLoading) return;
    m_bIsFetchingList = true;
    AfxBeginThread(ListFetcherThreadProc, this->GetSafeHwnd());
}
UINT __cdecl NoAuthDlg::ListFetcherThreadProc(LPVOID pParam)
{
    HWND hWnd = (HWND)pParam;
    if (!hWnd) return 1;

    ServiceListResult* pResult = new ServiceListResult();
    pResult->success = false;
    
    // Call ApiService to get list (Thread-safe)
    if (ApiService::GetInstance()->FetchActiveServices(pResult->list)) {
        pResult->success = true;
    }

    ::PostMessage(hWnd, WM_API_LIST_RESULT, 0, (LPARAM)pResult);
    return 0;
}

LRESULT NoAuthDlg::OnApiListResult(WPARAM wParam, LPARAM lParam)
{
    m_bIsFetchingList = false;
    ServiceListResult* pResult = (ServiceListResult*)lParam;
    if (!pResult) return 0;

    if (pResult->success) {
        bool changed = false;
        if (pResult->list.size() != m_uiItems.size()) changed = true;
        else {
            for (size_t i = 0; i < pResult->list.size(); i++) {
                if (pResult->list[i] != m_uiItems[i].data) {
                    changed = true;
                    break;
                }
            }
        }

        if (changed) {
            m_uiItems.clear();
            CRect rClient; 
            GetClientRect(&rClient);
            float cx = rClient.Width() / 2.0f;
            float cy = rClient.Height() / 2.0f + 40;
            float cardW = 980.0f;
            float cardH = 90.0f;
            float gapY = 24.0f;
            float startY = cy - 300.0f;
            float startX = cx - cardW / 2.0f;

            for (size_t i = 0; i < pResult->list.size(); i++) {
                ServiceUIItem uiItem;
                uiItem.data = pResult->list[i];
                uiItem.isPressed = false;
                uiItem.rect = Gdiplus::RectF(startX, startY + i * (cardH + gapY), cardW, cardH);
                m_uiItems.push_back(uiItem);
            }
            Invalidate();
        }
    }
    delete pResult;
    return 0;
}
// add end async service list updating logic NTTai 20260123

void NoAuthDlg::OnPaint()
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
    int cy = rect.Height() / 2 + 40;

    DrawHeaderTitle(g, cx, cy);

    // add start draw loading text if empty NTTai 20260123
    if (m_uiItems.empty()) {
        Gdiplus::Font fontLoad(L"Segoe UI", 14, Gdiplus::FontStyleRegular);
        Gdiplus::SolidBrush brushLoad(Gdiplus::Color(255, 100, 100, 100));
        Gdiplus::StringFormat fmt; fmt.SetAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(L"Đang tải danh sách dịch vụ...", -1, &fontLoad, Gdiplus::PointF((float)cx, (float)cy), &fmt, &brushLoad);
    }
    else {
        DrawServiceList(g, cx, cy);
    }
    // add end draw loading text if empty NTTai 20260123

    CButtonUI::DrawCancelButton(g, cx, cy + 90, m_rectCancelBtn);

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void NoAuthDlg::DrawHeaderTitle(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format; 
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    Gdiplus::Font fontTitle(L"Segoe UI", 30, Gdiplus::FontStyleBold);
    g.DrawString(L"Vui lòng chọn loại giao dịch", -1, &fontTitle, Gdiplus::PointF((float)cx, (float)cy - 420.0f), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

    Gdiplus::Font fontSub(L"Segoe UI", 16, Gdiplus::FontStyleRegular);
    g.DrawString(L"Chọn dịch vụ bạn cần hỗ trợ để lấy số thứ tự ưu tiên", -1, &fontSub, Gdiplus::PointF((float)cx, (float)cy - 350.0f), &format, &Gdiplus::SolidBrush(Gdiplus::Color(255, 108, 117, 125)));
}

void NoAuthDlg::DrawServiceList(Gdiplus::Graphics& g, int cx, int cy)
{
    for (auto& item : m_uiItems) {
        Gdiplus::RectF sRect = item.rect; sRect.Offset(0, 2);
        Gdiplus::GraphicsPath sPath; CButtonUI::AddRoundedRectToPath(sPath, sRect, 18.0f);
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(12, 0, 0, 0)), &sPath);

        Gdiplus::GraphicsPath path; CButtonUI::AddRoundedRectToPath(path, item.rect, 18.0f);
        Gdiplus::Color bgColor = item.isPressed ? Gdiplus::Color(255, 255, 242, 242) : Gdiplus::Color::White;
        g.FillPath(&Gdiplus::SolidBrush(bgColor), &path);

        Gdiplus::Color borderColor = item.isPressed ? Gdiplus::Color(255, 162, 32, 45) : Gdiplus::Color(255, 228, 231, 235);
        float borderWidth = item.isPressed ? 2.5f : 1.2f;
        Gdiplus::Pen p(borderColor, borderWidth); p.SetAlignment(Gdiplus::PenAlignmentInset);
        g.DrawPath(&p, &path);

        Gdiplus::RectF iconBox(item.rect.X + 20, item.rect.Y + 9, 75, 75);
        Gdiplus::GraphicsPath iPath; CButtonUI::AddRoundedRectToPath(iPath, iconBox, 12.0f);
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 253, 245, 245)), &iPath);
        g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)), (Gdiplus::REAL)(iconBox.X + 25), (Gdiplus::REAL)(iconBox.Y + 25), 25.0f, 25.0f);

        Gdiplus::StringFormat fNear; fNear.SetAlignment(Gdiplus::StringAlignmentNear);
        g.DrawString(item.data.title, -1, &Gdiplus::Font(L"Segoe UI", 16, Gdiplus::FontStyleBold), Gdiplus::PointF(iconBox.GetRight() + 25, item.rect.Y + 16), &fNear, &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));
        g.DrawString(item.data.description, -1, &Gdiplus::Font(L"Segoe UI", 11.0f), Gdiplus::PointF(iconBox.GetRight() + 25, item.rect.Y + 56), &fNear, &Gdiplus::SolidBrush(Gdiplus::Color(255, 120, 125, 135)));

        DrawChevron(g, item.rect);
    }
}

void NoAuthDlg::DrawChevron(Gdiplus::Graphics& g, Gdiplus::RectF cardRect)
{
    Gdiplus::Pen pen(Gdiplus::Color(255, 200, 205, 210), 3.0f);
    pen.SetStartCap(Gdiplus::LineCapRound); pen.SetEndCap(Gdiplus::LineCapRound);
    float cx = cardRect.GetRight() - 40;
    float cy = cardRect.Y + cardRect.Height / 2;
    float size = 8.0f;
    g.DrawLine(&pen, cx - size, cy - size, cx, cy);
    g.DrawLine(&pen, cx, cy, cx - size, cy + size);
}

void NoAuthDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_bIsLoading) return;
    Gdiplus::PointF p((float)point.x, (float)point.y);
    for (auto& item : m_uiItems) {
        if (item.rect.Contains(p)) {
            item.isPressed = true;
            CRect rInv((int)item.rect.X, (int)item.rect.Y, (int)item.rect.GetRight() + 2, (int)item.rect.GetBottom() + 2);
            InvalidateRect(&rInv, FALSE);
        }
    }
    if (m_rectCancelBtn.Contains(p)) {
        InvalidateRect(CRect((int)m_rectCancelBtn.X, (int)m_rectCancelBtn.Y, (int)m_rectCancelBtn.GetRight(), (int)m_rectCancelBtn.GetBottom()), FALSE);
    }
    CDialogEx::OnLButtonDown(nFlags, point);
}

void NoAuthDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (m_bIsLoading) return;
    Gdiplus::PointF p((float)point.x, (float)point.y);

    for (auto& item : m_uiItems) {
        if (item.isPressed && item.rect.Contains(p)) {
            item.isPressed = false;
            Invalidate(FALSE);
            StartTicketProcess(item.data.id);
            return;
        }
        item.isPressed = false;
    }

    if (m_rectCancelBtn.Contains(p)) { EndDialog(IDCANCEL); }
    Invalidate(FALSE);
    CDialogEx::OnLButtonUp(nFlags, point);
}

void NoAuthDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1) {
        CRect rect; GetClientRect(&rect);
        CRect rectTime(rect.Width() - 400, 140, rect.Width(), 210);
        InvalidateRect(&rectTime, FALSE);
    }
    // add start handle auto-refresh timer NTTai 20260123
    else if (nIDEvent == 2) {
        UpdateServiceList();
    }
    // add end handle auto-refresh timer NTTai 20260123
    CDialogEx::OnTimer(nIDEvent);
}

void NoAuthDlg::StartTicketProcess(int serviceID)
{
    m_bIsLoading = true;
    BeginWaitCursor();
    TicketProcessParam* pParam = new TicketProcessParam;
    pParam->hWnd = this->GetSafeHwnd();
    pParam->serviceID = serviceID;
    pParam->isAuthenticated = m_bIsAuthenticated;
    pParam->authData = m_authData;
    AfxBeginThread(WorkerThreadProc, pParam);
}

UINT __cdecl NoAuthDlg::WorkerThreadProc(LPVOID pParam)
{
    TicketProcessParam* pInput = (TicketProcessParam*)pParam;
    TicketProcessResult* pResult = new TicketProcessResult();
    pResult->serviceID = pInput->serviceID;

    ApiResponse response = ApiService::GetInstance()->IssueTicket(pInput->serviceID, pInput->isAuthenticated ? &pInput->authData : nullptr);

    pResult->success = response.success;
    if (response.success) pResult->ticketNumber = response.data;
    else pResult->errorMessage = response.message;

    ::PostMessage(pInput->hWnd, WM_API_TICKET_RESULT, 0, (LPARAM)pResult);
    delete pInput;
    return 0;
}

LRESULT NoAuthDlg::OnApiResult(WPARAM wParam, LPARAM lParam)
{
    m_bIsLoading = false;
    EndWaitCursor();
    TicketProcessResult* pResult = (TicketProcessResult*)lParam;
    if (!pResult) return 0;
    if (pResult->success) {
        CString strTitle = L"Dịch vụ";
        for (auto& item : m_uiItems) {
            if (item.data.id == pResult->serviceID) { 
                strTitle = item.data.title; 
                break; 
            }
        }
        DisplayNumbersDlg displayDlg(pResult->serviceID, strTitle, pResult->ticketNumber, this);
        displayDlg.DoModal();
        EndDialog(IDOK);
    }
    else {
        MessageBox(pResult->errorMessage, L"Thông báo", MB_ICONWARNING);
    }
    delete pResult;
    return 0;
}

void NoAuthDlg::SetAuthenticatedData(const CitizenCardData& data)
{
    m_authData = data;
    m_bIsAuthenticated = true;
}
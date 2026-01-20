#include "pch.h"
#include "NoAuthDlg.h"
#include "HeaderUI.h"
#include "DisplayNumbersDlg.h"
#include "AuthScreen.h"
#include "resource.h"
#include <atlstr.h>
IMPLEMENT_DYNAMIC(NoAuthDlg, CDialogEx)

BEGIN_MESSAGE_MAP(NoAuthDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
    ON_WM_LBUTTONUP()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

NoAuthDlg::NoAuthDlg(CWnd* pParent)
    : CDialogEx(IDD_NO_AUTH_DIALOG, pParent)
{
    m_services.clear();
}

NoAuthDlg::~NoAuthDlg() {}

BOOL NoAuthDlg::OnEraseBkgnd(CDC* pDC) { return TRUE; }

BOOL NoAuthDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    CHeaderUI::SetFullScreen(this);

    // add start initialize data fetching and timers NTTai 20260114
    FetchServicesFromServer(); // Fetch immediately on startup
    SetTimer(1, 1000, NULL);   // Timer for clock (1s)
    SetTimer(2, 5000, NULL);   // Timer for auto-refresh data (5s)
    // add end initialize data fetching and timers NTTai 20260114

    return TRUE;
}

void NoAuthDlg::OnPaint()
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
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);
    g.Clear(Gdiplus::Color(255, 235, 235, 235));
    CHeaderUI::DrawSharedHeader(&memDC, rect);

    int cx = rect.Width() / 2;
    int cy = rect.Height() / 2 + 40;

    DrawHeaderTitle(g, cx, cy);
    DrawServiceList(g, cx, cy);
    CButtonUI::DrawCancelButton(g, cx, cy + 90, m_rectCancelBtn);

    dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
}

void NoAuthDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    Gdiplus::PointF p((float)point.x, (float)point.y);
    for (auto& item : m_services) {
        if (item.rect.Contains(p)) {
            item.isPressed = true;
            CRect rInv((int)item.rect.X, (int)item.rect.Y, (int)item.rect.GetRight(), (int)item.rect.GetBottom());
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
    Gdiplus::PointF p((float)point.x, (float)point.y);

    for (auto& item : m_services) {
        if (item.isPressed && item.rect.Contains(p)) {

            BeginWaitCursor();
            CString strRealTicket = L"";

            if (m_bIsAuthenticated) {
                strRealTicket = RequestAuthenticatedTicket(item.id);
            }
            if (strRealTicket.IsEmpty()) {
                strRealTicket = RequestTicketFromServer(item.id);
            }
            EndWaitCursor();
            DisplayNumbersDlg displayNumbers_dlg(item.id, item.title.c_str(), strRealTicket, this);
            displayNumbers_dlg.DoModal();
            EndDialog(IDOK);
            return;
        }
        item.isPressed = false;
    }

    // add start handle cancel button click NTTai 20260114
    if (m_rectCancelBtn.Contains(p)) {
        EndDialog(IDCANCEL);
    }
	// add end handle cancel button visual reset NTTai 20260114
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
    // add start handle auto-refresh timer logic NTTai 20260114
    else if (nIDEvent == 2)
    {
        FetchServicesFromServer();
    }
    // add end handle auto-refresh timer logic NTTai 20260114
    CDialogEx::OnTimer(nIDEvent);
}

void NoAuthDlg::DrawHeaderTitle(Gdiplus::Graphics& g, int cx, int cy)
{
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);

    Gdiplus::Font fontTitle(L"Segoe UI", 30, Gdiplus::FontStyleBold);
    g.DrawString(L"Vui lòng chọn loại giao dịch", -1, &fontTitle,
        Gdiplus::PointF((float)cx, (float)cy - 420.0f), &format,
        &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

    Gdiplus::Font fontSub(L"Segoe UI", 16, Gdiplus::FontStyleRegular);
    g.DrawString(L"Chọn dịch vụ bạn cần hỗ trợ để lấy số thứ tự ưu tiên",
        -1,
        &fontSub,
        Gdiplus::PointF((float)cx,
            (float)cy - 350.0f),
        &format,
        &Gdiplus::SolidBrush(Gdiplus::Color(255, 108, 117, 125)));
}

// add start draw service list grid UI NTTai 20260114
void NoAuthDlg::DrawServiceList(Gdiplus::Graphics& g, int cx, int cy)
{
    float cardW = 980.0f;
    float cardH = 90.0f;
    float gapY = 24.0f;
    float startY = (float)cy - 300;
    float startX = (float)cx - cardW / 2;

    for (int i = 0; i < m_services.size(); i++) {
        m_services[i].rect = Gdiplus::RectF(startX, startY + i * (cardH + gapY), cardW, cardH);

        // Shadow
        Gdiplus::RectF sRect = m_services[i].rect; sRect.Offset(0, 2);
        Gdiplus::GraphicsPath sPath; CButtonUI::AddRoundedRectToPath(sPath, sRect, 18.0f);
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(12, 0, 0, 0)), &sPath);

        // Main Card
        Gdiplus::GraphicsPath path;
        CButtonUI::AddRoundedRectToPath(path, m_services[i].rect, 18.0f);
        Gdiplus::Color bgColor = m_services[i].isPressed ? Gdiplus::Color(255, 255, 242, 242) : Gdiplus::Color::White;
        g.FillPath(&Gdiplus::SolidBrush(bgColor), &path);

        Gdiplus::Color borderColor = m_services[i].isPressed ? Gdiplus::Color(255, 162, 32, 45) : Gdiplus::Color(255, 228, 231, 235);
        float borderWidth = m_services[i].isPressed ? 2.5f : 1.2f;
        Gdiplus::Pen p(borderColor, borderWidth);
        p.SetAlignment(Gdiplus::PenAlignmentInset);
        g.DrawPath(&p, &path);

        // Icon Box
        Gdiplus::RectF iconBox(startX + 20, m_services[i].rect.Y + 9, 75, 75);
        Gdiplus::GraphicsPath iPath; CButtonUI::AddRoundedRectToPath(iPath, iconBox, 12.0f);
        g.FillPath(&Gdiplus::SolidBrush(Gdiplus::Color(255, 253, 245, 245)), &iPath);
        g.FillEllipse(&Gdiplus::SolidBrush(Gdiplus::Color(255, 162, 32, 45)),
            (Gdiplus::REAL)(iconBox.X + 25),
            (Gdiplus::REAL)(iconBox.Y + 25),
            (Gdiplus::REAL)25,
            (Gdiplus::REAL)25);

        // Text
        Gdiplus::StringFormat fNear; fNear.SetAlignment(Gdiplus::StringAlignmentNear);
        g.DrawString(m_services[i].title.c_str(),
            -1,
            &Gdiplus::Font(L"Segoe UI", 16, Gdiplus::FontStyleBold),
            Gdiplus::PointF(iconBox.GetRight() + 25, m_services[i].rect.Y + 16),
            &fNear,
            &Gdiplus::SolidBrush(Gdiplus::Color(255, 33, 37, 41)));

        g.DrawString(m_services[i].description.c_str(),
            -1,
            &Gdiplus::Font(L"Segoe UI", 11.0f),
            Gdiplus::PointF(iconBox.GetRight() + 25, m_services[i].rect.Y + 56),
            &fNear,
            &Gdiplus::SolidBrush(Gdiplus::Color(255, 120, 125, 135)));

        DrawChevron(g, m_services[i].rect);
    }
}
// add end draw service list grid UI NTTai 20260114

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

// add start fetch active services from API logic NTTai 20260114
void NoAuthDlg::FetchServicesFromServer()
{
    CInternetSession session(_T("AgribankKioskSession"), 1, INTERNET_OPEN_TYPE_DIRECT); // add direct connection to avoid Proxy issues in LAN

    session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 2000);
    session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 2000);

    CHttpFile* pFile = nullptr;

    try {
        pFile = (CHttpFile*)session.OpenURL(_T("http://10.10.1.21:8081/api/service/activelist"),
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

            CString strResponse = CA2W(strRawResponse, CP_UTF8); // add convert raw response to Unicode
             
            if (!strResponse.IsEmpty()) {
				// add start parse into temporary list NTTai 20260114
                std::vector<ServiceItem> tempServices;
                ParseJSONAndPopulate(strResponse, tempServices);
				// add end parse into temporary list NTTai 20260114
                
				// add start compare temp vs current NTTai 20260114
                bool bChanged = false;
                if (tempServices.size() != m_services.size()) {
                    bChanged = true;
                }
                else {
                    for (size_t i = 0; i < tempServices.size(); i++) {
                        if (tempServices[i] != m_services[i]) {
                            bChanged = true;
                            break;
                        }
                    }
                }
				// add end compare temp vs current NTTai 20260114
                
				// add start update UI only if changed NTTai 20260114
                if (bChanged && tempServices.size() > 0) {
                    m_services = tempServices;
                    Invalidate();
                }
				// add end update UI only if changed NTTai 20260114
            }
        }
    }
    catch (CInternetException* pEx) {
        pEx->Delete();
    }
}
// add end fetch active services from API logic NTTai 20260114

// add start parse JSON response string logic NTTai 20260114
void NoAuthDlg::ParseJSONAndPopulate(CString strJSON, std::vector<ServiceItem>& outList)
{
    outList.clear();

    int len = strJSON.GetLength();
    int currentPos = 0;
	
    int arrayStart = strJSON.Find(L"\"services\"", 0); // add find "services" array NTTai 20260114
    if (arrayStart != -1) currentPos = arrayStart;

    while (currentPos < len) {
        int startObj = strJSON.Find(L"{", currentPos);
        if (startObj == -1) break;

		// add start count braces to extract full object NTTai 20260114
        int endObj = -1;
        int braceCount = 1;
        for (int i = startObj + 1; i < len; i++) {
            if (strJSON[i] == L'{') braceCount++;
            else if (strJSON[i] == L'}') {
                braceCount--;
                if (braceCount == 0) {
                    endObj = i;
                    break;
                }
            }
        }
        if (endObj == -1) break;

        CString strObj = strJSON.Mid(startObj, endObj - startObj + 1);
		// add end count braces to extract full object NTTai 20260114

        // add start get ID NTTai 20260114
        int nId = -1;
        int posId = strObj.Find(L"\"id\"");
        if (posId != -1) {
            int startNum = -1;
            for (int k = posId + 3; k < strObj.GetLength(); k++) {
                wchar_t c = strObj[k];
                if (c >= L'0' && c <= L'9') {
                    if (startNum == -1) startNum = k;
                }
                else if (startNum != -1) {
                    nId = _wtoi(strObj.Mid(startNum, k - startNum));
                    break;
                }
            }
            if (nId == -1 && startNum != -1) nId = _wtoi(strObj.Mid(startNum));
        }
		// add end get ID NTTai 20260114

        // add start get Name NTTai 20260114
        CString strTitle = L"";
        int posName = strObj.Find(L"\"name\"");
        if (posName != -1) {
            int startQuote = strObj.Find(L"\"", posName + 6);
            if (startQuote != -1) {
                int endQuote = strObj.Find(L"\"", startQuote + 1);
                // add start skip escaped quotes NTTai 20260114
                while (endQuote != -1 && strObj[endQuote - 1] == L'\\') {
                    endQuote = strObj.Find(L"\"", endQuote + 1);
                }
				// add end skip escaped quotes NTTai 20260114
                if (endQuote != -1) {
                    CString strRawName = strObj.Mid(startQuote + 1, endQuote - startQuote - 1);
                    strTitle = DecodeJsonString(strRawName);
                }
            }
        }
		// add end get Name NTTai 20260114
      
        if (nId != -1 && !strTitle.IsEmpty()) {
            ServiceItem item;
            item.id = nId;
            item.title = strTitle;
            item.description = L"Giao dịch tại quầy";
            outList.push_back(item);
        }
        currentPos = endObj + 1;
    }
}
// add end parse JSON response string logic NTTai 20260114

// add start decode unicode JSON string helper NTTai 20260114
CString NoAuthDlg::DecodeJsonString(CString strRaw)
{
    CString strResult = L"";
    int len = strRaw.GetLength();
    int i = 0;

    while (i < len) {
        if (i + 5 < len && strRaw[i] == L'\\' && strRaw[i + 1] == L'u') { // add check for unicode escape sequence \uXXXX NTTai 20260114
            CString strHex = strRaw.Mid(i + 2, 4);

            // add start convert Hex to WChar NTTai 20260114
            wchar_t* pEnd;
            long code = wcstol(strHex, &pEnd, 16);
            if (code != 0) {
                strResult += (wchar_t)code;
                i += 6;
            }
            else {
                strResult += strRaw[i];
                i++;
            }
            // add end convert Hex to WChar NTTai 20260114
        }
        else {
            strResult += strRaw[i];
            i++;
        }
    }
    return strResult;
}
// add end decode unicode JSON string helper NTTai 20260114

// add start data mapping helpers for API NTTai 20260114
CStringA MapGenderToAPI(CString strGender) {
    if (strGender.Trim().CompareNoCase(L"Nam") == 0) return "male";
    if (strGender.Trim().CompareNoCase(L"Nữ") == 0) return "female";
    return "other";
}

CStringA MapDateToAPI(CString strDate) {
    if (strDate.GetLength() >= 10 && strDate.GetAt(2) == L'/') { // add check date format DD/MM/YYYY NTTai 20260114
        CString day = strDate.Left(2);
        CString month = strDate.Mid(3, 2);
        CString year = strDate.Right(4);
        CStringA strResult;
        strResult.Format("%S-%S-%S", year, month, day); // add format YYYY-MM-DD NTTai 20260114
        return strResult;
    }
    return CW2A(strDate, CP_UTF8); // add keep original if invalid format NTTai 20260114
}
// add end data mapping helpers for API NTTai 20260114

// add start request ticket for walk-in guest (POST) NTTai 20260114
CString NoAuthDlg::RequestTicketFromServer(int serviceID)
{
    CString strTicket = L"";
    CInternetSession session(_T("AgribankKioskSession"), 1, INTERNET_OPEN_TYPE_DIRECT); // add configure connection NTTai 20260114
    CHttpConnection* pConnection = nullptr;
    CHttpFile* pFile = nullptr;

    try {
        CString strServer = L"10.10.1.21";
        INTERNET_PORT nPort = 8081;

        pConnection = session.GetHttpConnection(strServer, nPort);

        CString strPath;
        strPath.Format(L"/api/ticketAuth/%d", serviceID);

        // add start header JSON NTTai 20260114
        CString strHeaders = L"Content-Type: application/json; charset=utf-8\r\nAccept: application/json";
		// add end header JSON NTTai 20260114

        pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strPath);

        CString strVal = L"Chưa xác thực";
        CStringA strUTF8 = CW2A(strVal, CP_UTF8);

        CStringA strBody;
        strBody.Format(
            "{"
            "\"adapter_type\": null,"
            "\"data\": {"
            "\"cccd\": \"%s\","          // add use %s variable for UTF-8 string NTTai 20260114
            "\"cmnd\": null,"
            "\"full_name\": \"%s\","     // add use %s variable NTTai 20260114
            "\"date_of_birth\": null,"
            "\"sex\": null,"
            "\"address\": null,"
            "\"date_of_issue\": null"
            "}"
            "}",
            (LPCSTR)strUTF8, // add fill into cccd NTTai 20260114
            (LPCSTR)strUTF8  // add fill into full_name NTTai 20260114
        );

        // add send request NTTai 20260114
        pFile->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)(LPCSTR)strBody, strBody.GetLength());

        DWORD dwStatusCode;
        pFile->QueryInfoStatusCode(dwStatusCode);

        if (dwStatusCode == HTTP_STATUS_OK || dwStatusCode == 201) {
            CStringA strRawResponse;
            char buffer[4096];
            UINT nRead = 0;
            while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) {
                buffer[nRead] = '\0';
                strRawResponse += buffer;
            }
            CString strResponse = CA2W(strRawResponse, CP_UTF8);

            TRACE(L"Walk-in API Response: %s\n", strResponse); // add debug log NTTai 20260114

            int posTicketObj = strResponse.Find(L"\"ticket\""); // add parse JSON logic (keep original) NTTai 20260114
            if (posTicketObj == -1) posTicketObj = 0;

            int posKey = strResponse.Find(L"\"ticket_number\"", posTicketObj);

            if (posKey != -1) {
                int posColon = strResponse.Find(L":", posKey);
                if (posColon != -1) {
                    int posValStart = posColon + 1;
                    while (posValStart < strResponse.GetLength() &&
                        (strResponse[posValStart] == L' ' || strResponse[posValStart] == L'\t')) {
                        posValStart++;
                    }

                    if (strResponse[posValStart] == L'\"') {
                        posValStart++;
                        int endQuote = strResponse.Find(L"\"", posValStart);
                        if (endQuote != -1) strTicket = strResponse.Mid(posValStart, endQuote - posValStart);
                    }
                    else {
                        int posEndComma = strResponse.Find(L",", posValStart);
                        int posEndBrace = strResponse.Find(L"}", posValStart);
                        int posEnd = -1;
                        if (posEndComma != -1 && posEndBrace != -1) posEnd = min(posEndComma, posEndBrace);
                        else if (posEndComma != -1) posEnd = posEndComma;
                        else posEnd = posEndBrace;

                        if (posEnd != -1) {
                            strTicket = strResponse.Mid(posValStart, posEnd - posValStart);
                            strTicket.Trim();
                            strTicket.Remove(L'\"');
                        }
                    }
                }
            }
        }
        else {
            // add start show error if server rejects NTTai 20260114
            CStringA strRawResponse;
            char buffer[4096];
            UINT nRead = 0;
            while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) {
                buffer[nRead] = '\0';
                strRawResponse += buffer;
            }
            CString strResponse = CA2W(strRawResponse, CP_UTF8);
            CString strMsg;
            strMsg.Format(L"Lỗi Server (%d):\n%s", dwStatusCode, strResponse);
            AfxMessageBox(strMsg);
			// add end show error if server rejects NTTai 20260114
        }
    }
    catch (CInternetException* pEx) {
        pEx->Delete();
    }

    if (pFile) { pFile->Close(); delete pFile; }
    if (pConnection) { pConnection->Close(); delete pConnection; }
    session.Close();

    return strTicket;
}
// add end request ticket for walk-in guest (POST) NTTai 20260114

// add start request ticket for authenticated customer (POST) NTTai 20260114
CString NoAuthDlg::RequestAuthenticatedTicket(int serviceID)
{
    CString strTicket = L"";
    CInternetSession session(_T("KioskSession"));
    CHttpConnection* pConnection = nullptr;
    CHttpFile* pFile = nullptr;

    try {
        // add start configure server NTTai 20260114
        CString strServer = L"10.10.1.21";
        INTERNET_PORT nPort = 8081;
        // add end configure server NTTai 20260114

        CString strPath;
        strPath.Format(L"/api/ticketAuth/%d", serviceID);

        pConnection = session.GetHttpConnection(strServer, nPort);

        CString strHeaders = L"Content-Type: application/json; charset=utf-8\r\nAccept: application/json";

        pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, strPath);

        // add start prepare JSON body matching Postman sample NTTai 20260114
        CStringA strBody;
        strBody.Format(
            "{"
            "\"adapter_type\": \"NFC\","
            "\"data\": {"
            "\"cccd\": \"%s\","
            "\"cmnd\": \"%s\","
            "\"full_name\": \"%s\","
            "\"date_of_birth\": \"%s\","
            "\"sex\": \"%s\","
            "\"address\": \"%s\","
            "\"date_of_issue\": \"%s\""
            "}"
            "}",
            (LPCSTR)CW2A(m_authData.strIDNumber, CP_UTF8),
            (LPCSTR)CW2A(m_authData.strCMND, CP_UTF8),
            (LPCSTR)CW2A(m_authData.strFullName, CP_UTF8),
            (LPCSTR)MapDateToAPI(m_authData.strDateOfBirth), // add convert date NTTai 20260114
            (LPCSTR)MapGenderToAPI(m_authData.strGender),    // add convert gender NTTai 20260114
            (LPCSTR)CW2A(m_authData.strPlaceOfResidence, CP_UTF8),
            (LPCSTR)MapDateToAPI(m_authData.strDateOfIssue)
        );
        // add end prepare JSON body matching Postman sample NTTai 20260114

        // add send request NTTai 20260114
        pFile->SendRequest(strHeaders, strHeaders.GetLength(), (LPVOID)(LPCSTR)strBody, strBody.GetLength());

        // add start process response result NTTai 20260114
        DWORD dwStatusCode;
        pFile->QueryInfoStatusCode(dwStatusCode);

        if (dwStatusCode == 200 || dwStatusCode == 201) {
            CStringA strRawResponse;
            char buffer[4096];
            UINT nRead = 0;
            while ((nRead = pFile->Read(buffer, sizeof(buffer) - 1)) > 0) {
                buffer[nRead] = '\0';
                strRawResponse += buffer;
            }
            CString strResponse = CA2W(strRawResponse, CP_UTF8);
            TRACE(L"Auth API Response: %s\n", strResponse);

            int posTicketObj = strResponse.Find(L"\"ticket\""); // add find "ticket" object first NTTai 20260114
            if (posTicketObj != -1) {
                int posTicketNumKey = strResponse.Find(L"\"ticket_number\"", posTicketObj);
                if (posTicketNumKey != -1) {
                    int posColon = strResponse.Find(L":", posTicketNumKey);
                    if (posColon != -1) {
                        // add start  find opening quote of value NTTai 20260114
                        int startQuote = strResponse.Find(L"\"", posColon);
                        if (startQuote != -1) {
                            int endQuote = strResponse.Find(L"\"", startQuote + 1);
                            if (endQuote != -1) {
                                strTicket = strResponse.Mid(startQuote + 1, endQuote - startQuote - 1);
                            }
                        }
                        // add end find opening quote of value NTTai 20260114
                    }
                }
            }
        }
        else {
            CString strErr;
            strErr.Format(L"Server Error: %d", dwStatusCode);
            AfxMessageBox(strErr);
        }
        // add end process response result NTTai 20260114
    }
    catch (CInternetException* pEx) {
        pEx->Delete();
        AfxMessageBox(L"Lỗi kết nối Server (Auth Ticket)!");
    }

    if (pFile) delete pFile;
    if (pConnection) delete pConnection;
    session.Close();

    return strTicket;
}
// add end request ticket for authenticated customer (POST) NTTai 20260114

void NoAuthDlg::SetAuthenticatedData(const CitizenCardData& data)
{
    m_authData = data;
    m_bIsAuthenticated = true;
}


#include "pch.h"
#include "HeaderUI.h"
#include <afxwin.h>

CString CHeaderUI::GetFormattedDateTime() {
	CTime now = CTime::GetCurrentTime();
	CString days[] = {	_T(""), 
						_T("Chủ Nhật"), 
						_T("Thứ Hai"), 
						_T("Thứ Ba"), 
						_T("Thứ Tư"), 
						_T("Thứ Năm"), 
						_T("Thứ Sáu"), 
						_T("Thứ Bảy") };
	return days[now.GetDayOfWeek()] + _T(", ") + now.Format(_T("%d/%m/%Y\n%I:%M:%S %p"));
}

void CHeaderUI::DrawSharedHeader(CDC* pDC, CRect rectClient)
{
	Gdiplus::Graphics g(pDC->GetSafeHdc());
	g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	g.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);

	// add start draw header title NTTai 20260105
	Gdiplus::Font titleFont(L"Arial", 11, Gdiplus::FontStyleBold);
	Gdiplus::SolidBrush agriRedBrush(Gdiplus::Color(255, 147, 30, 31));
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	g.DrawString(L"NGÂN HÀNG NÔNG NGHIỆP VÀ PHÁT TRIỂN NÔNG THÔN VIỆT NAM", -1,
		&titleFont, Gdiplus::RectF(0, 20, (float)rectClient.Width(), 30), &format, &agriRedBrush);
	// add end draw header title NTTai 20260105

	// add start calculate position of Logo and "AGRIBANK" NTTai 20260105
	int imgW = 55, gap = 15, textW = 380;
	int totalW = imgW + gap + textW;
	int startX = (rectClient.Width() - totalW) / 2;
	int posY = 60;
	// add end calculate position of Logo and "AGRIBANK" NTTai 20260105

	// add start draw logo NTTai 20260105
	Gdiplus::Image* pLogo = Gdiplus::Bitmap::FromResource(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_BITMAP1));
	if (pLogo) {
		g.DrawImage(pLogo, startX, posY, imgW, imgW);
		delete pLogo;
	}
	// add end draw logo NTTai 20260105

	// add sart draw "AGRIBANK" NTTai 20260105
	Gdiplus::Font agriFont(L"Times New Roman", 40, Gdiplus::FontStyleBold);
	g.DrawString(L"AGRIBANK", -1, &agriFont, Gdiplus::PointF((float)startX + imgW + gap, (float)posY - 9), &agriRedBrush);
	// add end draw "AGRIBANK" NTTai 20260105

	// add start draw red line NTTai 20260105
	Gdiplus::Pen linePen(Gdiplus::Color(255, 147, 30, 31), 2);
	float lineX1 = 10.0f;
	float lineX2 = (float)rectClient.Width() - 10.0f;
	float lineY = 130.0f;
	g.DrawLine(&linePen, lineX1, lineY, lineX2, lineY);
	// add end draw red line NTTai 20260105

	// add start draw date-time NTTai 20260105
	Gdiplus::Font timeFont(L"Arial", 11, Gdiplus::FontStyleRegular);
	Gdiplus::StringFormat datetimeFormat;
	datetimeFormat.SetAlignment(Gdiplus::StringAlignmentFar);

	CString strTime = GetFormattedDateTime();
	g.DrawString(strTime, -1, &timeFont,
		Gdiplus::RectF(0, 145, (float)rectClient.Width() - 20, 50), &datetimeFormat, &agriRedBrush);
	// add end draw date-time NTTai 20260105
}

// add start set fullscreen NTTai 20260106
void CHeaderUI::SetFullScreen(CWnd* pWnd)
{
	if (!pWnd) return;
	pWnd->ModifyStyleEx(WS_EX_DLGMODALFRAME, 0, SWP_FRAMECHANGED);
	int cx = GetSystemMetrics(SM_CXSCREEN);
	int cy = GetSystemMetrics(SM_CYSCREEN);
	pWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, cx, cy, SWP_SHOWWINDOW);
}
// add end set fullscreen NTTai 20260106
#include "pch.h"
#include "ButtonUI.h"
#include <iostream>

// add start create rounded rectangle path NTTai 20260106
void CButtonUI::AddRoundedRectToPath(Gdiplus::GraphicsPath& path, Gdiplus::RectF rect, float radius)
{
	float d = radius * 2.0f;
	path.AddArc(rect.X, rect.Y, d, d, 180, 90); // Góc trên trái
	path.AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270, 90); // Góc trên phải
	path.AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0, 90); // Góc dưới phải
	path.AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90, 90); // Góc dưới trái
	path.CloseFigure();
}
// add end create rounded rectangle path NTTai 20260106

// add start draw cancel button with Agribank theme NTTai 20260106
void CButtonUI::DrawCancelButton(Gdiplus::Graphics& g, int cx, int cy, Gdiplus::RectF &m_rectCancelBtn)
{
	float btnWidth = 180.0f;
	float btnHeight = 54.0f;
	m_rectCancelBtn = Gdiplus::RectF((float)cx - btnWidth / 2, (float)cy + 320, btnWidth, btnHeight);

	Gdiplus::GraphicsPath path;
	AddRoundedRectToPath(path, m_rectCancelBtn, 27.0f);

	Gdiplus::SolidBrush btnBgBrush(Gdiplus::Color(255, 248, 249, 252));
	g.FillPath(&btnBgBrush, &path);

	Gdiplus::Pen borderPen(Gdiplus::Color(255, 162, 32, 45), 1.5f);
	g.DrawPath(&borderPen, &path);

	Gdiplus::Font font(L"Segoe UI", 12, Gdiplus::FontStyleBold);
	Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 162, 32, 45));
	Gdiplus::StringFormat format;
	format.SetAlignment(Gdiplus::StringAlignmentCenter);
	format.SetLineAlignment(Gdiplus::StringAlignmentCenter);

	std::wstring btnText = L" \x2190  Hủy bỏ";
	g.DrawString(btnText.c_str(), -1, &font, m_rectCancelBtn, &format, &textBrush);
}
// add end draw cancel button with Agribank theme NTTai 20260106
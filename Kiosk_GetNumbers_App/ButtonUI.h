#pragma once
class CButtonUI
{
public:
	static void DrawCancelButton(Gdiplus::Graphics& g, int cx, int cy, Gdiplus::RectF &m_rectCancelBtn);
	static void AddRoundedRectToPath(Gdiplus::GraphicsPath& path, Gdiplus::RectF rect, float radius);
};


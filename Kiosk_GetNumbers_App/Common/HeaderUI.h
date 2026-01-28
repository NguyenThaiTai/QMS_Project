// SharedUI.h
#pragma once
#include <gdiplus.h>
#include "../resource.h"

class CHeaderUI
{
public:
    static CString GetFormattedDateTime();
    static void DrawSharedHeader(CDC* pDC, CRect rectClient);
    static void SetFullScreen(CWnd* pWnd);
};
#pragma once
#include <vector>
#include <atlimage.h>
#include "resource.h"
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
// add start set description text and icon to button NTTais 20250103
class CCardButton : public CMFCButton
{
public:
    CString m_strDesc;
    CFont* m_pFontDesc;
    COLORREF m_clrDesc;
    Gdiplus::Image* m_pIcon;

    virtual void OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState) override;
};
// add end set description text and icon to button NTTai 20250103

class AuthScreen
{
public:
    AuthScreen(CWnd* pParent);
    virtual ~AuthScreen();
    void CreateUI(CRect rectClient);
    void CleanUp();

private:
    CWnd* m_pParent;
    CFont m_fontGreeting;
    CFont m_fontDesc;
    CFont m_fontCardTitle;
    CFont m_fontCardSubDesc;
    std::vector<CMFCButton*> m_vButtons;
    std::vector<CStatic*> m_vStatics;
    std::vector<Gdiplus::Image*> m_vImages; // add vector to hold images (icon of button)
};
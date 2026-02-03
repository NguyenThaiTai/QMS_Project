#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h"
#include "../Common/HeaderUI.h"
#include "../Common/ButtonUI.h"
#include "../AuthIDCard/AuthIDCardDlg.h" 
#include "../AuthFaceID/AuthFaceIDDlg.h"
#include "../Common/Common.h"

class FaceOptionDlg : public CDialogEx
{
    DECLARE_DYNAMIC(FaceOptionDlg)

public:
    FaceOptionDlg(CWnd* pParent = nullptr);
    virtual ~FaceOptionDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_FACEID_OPTIONS_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()

private:
    Gdiplus::RectF m_rectOptNew;
    Gdiplus::RectF m_rectOptExist;
    Gdiplus::RectF m_rectBack;

    bool m_bNewPressed;
    bool m_bExistPressed;

    Gdiplus::Image* m_pIconFace;

    void DrawTitle(Gdiplus::Graphics& g, int cx, int cy);
    void DrawOptionCard(Gdiplus::Graphics& g, Gdiplus::RectF rect, Gdiplus::Image* pIcon, CString strTitle, CString strDesc, bool bIsPressed);
    void DrawAddUserIcon(Gdiplus::Graphics& g, float x, float y, float size, Gdiplus::Color color);
};
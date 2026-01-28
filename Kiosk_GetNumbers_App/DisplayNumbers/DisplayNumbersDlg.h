#pragma once
#include "afxdialogex.h"
#include <gdiplus.h>
#include "../resource.h" 
#include "../Common/ButtonUI.h"
#include "../Common/HeaderUI.h"

class DisplayNumbersDlg : public CDialogEx
{
    DECLARE_DYNAMIC(DisplayNumbersDlg)
public:
    DisplayNumbersDlg(int serviceID, CString serviceTitle, CString ticketNumber, CWnd* pParent = nullptr);
    virtual ~DisplayNumbersDlg();

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DISPLAY_NUMBERS_DIALOG };
#endif

protected:
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    DECLARE_MESSAGE_MAP()

private:
    // add start drawing modules NTTai 20260109
    void DrawMainTicket(Gdiplus::Graphics& g, int cx, int cy);
    void DrawInfoBoxes(Gdiplus::Graphics& g, Gdiplus::RectF cardRect);
    void DrawFooterButtons(Gdiplus::Graphics& g, int cx, int cy);
    void DrawInstructionBar(Gdiplus::Graphics& g, Gdiplus::RectF cardRect, int cx);
	// add end drawing modules NTTai 20260109

    Gdiplus::RectF m_rectPrintBtn;
    Gdiplus::RectF m_rectFinishBtn;
    int m_serviceID;        // add store service ID NTTai 20260109
    CString m_serviceTitle; // add store service title NTTai 20260109
    CString m_strTicketNumber;
    bool m_bPrintPressed = false;
    bool m_bFinishPressed = false;
};


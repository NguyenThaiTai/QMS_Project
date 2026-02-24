#include "pch.h"
#include "TicketPrinter.h"
#include <afxdlgs.h>
#include <shlobj.h>

#define BANK_NAME       L"AGRIBANK"
#define BRANCH_NAME     L"Chi nhánh Trung Tâm Sài Gòn"
#define FOLDER_NAME     L"Agribank_Tickets"

// ============================================================================
// PUBLIC INTERFACE
// ============================================================================

void TicketPrinter::Print(CString strTicketNumber, CString strServiceTitle)
{
    // Retrieve the Default Printer configuration (Silent Print mechanism)
    CPrintDialog printDlg(FALSE);
    if (!printDlg.GetDefaults()) {
        AfxMessageBox(L"Default printer not found. Please check Windows printer settings!", MB_ICONERROR);
        return;
    }

    HDC hPrinterDC = printDlg.GetPrinterDC();
    if (hPrinterDC == NULL) return;

    // Generate PDF file path (Primarily for debugging/logging purposes)
    CString strFullPath = GeneratePDFPath(strTicketNumber);

    // Configure Print Document Info
    CDC dc;
    dc.Attach(hPrinterDC);
    DOCINFO di;
    memset(&di, 0, sizeof(DOCINFO));
    di.cbSize = sizeof(DOCINFO);
    di.lpszDocName = L"Phieu_Lay_So"; // Name appearing in the Print Queue

    // ------------------------------------------------------------------------
    // TOGGLE OUTPUT MODE:
    // Set 'di.lpszOutput' to 'strFullPath' to print to a PDF file.
    // Set 'di.lpszOutput' to 'NULL' to print to the physical thermal printer.
    // ------------------------------------------------------------------------
    di.lpszOutput = strFullPath;
    // di.lpszOutput = NULL; // <-- Uncomment this line for Physical Printer
    // ------------------------------------------------------------------------

    if (dc.StartDoc(&di) > 0)
    {
        if (dc.StartPage() > 0)
        {
            // Initialize GDI+ Graphics context
            Gdiplus::Graphics g(dc.GetSafeHdc());

            // Set PageUnit to UnitDocument (1 unit = 1/300 inch) for high-resolution printing
            g.SetPageUnit(Gdiplus::UnitDocument);

            // Calculate actual page width based on printer DPI
            float dpiX = (float)dc.GetDeviceCaps(LOGPIXELSX);
            float pixelWidth = (float)dc.GetDeviceCaps(HORZRES);
            float pageWidth = (pixelWidth / dpiX) * 300.0f;

            // --- RENDERING PIPELINE ---
            // 'currentY' tracks the vertical cursor position. 
            // It is passed by reference to subsequent methods to ensure fluid layout.
            float currentY = 50.0f; // Initial Top Margin

            DrawHeader(g, pageWidth, currentY);
            DrawTicketInfo(g, pageWidth, currentY, strTicketNumber);
            DrawServiceInfo(g, pageWidth, currentY, strServiceTitle);
            DrawFooter(g, pageWidth, currentY);

            dc.EndPage();
        }
        dc.EndDoc();
    }

    // Cleanup resources
    dc.Detach();
    DeleteDC(hPrinterDC);
}

// ============================================================================
// RENDERING IMPLEMENTATIONS
// ============================================================================

void TicketPrinter::DrawHeader(Gdiplus::Graphics& g, float pageWidth, float& y)
{
    // Resources
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);
    Gdiplus::SolidBrush brushWhite(Gdiplus::Color::White);
    Gdiplus::Pen penDash(Gdiplus::Color::Black, 2.0f);
    penDash.SetDashStyle(Gdiplus::DashStyleDash);

    // Dimensions & Spacing
    float logoW = pageWidth * 0.7f;
    float logoH = 85.0f;
    float logoX = (pageWidth - logoW) / 2;
    float gapSmall = 40.0f;
    float gapMedium = 60.0f;

    // Draw Logo Background (Black Box)
    g.FillRectangle(&brushBlack, logoX, y, logoW, logoH);

    // Draw Bank Name (White text inside Black Box)
    Gdiplus::Font fontLogo(L"Arial", 22, Gdiplus::FontStyleBold);
    Gdiplus::RectF logoRect(logoX, y, logoW, logoH);
    Gdiplus::StringFormat logoFormat;
    logoFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
    logoFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    g.DrawString(BANK_NAME, -1, &fontLogo, logoRect, &logoFormat, &brushWhite);

    y += logoH + gapSmall; // Update cursor

    // Draw Branch Name
    Gdiplus::Font fontBranch(L"Arial", 11, Gdiplus::FontStyleRegular);
    DrawCenteredText(g, BRANCH_NAME, fontBranch, brushBlack, y, pageWidth);
    y += gapMedium;

    // Draw Separator Line
    g.DrawLine(&penDash, 30.0f, y, pageWidth - 30.0f, y);
    y += gapMedium;
}

void TicketPrinter::DrawTicketInfo(Gdiplus::Graphics& g, float pageWidth, float& y, CString strTicketNumber)
{
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);
    Gdiplus::Pen penBox(Gdiplus::Color::Black, 6.0f);

    float gapSmall = 40.0f;
    float gapLarge = 80.0f;

    // Draw Label
    Gdiplus::Font fontLabel(L"Arial", 15, Gdiplus::FontStyleBold);
    // Apply offset (y - 40) as per design requirement
    DrawCenteredText(g, L"SỐ THỨ TỰ CỦA BẠN", fontLabel, brushBlack, y - 40, pageWidth);
    y += gapSmall + 20.0f;

    // Draw Ticket Number Frame
    float numBoxH = 240.0f;
    float numBoxW = pageWidth * 0.5f; // Fixed width ratio
    float numBoxX = (pageWidth - numBoxW) / 2;

    g.DrawRectangle(&penBox, numBoxX, y, numBoxW, numBoxH);

    // Draw Ticket Number
    Gdiplus::Font fontNumber(L"Arial", 60, Gdiplus::FontStyleBold);

    // Apply offset (y + 20.0f) to vertically center-align visually
    Gdiplus::RectF textRect(numBoxX, y + 20.0f, numBoxW, numBoxH);
    Gdiplus::StringFormat centerFormat;
    centerFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
    centerFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);

    g.DrawString(strTicketNumber, -1, &fontNumber, textRect, &centerFormat, &brushBlack);

    y += numBoxH + gapLarge; // Large gap after the main number
}

void TicketPrinter::DrawServiceInfo(Gdiplus::Graphics& g, float pageWidth, float& y, CString strServiceTitle)
{
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);
    Gdiplus::Pen penDash(Gdiplus::Color::Black, 2.0f);
    penDash.SetDashStyle(Gdiplus::DashStyleDash);
    float gapMedium = 60.0f;

    // Draw Service Name
    Gdiplus::Font fontService(L"Arial", 17, Gdiplus::FontStyleBold);
    DrawCenteredText(g, strServiceTitle, fontService, brushBlack, y - 50, pageWidth);
    y += gapMedium;

    // Draw Separator Line
    g.DrawLine(&penDash, 30.0f, y, pageWidth - 30.0f, y);
    y += gapMedium;
}

void TicketPrinter::DrawFooter(Gdiplus::Graphics& g, float pageWidth, float& y)
{
    Gdiplus::SolidBrush brushBlack(Gdiplus::Color::Black);
    CTime now = CTime::GetCurrentTime();
    float gapMedium = 60.0f;
    float gapSmall = 40.0f;

    // Draw DateTime
    CString strTime = now.Format(L"Ngày: %d/%m/%Y - Giờ: %H:%M");
    Gdiplus::Font fontTime(L"Arial", 10, Gdiplus::FontStyleRegular);
    DrawCenteredText(g, strTime, fontTime, brushBlack, y - 50, pageWidth);
    y += gapMedium;

    // Draw Thank You Note
    Gdiplus::Font fontFooter(L"Arial", 10, Gdiplus::FontStyleItalic);
    DrawCenteredText(g, L"Vui lòng đợi đến lượt phục vụ.", fontFooter, brushBlack, y - 50, pageWidth);
    y += gapSmall;
    DrawCenteredText(g, L"Xin cảm ơn Quý khách!", fontFooter, brushBlack, y - 50, pageWidth);
}

// ============================================================================
// UTILITY HELPERS
// ============================================================================

CString TicketPrinter::GeneratePDFPath(CString strTicketNumber)
{
    TCHAR szMyDocsPath[MAX_PATH];
    // Retrieve "My Documents" path to avoid permission issues
    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, 0, szMyDocsPath)))
    {
        CString strFolder = CString(szMyDocsPath) + L"\\" + FOLDER_NAME;

        // Recursively create directory if it doesn't exist
        SHCreateDirectoryEx(NULL, strFolder, NULL);

        // Format filename: DATE_TIME_Ticket_NUMBER.pdf
        CTime now = CTime::GetCurrentTime();
        CString strFileName;
        strFileName.Format(L"%s_Phieu_so_%s", now.Format(L"%d%m%Y_%H%M%S"), strTicketNumber);

        CString strFullPath;
        strFullPath.Format(L"%s\\%s.pdf", strFolder, strFileName);

        // Output debug string to Visual Studio Output window
        TRACE(L"--> FILE SAVED AT: %s\n", strFullPath);
        return strFullPath;
    }
    return L"";
}

void TicketPrinter::DrawCenteredText(Gdiplus::Graphics& g, CString text, Gdiplus::Font& font, Gdiplus::Brush& brush, float y, float pageWidth)
{
    // Define a rectangle wide enough to hold the text
    Gdiplus::RectF rect(0, y, pageWidth, 150.0f);

    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);       // Center Horizontally
    format.SetLineAlignment(Gdiplus::StringAlignmentNear);     // Align to Top

    g.DrawString(text, -1, &font, rect, &format, &brush);
}
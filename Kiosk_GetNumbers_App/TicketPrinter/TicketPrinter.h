#pragma once
#include <afxwin.h>
#include <gdiplus.h>

/**
 * TicketPrinter
 * A utility class responsible for rendering and printing queue tickets.
 * This class uses GDI+ for drawing and supports both physical thermal printers
 * and PDF generation for debugging.
 */
class TicketPrinter
{
public:
    // Entry point for printing a ticket. 
    // This method is static to allow direct access without instantiation.
    static void Print(CString strTicketNumber, CString strServiceTitle);

private:
    // --- Rendering Helpers ---
    // These methods render specific sections of the ticket.
    // The 'float& y' parameter is passed by reference to automatically update 
    // the vertical cursor position after drawing each section.

    static void DrawHeader(Gdiplus::Graphics& g, float pageWidth, float& y);
    static void DrawTicketInfo(Gdiplus::Graphics& g, float pageWidth, float& y, CString strTicketNumber);
    static void DrawServiceInfo(Gdiplus::Graphics& g, float pageWidth, float& y, CString strServiceTitle);
    static void DrawFooter(Gdiplus::Graphics& g, float pageWidth, float& y);

    // --- Utility Helpers ---

    // Generates a unique file path for PDF output (used for testing).
    static CString GeneratePDFPath(CString strTicketNumber);

    // specialized helper to draw centered text within a specific region.
    static void DrawCenteredText(Gdiplus::Graphics& g, CString text, Gdiplus::Font& font, Gdiplus::Brush& brush, float y, float pageWidth);
};
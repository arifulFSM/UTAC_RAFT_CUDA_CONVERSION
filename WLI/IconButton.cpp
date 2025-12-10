#include "pch.h"
#include "IconButton.h"

// Include GDI+ headers
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIconButton::CIconButton()
{
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);

    // Initialize standard variables
    m_hIconCreated = NULL;

    // CMFCButton specific settings
    m_bTransparent = FALSE;
    m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    m_bTopImage = TRUE;
    m_bRightImage = FALSE;
    m_nAlignStyle = CMFCButton::ALIGN_CENTER;
    m_bDrawFocus = FALSE;

    // Default Colors
    m_clrBack = RGB(255, 255, 255);
    m_clrHover = RGB(229, 241, 251);
}

CIconButton::~CIconButton()
{
    FreeCustomIcon();
    GdiplusShutdown(m_gdiplusToken);
}

BEGIN_MESSAGE_MAP(CIconButton, CMFCButton)
END_MESSAGE_MAP()

void CIconButton::PreSubclassWindow()
{
    ModifyStyle(0, BS_OWNERDRAW);
    CMFCButton::PreSubclassWindow();
}

void CIconButton::FreeCustomIcon()
{
    if (m_hIconCreated != NULL)
    {
        ::DestroyIcon(m_hIconCreated);
        m_hIconCreated = NULL;
    }
}

void CIconButton::SetIconByID(UINT nIconID, int size)
{
    if (nIconID == 0) return;

    // 1. Clean up old icon
    FreeCustomIcon();

    // 2. Determine Target Size
    int targetSize = size;
    if (targetSize <= 0)
    {
        CRect rect;
        GetClientRect(rect);
        if (rect.Height() > 10)
            targetSize = rect.Height() - 8;
        else
            targetSize = 32;
    }

    // 3. Load the SOURCE icon at FULL RESOLUTION (128x128)
    int nSourceSize = 128;
    HICON hIconRaw = (HICON)::LoadImage(
        AfxGetInstanceHandle(),
        MAKEINTRESOURCE(nIconID),
        IMAGE_ICON,
        nSourceSize,
        nSourceSize,
        LR_DEFAULTCOLOR
    );

    if (!hIconRaw) return;

    // 4. Perform High-Quality Resizing using GDI+
    Bitmap* pBitmapSrc = Bitmap::FromHICON(hIconRaw);

    if (pBitmapSrc)
    {
        // --- FIX IS HERE: Use Stack Allocation instead of 'new' ---
        // This avoids the conflict with MFC's DEBUG_NEW macro
        Bitmap bitmapDest(targetSize, targetSize, PixelFormat32bppARGB);

        Graphics g(&bitmapDest); // Pass address of stack object

        // Settings for crispness
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.SetSmoothingMode(SmoothingModeHighQuality);
        g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

        // Draw
        g.DrawImage(pBitmapSrc, 0, 0, targetSize, targetSize);

        // 5. Convert back to HICON (Use dot syntax for stack object)
        bitmapDest.GetHICON(&m_hIconCreated);

        // Cleanup the Source bitmap (This is a pointer from GDI+, so we delete it)
        delete pBitmapSrc;

        // NOTE: We do NOT delete bitmapDest, it cleans itself up automatically!
    }

    ::DestroyIcon(hIconRaw);

    if (m_hIconCreated)
    {
        SetIcon(m_hIconCreated);
    }

    if (GetSafeHwnd()) Invalidate();
}

void CIconButton::SetHoverColor(COLORREF clrHover)
{
    m_clrHover = clrHover;
    if (GetSafeHwnd()) Invalidate();
}

void CIconButton::SetBackgroundColor(COLORREF clrBack)
{
    m_clrBack = clrBack;
    if (GetSafeHwnd()) Invalidate();
}

void CIconButton::OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState)
{
}

void CIconButton::OnFillBackground(CDC* pDC, const CRect& rectClient)
{
    if (IsPressed() || IsHighlighted())
    {
        pDC->FillSolidRect(rectClient, m_clrHover);
    }
    else
    {
        pDC->FillSolidRect(rectClient, m_clrBack);
    }
}
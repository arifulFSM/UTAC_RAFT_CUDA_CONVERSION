#include "pch.h"
#include "IconButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIconButton::CIconButton()
{
    // Initialize standard variables
    m_hIconCreated = NULL;

    // CMFCButton specific settings
    m_bTransparent = FALSE; // Must be FALSE to allow OnFillBackground to work
    m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
    m_bTopImage = TRUE;     // Icon above text (if text exists)
    m_bRightImage = FALSE;
    m_nAlignStyle = CMFCButton::ALIGN_CENTER;
    m_bDrawFocus = FALSE;   // Hide the dotted focus rectangle

    // Default Colors
    m_clrBack = RGB(255, 255, 255); // Standard Gray
    m_clrHover = RGB(229, 241, 251);          // Soft Blue
}

CIconButton::~CIconButton()
{
    // Clean up GDI resources
    FreeCustomIcon();
}

BEGIN_MESSAGE_MAP(CIconButton, CMFCButton)
END_MESSAGE_MAP()

void CIconButton::PreSubclassWindow()
{
    // Ensure we get mouse messages for hover effects
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
    int nWidth = size;
    int nHeight = size;
    if (nIconID == 0) return;

    // 1. Clean up any existing icon we loaded previously
    FreeCustomIcon();

    // 2. Determine desired size. 
    // If 0 is passed, use the system metric (usually 32x32 for large icons)
    int cx = (nWidth > 0) ? nWidth : ::GetSystemMetrics(SM_CXICON);
    int cy = (nHeight > 0) ? nHeight : ::GetSystemMetrics(SM_CYICON);

    // 3. Load the specific size using LoadImage.
    // This allows loading 48x48, 64x64, etc.
    m_hIconCreated = (HICON)::LoadImage(
        AfxGetInstanceHandle(),
        MAKEINTRESOURCE(nIconID),
        IMAGE_ICON,
        cx,
        cy,
        LR_DEFAULTCOLOR | LR_SHARED
    );

    if (m_hIconCreated)
    {
        // Pass the handle to the parent CMFCButton class
        SetIcon(m_hIconCreated);
    }

    // Force a repaint to show the new icon
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

// ---------------------------------------------------------
// Custom Drawing Overrides
// ---------------------------------------------------------

void CIconButton::OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState)
{
    // Intentionally empty implementation.
    // This prevents the default border from drawing.
   
    
}

void CIconButton::OnFillBackground(CDC* pDC, const CRect& rectClient)
{
    // Check if the button is Pressed or Highlighted (Hovered)
    if (IsPressed() || IsHighlighted())
    {
        pDC->FillSolidRect(rectClient, m_clrHover);
    }
    else
    {
        // Draw normal background
        pDC->FillSolidRect(rectClient, m_clrBack);
    }
}
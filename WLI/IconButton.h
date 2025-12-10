#pragma once
#include <afxbutton.h>

class CIconButton : public CMFCButton
{
public:
    CIconButton();
    virtual ~CIconButton();

    // Setup the icon from resource ID.
    // size: The target pixel size (e.g., 24, 32, 48).
    //       If 0 is passed, it fits automatically within the button height.
    void SetIconByID(UINT nIconID, int size);

    // Customization functions
    void SetHoverColor(COLORREF clrHover);
    void SetBackgroundColor(COLORREF clrBack);

protected:
    virtual void PreSubclassWindow();

    // Overrides to handle custom drawing
    virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
    virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);

    // Internal Clean up helper
    void FreeCustomIcon();

    // Member variables
    COLORREF m_clrHover;
    COLORREF m_clrBack;

    // GDI+ Token for initialization
    ULONG_PTR m_gdiplusToken;

    // We need to keep track of the icon handle to destroy it properly 
    HICON    m_hIconCreated;

    DECLARE_MESSAGE_MAP()
};
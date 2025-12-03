#pragma once
#include <afxbutton.h>

class CIconButton : public CMFCButton
{
public:
    CIconButton();
    virtual ~CIconButton();

    // Setup the icon from resource ID with specific Width and Height.
    // Pass 0,0 to use the system default size.
    void SetIconByID(UINT nIconID,int size);

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

    // We need to keep track of the icon handle to destroy it properly 
    // because we are loading it manually via LoadImage.
    HICON    m_hIconCreated;

    DECLARE_MESSAGE_MAP()
};
#include "pch.h"
#include "CheckBox.h"

CCheckBox::CCheckBox()
{
    m_bTransparent = FALSE;
    m_colorText = RGB(0, 0, 0);
    m_colorBack = RGB(255, 255, 255);
    m_brushBack.CreateSolidBrush(m_colorBack);
}

CCheckBox::~CCheckBox()
{
    m_brushBack.DeleteObject();
}

BEGIN_MESSAGE_MAP(CCheckBox, CButton)
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CCheckBox::CtlColor(CDC* pDC, UINT nCtlColor)
{
    pDC->SetTextColor(m_colorText);
    pDC->SetBkMode(TRANSPARENT);

    if (m_bTransparent)
    {
        return (HBRUSH)GetStockObject(NULL_BRUSH);
    }
    else
    {
        pDC->SetBkColor(m_colorBack);
        return (HBRUSH)m_brushBack;
    }
}

void CCheckBox::SetTransparent(BOOL bTransparent)
{
    m_bTransparent = bTransparent;
    Invalidate();
}

void CCheckBox::SetBackgroundColor(COLORREF color)
{
    m_colorBack = color;
    m_bTransparent = FALSE;
    m_brushBack.DeleteObject();
    m_brushBack.CreateSolidBrush(m_colorBack);
    Invalidate();
}

void CCheckBox::SetTextColor(COLORREF color)
{
    m_colorText = color;
    Invalidate();
}
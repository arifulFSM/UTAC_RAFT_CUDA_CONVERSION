#pragma once
#include "afxwin.h"

// Renamed from CCustomRadioButton to CCustomButton
class CCheckBox : public CButton
{
public:
    CCheckBox();
    virtual ~CCheckBox();

    void SetTransparent(BOOL bTransparent);
    void SetBackgroundColor(COLORREF color);
    void SetTextColor(COLORREF color);

protected:
    BOOL m_bTransparent;
    COLORREF m_colorBack;
    COLORREF m_colorText;
    CBrush m_brushBack;

    DECLARE_MESSAGE_MAP()
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
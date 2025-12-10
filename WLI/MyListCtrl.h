#pragma once
#include "MyHeaderCtrl.h" 

class CMyListCtrl : public CListCtrl
{
    CMyHeaderCtrl m_headerCtrl; // The custom header instance

protected:
    virtual void PreSubclassWindow();

public:
    DECLARE_MESSAGE_MAP()
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

    afx_msg BOOL OnEraseBkgnd(CDC* pDC); // <--- ADD THIS


    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);


};
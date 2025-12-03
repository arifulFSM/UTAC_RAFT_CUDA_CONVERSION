#pragma once
#include <afxbutton.h>

class CIconBorderButton : public CMFCButton
{
public:
    CIconBorderButton();
    virtual ~CIconBorderButton();

    // Setup the icon from resource ID with specific Width and Height.
    // Pass 0,0 to use the system default size.
    void SetIconByID(UINT nIconID, int size);

    void SetBitmapByID(UINT nBitmapID, int size);


protected:
    virtual void PreSubclassWindow();

   

    DECLARE_MESSAGE_MAP()
};
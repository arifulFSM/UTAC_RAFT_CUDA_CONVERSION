#include "pch.h"
#include "IconBorderButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CIconBorderButton::CIconBorderButton()
{
    // Initialize standard variables
 //   m_hIconCreated = NULL;

    // CMFCButton specific settings
 //   m_bTransparent = FALSE; // Must be FALSE to allow OnFillBackground to work
  //  m_nFlatStyle = CMFCButton::BUTTONSTYLE_FLAT;
  //  m_bTopImage = TRUE;     // Icon above text (if text exists)
  //  m_bRightImage = FALSE;
  //  m_nAlignStyle = CMFCButton::ALIGN_CENTER;
    m_bDrawFocus = FALSE;   // Hide the dotted focus rectangle

       // --- UPDATED COLORS TO MATCH IMAGE ---
  //  m_clrBack = RGB(255, 255, 255);       // Pure White Background
  //  m_clrHover = RGB(229, 241, 251);       // Very Light Gray for Hover
  //  m_clrBorder = RGB(200, 200, 200);     // Light Gray Border
 //   m_nCornerRadius = 10;                  // Rounded corner amount (X and Y)

}

CIconBorderButton::~CIconBorderButton()
{
  
}

BEGIN_MESSAGE_MAP(CIconBorderButton, CMFCButton)
END_MESSAGE_MAP()

void CIconBorderButton::PreSubclassWindow()
{
    // Ensure we get mouse messages for hover effects
    ModifyStyle(0, BS_OWNERDRAW);
    CMFCButton::PreSubclassWindow();
}



void CIconBorderButton::SetIconByID(UINT nIconID, int size)
{


    int nWidth = size;
    int nHeight = size;

    int cx =  ::GetSystemMetrics(SM_CXICON);
    int cy =  ::GetSystemMetrics(SM_CYICON);

	HICON hIcon = (HICON)::LoadImage(
		AfxGetInstanceHandle(),
		MAKEINTRESOURCE(nIconID),
		IMAGE_ICON,
		cx,        // width
		cy,        // height
		LR_DEFAULTCOLOR
	);


	if(hIcon) SetImage(hIcon);
}


void CIconBorderButton::SetBitmapByID(UINT nBitmapID, int size)
{

    int nWidth = size;
    int nHeight = size;

    int cx = (nWidth > 0) ? nWidth : ::GetSystemMetrics(SM_CXICON);
    int cy = (nHeight > 0) ? nHeight : ::GetSystemMetrics(SM_CYICON);

    HBITMAP hBmp = (HBITMAP)::LoadImage(
        AfxGetInstanceHandle(),
        MAKEINTRESOURCE(nBitmapID),
        IMAGE_BITMAP,
        cx,            // desired width
        cy,           // desired height
        LR_CREATEDIBSECTION | LR_DEFAULTCOLOR
    );

    if (hBmp)
    {
        ModifyStyle(0, BS_BITMAP);   // important for bitmap buttons
        SetImage(hBmp);
    }
}


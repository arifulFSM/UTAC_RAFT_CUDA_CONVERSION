#pragma once
// EditPoint.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CEditPoint dialog

class CEditPoint : public CDialog {
	// Construction
public:
	float OldY;
	float OldX;
	CEditPoint(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditPoint)
	enum { IDD = IDD_EDITMPOINT };
	float	m_XCoor;
	float	m_YCoor;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditPoint)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditPoint)
	virtual BOOL OnInitDialog();
	//afx_msg void OnUsecurrent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
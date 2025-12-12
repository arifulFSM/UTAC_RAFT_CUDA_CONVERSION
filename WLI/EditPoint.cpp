// EditPoint.cpp : implementation file
//

#include "pch.h"
//#include "e95.h"
#include "EditPoint.h"
#include "RAFTApp.h"
//#include "..\413\MotorS2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditPoint dialog

CEditPoint::CEditPoint(CWnd* pParent /*=NULL*/)
	: CDialog(CEditPoint::IDD, pParent) {
	//{{AFX_DATA_INIT(CEditPoint)
	m_XCoor = 0.0f;
	m_YCoor = 0.0f;
	//}}AFX_DATA_INIT
}

void CEditPoint::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditPoint)
	DDX_Text(pDX, IDC_XCOOR, m_XCoor);
	DDX_Text(pDX, IDC_YCOOR, m_YCoor);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CEditPoint, CDialog)
	//{{AFX_MSG_MAP(CEditPoint)
	//ON_BN_CLICKED(IDC_USECURRENT, OnUsecurrent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditPoint message handlers

BOOL CEditPoint::OnInitDialog() {
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

//void CEditPoint::OnUsecurrent() {
//	p413App->pMtr->GetXYPos(&m_XCoor, &m_YCoor);
//	UpdateData(FALSE);
//}
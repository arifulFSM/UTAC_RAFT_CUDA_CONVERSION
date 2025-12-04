// AnalysisNewDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "AnalysisNewDlg.h"
#include "afxdialogex.h"


// CAnalysisNewDlg dialog

IMPLEMENT_DYNAMIC(CAnalysisNewDlg, CResizableDialog)

CAnalysisNewDlg::CAnalysisNewDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_ANALYSIS_DLG_NEW, pParent)
{

}

CAnalysisNewDlg::~CAnalysisNewDlg()
{
}

void CAnalysisNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON1, m_toolButton1);
	DDX_Control(pDX, IDC_BUTTON2, m_toolButton2);
	DDX_Control(pDX, IDC_BUTTON3, m_toolButton3);
	DDX_Control(pDX, IDC_BUTTON4, m_toolButton4);
	DDX_Control(pDX, IDC_BUTTON8, m_toolButton5);
	DDX_Control(pDX, IDC_BUTTON5, m_toolButton6);
	DDX_Control(pDX, IDC_BUTTON6, m_toolButton7);
	DDX_Control(pDX, IDC_BUTTON7, m_toolButton8);
}


BEGIN_MESSAGE_MAP(CAnalysisNewDlg, CResizableDialog)
	ON_COMMAND_EX(IDC_BUTTON1, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON2, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON3, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON4, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON5, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON6, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON7, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON8, OnToolButtonClicked)
END_MESSAGE_MAP()


// CAnalysisNewDlg message handlers


// Adjust these RGB values to match your UI design
const COLORREF CLR_NORMAL_BACK = RGB(240, 240, 240); // Standard Grey/White
const COLORREF CLR_NORMAL_HOVER = RGB(220, 220, 220); // Slightly darker when hovering
const COLORREF CLR_SELECTED = RGB(173, 216, 230); // Light Blue for Active Tool

BOOL CAnalysisNewDlg::OnToolButtonClicked(UINT nID)
{
	// 1. NON-CLICKABLE LOGIC:
	// If clicking the button that is ALREADY selected, ignore it.
	if (nID == m_nSelectedToolID)
		return TRUE;

	// 2. RESET THE OLD BUTTON (Deselect)
	// We cast GetDlgItem to your custom class to access your custom functions
	if (m_nSelectedToolID != 0)
	{
		CIconButton* pOldBtn = (CIconButton*)GetDlgItem(m_nSelectedToolID);
		if (pOldBtn)
		{
			// Restore normal background
			pOldBtn->SetBackgroundColor(CLR_NORMAL_BACK);
			// Restore normal hover effect
			pOldBtn->SetHoverColor(CLR_NORMAL_HOVER);
		}
	}

	// 3. SETUP THE NEW BUTTON (Select)
	CIconButton* pNewBtn = (CIconButton*)GetDlgItem(nID);
	if (pNewBtn)
	{
		// Set the "Selected" background
		pNewBtn->SetBackgroundColor(CLR_SELECTED);

		// DISABLE HOVER:
		// Set hover color same as background so it looks like hover is disabled
		pNewBtn->SetHoverColor(CLR_SELECTED);
		//AfxMessageBox(_T("Tool Selected"));
	}

	// 4. Update the tracking variable
	m_nSelectedToolID = nID;

	// 5. Handle Tool Logic
	// switch(nID) { ... }

	return TRUE; // Stop further processing
}
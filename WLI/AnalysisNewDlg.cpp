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
	DDX_Control(pDX, IDC_BUTTON_2_POINTS, m_toolButton1);
	DDX_Control(pDX, IDC_BUTTON_HORIZONTAL, m_toolButton2);
	DDX_Control(pDX, IDC_BUTTON_VERTICAL, m_toolButton3);
	DDX_Control(pDX, IDC_BUTTON_LINE, m_toolButton4);
	DDX_Control(pDX, IDC_BUTTON_PARALLEL, m_toolButton5);
	DDX_Control(pDX, IDC_BUTTON_PERPENDICULAR, m_toolButton6);
	DDX_Control(pDX, IDC_BUTTON_BOX, m_toolButton7);
	DDX_Control(pDX, IDC_BUTTON_CIRCLE, m_toolButton8);
	DDX_Control(pDX, IDC_PROFILE_CARD_PANEL, m_profileToolCardPanel);
	DDX_Control(pDX, IDC_LINE_ROUGHNESS_CARD_PANEL, m_lineRoughnessCardPanel);
	DDX_Control(pDX, IDC_MEASUREMENT_TYPE_TEXT, m_measureTypeText);
	DDX_Control(pDX, IDC_RADIO1, m_MeasurementTypeRadio1);
	DDX_Control(pDX, IDC_RADIO2, m_MeasurementTypeRadio2);
	DDX_Control(pDX, IDC_RADIO3, m_MeasurementTypeRadio3);
	DDX_Control(pDX, IDC_CUTOFF_FILTERS, m_cutoffFilterTitle);
	DDX_Control(pDX, IDC_LAMDA_LOW, m_lamdaLow);
	DDX_Control(pDX, IDC_LAMDA_HIGH, m_lamdaHigh);
	DDX_Control(pDX, IDC_MICRO_METER, m_microMeter);
	DDX_Control(pDX, IDC_MICROMETER2, m_microMeter2);
}


BEGIN_MESSAGE_MAP(CAnalysisNewDlg, CResizableDialog)
	ON_COMMAND_EX(IDC_BUTTON_2_POINTS, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_HORIZONTAL, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_VERTICAL, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_LINE, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_PARALLEL, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_PERPENDICULAR, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_BOX, OnToolButtonClicked)
	ON_COMMAND_EX(IDC_BUTTON_CIRCLE, OnToolButtonClicked)
	ON_STN_CLICKED(IDC_PROFILE_CARD_PANEL, &CAnalysisNewDlg::OnStnClickedProfileCardPanel)
END_MESSAGE_MAP()


// CAnalysisNewDlg message handlers

BOOL CAnalysisNewDlg::OnInitDialog() {
	__super::OnInitDialog();

	m_profileToolCardPanel.SetTitle(_T("Profile Tool"));
	m_lineRoughnessCardPanel.SetTitle(_T("Line Roughness Setting"));
	
	OnToolButtonClicked(IDC_BUTTON1);

	m_toolButton1.SetIconByID(IDI_ICON29, 28);
	m_toolButton2.SetIconByID(IDI_ICON29, 28);
	m_toolButton3.SetIconByID(IDI_ICON29, 28);
	m_toolButton4.SetIconByID(IDI_ICON29, 28);
	m_toolButton5.SetIconByID(IDI_ICON29, 28);
	m_toolButton6.SetIconByID(IDI_ICON29, 28);
	m_toolButton7.SetIconByID(IDI_ICON29, 28);
	m_toolButton8.SetIconByID(IDI_ICON29, 28);



	return TRUE;
}

// Adjust these RGB values to match your UI design
const COLORREF CLR_NORMAL_BACK = RGB(255, 255, 255); // Standard Grey/White
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
	switch (nID)
	{
	case IDC_BUTTON_2_POINTS:
		//AfxMessageBox(_T("button 1 clicked"));
		break;

	case IDC_BUTTON_HORIZONTAL:
		//AfxMessageBox(_T("button 2 clicked"));
		break;

	case IDC_BUTTON_VERTICAL:
		//AfxMessageBox(_T("button 3 clicked"));
		break;

	case IDC_BUTTON_LINE:
		//AfxMessageBox(_T("button 4 clicked"));
		break;

	case IDC_BUTTON_PARALLEL:
		//AfxMessageBox(_T("button 5 clicked"));
		break;

	case IDC_BUTTON_PERPENDICULAR:
		//AfxMessageBox(_T("button 6 clicked"));
		break;

	case IDC_BUTTON_BOX:
		//AfxMessageBox(_T("button 7 clicked"));
		break;

	case IDC_BUTTON_CIRCLE:
		//AfxMessageBox(_T("button 8 clicked"));
		break;

	default:
		// Optional: Handle if an unknown ID gets here
		break;
	}


	// 5. Handle Tool Logic
	// switch(nID) { ... }

	return TRUE; // Stop further processing
}


void CAnalysisNewDlg::OnStnClickedProfileCardPanel()
{
	// TODO: Add your control notification handler code here
}

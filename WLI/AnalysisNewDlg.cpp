// AnalysisNewDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "AnalysisNewDlg.h"
#include "afxdialogex.h"


//20250112 - Mahmudul Haque -------- Analysis Tab New UI Design


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
	DDX_Control(pDX, IDC_BUTTON_2_POINTS, m_toolButton2Points);
	DDX_Control(pDX, IDC_BUTTON_HORIZONTAL, m_toolButtonHorizontalLine);
	DDX_Control(pDX, IDC_BUTTON_VERTICAL, m_toolButtonVerticalLine);
	DDX_Control(pDX, IDC_BUTTON_LINE, m_toolButtonLine);
	DDX_Control(pDX, IDC_BUTTON_PARALLEL, m_toolButtonParallel);
	DDX_Control(pDX, IDC_BUTTON_PERPENDICULAR, m_toolButtonPerpendicular);
	DDX_Control(pDX, IDC_BUTTON_BOX, m_toolButtonBox);
	DDX_Control(pDX, IDC_BUTTON_CIRCLE, m_toolButtonCircle);
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
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	DDX_Control(pDX, IDC_CHECK_END_EFFECT_CORRECTION, m_checkEffectCorrection);
	DDX_Control(pDX, IDC_CHECK_DOUBLE_GAUSSIAN_FILTER, m_checkDoubleGaussianFilter);
	DDX_Control(pDX, IDC_2D_VIEW_PANEL, m_2dViewPanel);
	DDX_Control(pDX, IDC_3D_VIEW_PANEL, m_3dViewPanel);
	DDX_Control(pDX, IDC_GRAPH_1, m_profileGraph);
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
	
	OnToolButtonClicked(IDC_BUTTON_2_POINTS);


	setButtonIcon(35);

	PopulateList();


	return TRUE;
}


void CAnalysisNewDlg::setButtonIcon(int size) {

	m_toolButton2Points.SetIconByID(IDI_ICON30, size);
	m_toolButtonHorizontalLine.SetIconByID(IDI_ICON33, size);
	m_toolButtonVerticalLine.SetIconByID(IDI_ICON37, size);
	m_toolButtonLine.SetIconByID(IDI_ICON34, size);
	m_toolButtonParallel.SetIconByID(IDI_ICON35, size);
	m_toolButtonPerpendicular.SetIconByID(IDI_ICON36, size);
	m_toolButtonBox.SetIconByID(IDI_ICON31, size);
	m_toolButtonCircle.SetIconByID(IDI_ICON32, size);

}




void CAnalysisNewDlg::PopulateList()
{

	m_listCtrl.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	// Now you can set extended styles (Grid lines, etc.)
	m_listCtrl.SetExtendedStyle(m_listCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	//m_table_ctrl.DeleteAllItems(); // Clear old data if refreshing
	m_listCtrl.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 30);    // Column 0
	m_listCtrl.InsertColumn(1, _T("Rq"), LVCFMT_LEFT, 70);  // Column 1
	m_listCtrl.InsertColumn(2, _T("Ra"), LVCFMT_LEFT, 70);   // Column 2
	m_listCtrl.InsertColumn(3, _T("Ry"), LVCFMT_LEFT, 70); // Column 3
	// --- SIMULATED DATABASE DATA ---
	struct Employee {
		CString id;
		CString Rq;
		CString Ra;
		CString Ry;
	};

	Employee data[] = {
		{ _T("1"), _T("0.25"), _T("0.25"), _T("0.25") },
		{ _T("2"),_T("0.25"), _T("0.25"), _T("0.25") },
		{ _T("3"),_T("0.25"), _T("0.25"), _T("0.25") },
		{ _T("4"), _T("0.25"), _T("0.25"), _T("0.25") },
		{ _T("5"),_T("0.25"), _T("0.25"), _T("0.25") },
		{ _T("6"),_T("0.25"), _T("0.25"), _T("0.25") }
	};
	// -------------------------------

	// Loop through your data
	for (int i = 0; i < 6; i++)
	{
		// 1. Insert the Row (and set Column 0 text)
		// InsertItem returns the index of the new row
		int nIndex = m_listCtrl.InsertItem(i, data[i].id);

		// 2. Set text for specific columns in that row
		m_listCtrl.SetItemText(nIndex, 1, data[i].Rq);   // Column 1
		m_listCtrl.SetItemText(nIndex, 2, data[i].Ra);   // Column 2
		m_listCtrl.SetItemText(nIndex, 3, data[i].Ry); // Column 3

		// Optional: Set row data (useful for retrieving DB ID later)
		// m_ListCtrl.SetItemData(nIndex, (DWORD_PTR)real_database_id);
	}
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



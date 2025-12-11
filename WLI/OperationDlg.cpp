// OperationDlg.cpp : implementation file
//
//20250112 - Mahmudul Haque



#include "pch.h"
#include "WLI.h"
#include "OperationDlg.h"
#include "afxdialogex.h"


// COperationDlg dialog

IMPLEMENT_DYNAMIC(COperationDlg, CResizableDialog)

COperationDlg::COperationDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_OPERATION_DLG, pParent)
{

}

COperationDlg::~COperationDlg()
{
}

void COperationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STAGE_MOTION_PANEL, m_stageMotionPanelCard);
	DDX_Control(pDX, IDC_BUTTON_TOP_LEFT, topLeftButton);
	DDX_Control(pDX, IDC_BUTTON_UP, upButton);
	DDX_Control(pDX, IDC_BUTTON_TOP_RIGHT, topRightButton);
	DDX_Control(pDX, IDC_BUTTON_LEFT, leftButton);
	DDX_Control(pDX, IDC_BUTTON_HOME, homeButton);
	DDX_Control(pDX, IDC_BUTTON_RIGHT, rightButton);
	DDX_Control(pDX, IDC_BUTTON_BOTTOM_LEFT, bottomLeftButton);
	DDX_Control(pDX, IDC_BUTTON_DOWN, downButton);
	DDX_Control(pDX, IDC_BUTTON_BOTTOM_RIGHT, bottomRightButton);
	DDX_Control(pDX, IDC_BUTTON_Z_UP, zUpButton);
	DDX_Control(pDX, IDC_BUTTON_Z_DOWN, zDownButton);
	DDX_Control(pDX, IDC_AUTOFOCUS_CARD, autoFocusCardPanel);
	DDX_Control(pDX, IDC_MEASUREMENT_EXECUTION_PANEL, measurementExecutionPanel);
	DDX_Control(pDX, IDC_BUTTON_MEASURE, m_measureButton);
	DDX_Control(pDX, IDC_BUTTON_STOP, m_stopMeasureButton);
	DDX_Control(pDX, IDC_LOADED_WAFER_RECIPE_FILE_NAME, m_loadedWaferRecipeName);
	DDX_Control(pDX, IDC_CURRENT_Z, m_currentZ);
	DDX_Control(pDX, IDC_LOW_Z, m_lowZ);
	DDX_Control(pDX, IDC_HIGH_Z, m_highZ);
	DDX_Control(pDX, IDC_CURRENT_Z_VALUE, m_currentZValue);
	DDX_Control(pDX, IDC_LOW_Z_VALUE, m_lowZValue);
	DDX_Control(pDX, IDC_HIGH_Z_VALUE, m_highZValue);
	DDX_Control(pDX, IDC_BUTTON_RUN_AF, m_runAFButton);
	DDX_Control(pDX, IDC_ACTIVE_RECIPE_TITLE, m_activeRecipe);
	DDX_Control(pDX, IDC_SPEED, m_speedTitle);
	DDX_Control(pDX, IDC_LENS_SELECTION, m_lensCtrl);
	DDX_Control(pDX, IDC_LENS_CARD_PANEL, m_lensCardPanel);
	DDX_Control(pDX, IDC_BRIGHTNESS, m_brightness);
	DDX_Control(pDX, IDC_EXPOSURE, m_exposure);
	DDX_Control(pDX, IDC_PROGRESS_PERCENTAGE, m_progress_percentage);
	DDX_Control(pDX, IDC_SLIDER_Z_AXIS_CONTROL, m_zValueSlider);
	DDX_Control(pDX, IDC_SLIDER_BRIGHTNESS, m_brightnessSlider);
	DDX_Control(pDX, IDC_SLIDER_EXPOSURE, m_exposureSlider);
	DDX_Control(pDX, IDC_RADIO_FINE, m_FineRadioButton);
	DDX_Control(pDX, IDC_RADIO_FAST, m_fastRadioButton);
}


BEGIN_MESSAGE_MAP(COperationDlg, CResizableDialog)
	//ON_WM_CTLCOLOR()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_PROGRESS1, &COperationDlg::OnNMCustomdrawProgress1)
	ON_STN_CLICKED(IDC_LENS_CARD_PANEL, &COperationDlg::OnStnClickedLensCardPanel)
END_MESSAGE_MAP()


// COperationDlg message handlers
BOOL COperationDlg::OnInitDialog() {
	__super::OnInitDialog();



	//m_brushBack.CreateSolidBrush(RGB(244, 247, 249)); //235, 236, 237 dialog background color 

	m_stageMotionPanelCard.SetTitle(_T("Stage Motion"));
	autoFocusCardPanel.SetTitle(_T("Auto Focus"));
	measurementExecutionPanel.SetTitle(_T("Measurement Execution"));
	m_lensCardPanel.SetTitle(_T("Observation Settings"));


	m_lensCtrl.AddLens(_T("2x"));
	m_lensCtrl.AddLens(_T("5x"));
	m_lensCtrl.AddLens(_T("10x"));
	m_lensCtrl.AddLens(_T("20x"));
	m_lensCtrl.AddLens(_T("50x"));
	//m_lensCtrl.AddLens(_T("1000x"));




	m_measureButton.SetColor(RGB(16, 124, 16), RGB(50, 205, 50), RGB(255, 255, 255));
	m_measureButton.SetTextProperties(10, true, _T("Arial"));

	m_stopMeasureButton.SetColor(RGB(209, 52, 56), RGB(150, 30, 30), RGB(255, 255, 255));
	m_stopMeasureButton.SetTextProperties(10, true, _T("Arial"));

	m_runAFButton.SetColor(RGB(248, 248, 248), RGB(230, 230, 230), RGB(9, 129, 217));
	m_runAFButton.SetTextProperties(10, true, _T("Arial"));

	
	setButtonIcon(20);



	 m_currentZ.SetFontOptions(10,false);
	 m_lowZ.SetFontOptions(10, false);
	 m_highZ.SetFontOptions(10, false);
	 m_activeRecipe.SetFontOptions(10, false);

	 m_brightness.SetFontOptions(10, false);
	 m_exposure.SetFontOptions(10, false);



	return TRUE;
}




void COperationDlg::setButtonIcon(int size) {
	//stage motion
	topLeftButton.SetIconByID(IDI_ICON19, size);
	topRightButton.SetIconByID(IDI_ICON20, size);
	bottomLeftButton.SetIconByID(IDI_ICON17, size);
	bottomRightButton.SetIconByID(IDI_ICON18, size);
	upButton.SetIconByID(IDI_ICON12, size);
	downButton.SetIconByID(IDI_ICON8, size);
	leftButton.SetIconByID(IDI_ICON10, size);
	rightButton.SetIconByID(IDI_ICON11, size);


	homeButton.SetIconByID(IDI_ICON24, size+20);


	zUpButton.SetIconByID(IDI_ICON28,size+10);
	zDownButton.SetIconByID(IDI_ICON27,size+10);




}





//HBRUSH COperationDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	if (nCtlColor == CTLCOLOR_DLG)
//	{
//		return m_brushBack;
//	}
//	return CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
//}



void COperationDlg::OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}


void COperationDlg::OnStnClickedLensCardPanel()
{
	// TODO: Add your control notification handler code here
}

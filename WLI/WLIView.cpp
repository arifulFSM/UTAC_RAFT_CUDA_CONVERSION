#include "pch.h"
#include "framework.h"

// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "WLI.h"
#endif

#include "AcqDlg.h"
#include "wdefine.h"
#include "StripDlg.h"
#include "HeightDlg.h"
#include "HeightPlot.h" // 05302023 - Mortuja
#include "MeasurementDlg.h"
#include "AnalysisDlg.h"

#include "WLIDoc.h"
#include "WLIView.h"
#include "BaslerCameraDlg.h" //12222022
#include "RAFTApp.h"
#include "ResultDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif




CMoCtrl* CWLIView::pMCtr = nullptr;
CMotionControlDlg* CWLIView::pMSet = nullptr;
CCameraDlg* CWLIView::pCam1 = nullptr;
CCameraDlg* CWLIView::pCam2 = nullptr;
CWLIView* pWLIView = nullptr;
//BaslerCameraDlg* CWLIView::pBaslerCam = nullptr; //12222022

IMPLEMENT_DYNCREATE(CWLIView, CResizableFormView)

BEGIN_MESSAGE_MAP(CWLIView, CResizableFormView)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR() //BACKGROUND COLOR //20250112 - Mahmudul Haque
	ON_WM_TIMER()

	ON_COMMAND(ID_WLI_CAMERA, &CWLIView::OnWliCamera1)
	ON_COMMAND(ID_WLI_MOTIONCONTROLLER, &CWLIView::OnWliMotioncontroller)
	ON_COMMAND(ID_WLI_ACQUIRE, &CWLIView::OnWliAcquire)
	ON_COMMAND(ID_WLI_MOTORSETUP, &CWLIView::OnWliMotorsetup)
	ON_MESSAGE(UM_STRIP_LOADED, &CWLIView::OnUmStripLoaded)
	ON_COMMAND(ID_CAMERA2, &CWLIView::OnWliCamera2)
	ON_MESSAGE(UM_HEIGHT_CALCED, &CWLIView::OnUmHeightCalced)
	ON_COMMAND(ID_BASLERCAMERA, &CWLIView::OnBaslerCamera)
	ON_MESSAGE(IDC_HEIGHT_MAP_PM, &CWLIView::OnUmHeightCalc) // 05302023 - Mortuja
	ON_MESSAGE(IDC_ROUGHNESS_DLG, &CWLIView::OnRoughnessDlg)
	ON_COMMAND(ID_RECIPE_CREATERECIPE, &CWLIView::OnRecipeCreaterecipe)
	ON_MESSAGE(UM_RESULT_DLG, &CWLIView::OnUmResultDlg)
	ON_MESSAGE(UM_ANALYSIS_DLG, &CWLIView::OnUmAnalysisDlg)

	ON_MESSAGE(IDC_ADDPOINT, OnAddPoint)
	ON_MESSAGE(IDC_ADDALGNPOINT, OnAddalgnpoint)
	ON_MESSAGE(IDC_DELETEPOINT, OnDeletepoint)
END_MESSAGE_MAP()

CWLIView::CWLIView() noexcept
	: CResizableFormView(IDD_WLI_FORM) {
	pWLIView = this;
}

CWLIView::~CWLIView() {
	pWLIView = nullptr;
	if (pRAFTApp != NULL) {
		delete pRAFTApp;
	}
}

void CWLIView::DoDataExchange(CDataExchange* pDX) {
	CResizableFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, cTab);
	DDX_Control(pDX, IDC_BUTTON_LOAD, loadButton);
	DDX_Control(pDX, IDC_BUTTON_SAVE, saveButton);
	DDX_Control(pDX, IDC_BUTTON_SETTING, settingButton);
	DDX_Control(pDX, IDC_BUTTON_LOGIN, loginButton);
	DDX_Control(pDX, IDC_SIGNAL_TOWER, m_signalTower);
	DDX_Control(pDX, IDC_BUTTON_MOTION, cameraMotionButton);
	DDX_Control(pDX, IDC_BUTTON_LOAD_DATA, loadDataButton);
	DDX_Control(pDX, IDC_POSITION, m_cameraPosition);
	DDX_Control(pDX, IDC_X_TITLE, m_xTitle);
	DDX_Control(pDX, IDC_Y_TITLE, m_yTitle);
	DDX_Control(pDX, IDC_Z_TITLE, m_zTitle);
	DDX_Control(pDX, IDC_CAMERA_X_VALUE, m_cameraXValue);
	DDX_Control(pDX, IDC_CAMERA_Y_VALUE, m_cameraYValue);
	DDX_Control(pDX, IDC_CAMERA_Z_VALUE, m_cameraZValue);
	DDX_Control(pDX, IDC_WAFERMAP, m_cWaferMap);
	DDX_Control(pDX, IDC_CAMERA, m_cLiveVid);
}

CWLIView* CWLIView::GetView() {
	CFrameWnd* pFrame = (CFrameWnd*)(AfxGetApp()->m_pMainWnd);
	CView* pView = pFrame->GetActiveView();
	if (!pView) return nullptr;
	if (!pView->IsKindOf(RUNTIME_CLASS(CWLIView))) return nullptr;
	return (CWLIView*)pView;
}

BOOL CWLIView::PreCreateWindow(CREATESTRUCT& cs) {
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CResizableFormView::PreCreateWindow(cs);
}

void CWLIView::OnInitialUpdate() {
	AddResizedControl(IDC_TAB1, TOP_LEFT, BOTTOM_RIGHT);
	CResizableFormView::OnInitialUpdate();
	//GetParentFrame()->RecalcLayout();
	//ResizeParentToFit();

	short nTab = 0;
	pRAFTApp = new CRAFTApp;

	pRcp = &pRAFTApp->RcpSetup;
	m_cWaferMap.pRcp = pRcp;
	m_cWaferMap.bSiteView = FALSE; // Show recipe points [6/25/2010 Yuen]
	m_cWaferMap.pParent = this;
	m_cWaferMap.Redraw();

	m_cPoint = nullptr;


	rcpDlg = new RecipeDlg;
	if (rcpDlg) {
		rcpDlg->Create(IDD_RCP_DLG, &cTab);
		cTab.AddTab(rcpDlg, CString("Recipe").GetBuffer(), nTab++);
	}
	measDlg = new MeasurementDlg;
	if (measDlg) {
		measDlg->hWndParent = GetSafeHwnd();
		measDlg->Create(IDD_MEASUREMENT_DLG, &cTab);
		cTab.AddTab(measDlg, CString("Measurement").GetBuffer(), nTab++);
	}
	rsltDlg = new ResultDlg;
	if (rsltDlg) {
		rsltDlg->Create(IDD_DLG_RESULT, &cTab);
		cTab.AddTab(rsltDlg, CString("Result").GetBuffer(), nTab++);
	}
	analysisDlg = new AnalysisDlg;
	if (analysisDlg) {
		analysisDlg->Create(IDD_ANALYSIS_DLG, &cTab);
		cTab.AddTab(analysisDlg, CString("Analysis").GetBuffer(), nTab++);
	}
	pStrip = new CStripDlg;
	if (pStrip) {
		pStrip->hWndParent = GetSafeHwnd();
		pStrip->Create(IDD_DIALOG8, &cTab);
		cTab.AddTab(pStrip, CString("Interferogram").GetBuffer(), nTab++); // Enable Morsalin
	}
	pHeight = new CHeightDlg;
	if (pHeight) {
		pHeight->Create(IDD_DIALOG4, &cTab);
		//cTab.AddTab(pHeight, CString("Height").GetBuffer(), nTab++);
	}
	//20250112 - Mahmudul Haque
	operationDlg = new COperationDlg();
	if (operationDlg) {
		operationDlg->Create(IDD_OPERATION_DLG, &cTab);
		cTab.AddTab(operationDlg, CString("Operation").GetBuffer(), nTab++);
	}
	//20250112 - Mahmudul Haque 
	analysisNewDlg = new CAnalysisNewDlg();
	if (analysisNewDlg) {
		analysisNewDlg->Create(IDD_ANALYSIS_DLG_NEW, &cTab);
		cTab.AddTab(analysisNewDlg, CString("Analysis").GetBuffer(), nTab++);
	}


	//cTab.SetCurSel(5);





	//20250112 - Mahmudul Haque -------start ---------
	AddResizedControl(IDC_FSM_LOGO, TOP_LEFT, TOP_LEFT);
	
	AddResizedControl(IDC_SIGNAL_TOWER, TOP_RIGHT, TOP_RIGHT);
	AddResizedControl(IDC_BUTTON_LOGIN, TOP_RIGHT, TOP_RIGHT);


	//AddResizedControl(IDC_CAMERA, CSize(0, 80), CSize(0, 80));
	AddResizedControl(IDC_WAFERMAP, CSize(0, 95), CSize(0, 95));

	//AddResizedControl(IDC_BUTTON_LOAD, CSize(65, 0), CSize(65, 0));
	//AddResizedControl(IDC_BUTTON_SAVE, CSize(65, 0), CSize(65, 0));
	//AddResizedControl(IDC_BUTTON_SETTING, CSize(65, 0), CSize(65, 0));
	//AddResizedControl(IDC_BUTTON_MOTION, CSize(65, 0), CSize(65, 0));


	m_brushBack.CreateSolidBrush(RGB(255, 255, 255)); //235, 236, 237 setting dialog background color

	setButtonIcon(48);

	camRun();//20251205

	UpdateTimeLabel();       // Show time immediately
	SetTimer(1, 1000, NULL);
	//20250112 ------------end ------------------

	AnchorControls();



}

#ifdef _DEBUG
void CWLIView::AssertValid() const {
	CResizableFormView::AssertValid();
}

void CWLIView::Dump(CDumpContext& dc) const {
	CResizableFormView::Dump(dc);
}

CWLIDoc* CWLIView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWLIDoc)));
	return (CWLIDoc*)m_pDocument;
}
#endif //_DEBUG

void CWLIView::OnDestroy() {
	CResizableFormView::OnDestroy();

	if (pCam1) delete pCam1;
	if (pCam2) delete pCam2;
	if (pMCtr) delete pMCtr;
	if (pMSet) delete pMSet;
	if (pStrip) delete pStrip;
	if (pHeight) delete pHeight;
	if (pBaslerCam) delete pBaslerCam;
	if (heightPlotDlg) delete heightPlotDlg;
	if (rcpDlg) delete rcpDlg; //07252023
	if (measDlg) delete measDlg;
	if (analysisDlg) delete analysisDlg;
	if (roughDlg) delete roughDlg;
	if (rsltDlg) delete rsltDlg;
}

void CWLIView::HideMoCtrl() {
	if (pMSet) pMSet->ShowWindow(SW_HIDE);
}

void CWLIView::HideMoSetup() {
	if (pMCtr) {
		pMCtr->ShowWindow(SW_HIDE);
	}
}

void CWLIView::ShowMoCtrl() {
	if (!pMCtr) {
		pMCtr = new CMoCtrl;
		if (pMCtr) pMCtr->Create(IDD_DIALOG1);
	}
	if (pMCtr) {
		if (pMCtr->IsWindowVisible()) return;
		pMCtr->ShowWindow(SW_SHOW);
	}
}

void CWLIView::ShowMoSetup(HWND hParent) {
	if (!pMSet) {
		pMSet = new CMotionControlDlg;
		if (pMSet) pMSet->Create(IDD_DIALOG10);
		return;
	}
	if (pMSet) {
		pMSet->SetHParent(hParent);
		if (pMSet->IsWindowVisible()) return;
		pMSet->ShowWindow(SW_SHOW);
	}
}

void CWLIView::OnWliMotioncontroller() {
	ShowMoCtrl();
}

void CWLIView::OnWliMotorsetup() {
	ShowMoSetup(0);
}

void CWLIView::OnWliAcquire() {
	CAcqDlg dlg;
	if (dlg.DoModal() == IDOK) {
		Refresh();
	}
	/*if (!acqDlg) {
		acqDlg = new CAcqDlg;
		if (acqDlg) {
			acqDlg->Create(IDD_DIALOG3);
			acqDlg->SetWindowTextW(L"Acquisition Dialog");
			acqDlg->ShowWindow(SW_SHOW);
		}
	}*/
}

afx_msg LRESULT CWLIView::OnUmStripLoaded(WPARAM wParam, LPARAM lParam) {
	CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Interferogram")));
	if (pWnd) {
		::PostMessage(pWnd->GetSafeHwnd(), UM_STRIP_LOADED, wParam, lParam);
	}
	return 0;
}

void CWLIView::OnWliCamera1() {
	if (!pCam1) {
		pCam1 = new CCameraDlg;
		if (pCam1) {
			pCam1->eID = CAM::PRICAM;
			pCam1->Create(IDD_DIALOG2);
			pCam1->SetWindowTextW(L"Main Camera");
		}
	}
	if (pCam1) {
		if (pCam1->IsWindowVisible()) return;
		pCam1->ShowWindow(SW_SHOW);
	}
}

void CWLIView::OnWliCamera2() {
	if (!pCam2) {
		pCam2 = new CCameraDlg;
		if (pCam2) {
			pCam2->eID = CAM::SECCAM;
			pCam2->Create(IDD_DIALOG2);
			pCam2->SetWindowTextW(L"Secondary Camera");
		}
	}
	if (pCam2) {
		if (pCam2->IsWindowVisible()) return;
		pCam2->ShowWindow(SW_SHOW);
	}
}

afx_msg LRESULT CWLIView::OnUmHeightCalced(WPARAM wParam, LPARAM lParam) {
	//TODO: Perform Height calculation based on selected method

	//! Switch & Notify Height dialog
	if (pHeight) {
		CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Height")));
		if (pWnd) {
			::PostMessage(pWnd->GetSafeHwnd(), UM_HEIGHT_CALCED, wParam, lParam);
		}
	}
	return 0;
}

void CWLIView::OnBaslerCamera() {
	if (pBaslerCam) {
		pBaslerCam->DeleteContents();
		delete pBaslerCam;
		pBaslerCam = nullptr;
	}

	if (!pBaslerCam) {
		pBaslerCam = new BaslerCameraDlg;
		if (pBaslerCam) {
			pBaslerCam->Create(IDD_DIALOG15); //Basler Camera Dlg
			pBaslerCam->SetWindowTextW(L"Main Basler Camera");
			pBaslerCam->ShowWindow(SW_SHOW);
		}
	}
	/*if (pBaslerCam) {
		if (pBaslerCam->IsWindowVisible()) return;
		pBaslerCam->ShowWindow(SW_SHOW);
	}*/
}

// 05302023 - Mortuja [START]

afx_msg LRESULT CWLIView::OnUmHeightCalc(WPARAM wParam, LPARAM lParam) {
	if (heightPlotDlg) {
		delete heightPlotDlg;
		heightPlotDlg = NULL;
	}

	heightPlotDlg = new HeightPlot;
	heightPlotDlg->Create(IDD_DLG_HEIGHT_PLOT);
	heightPlotDlg->ShowWindow(SW_SHOW);

	HWND hWnd = heightPlotDlg->GetSafeHwnd();
	::PostMessage(hWnd, IDC_HEIGHT_MAP_PM, wParam, lParam);

	return 0;
}

// 05302023 - Mortuja [END]

afx_msg LRESULT CWLIView::OnRoughnessDlg(WPARAM wParam, LPARAM lParam) {
	if (roughDlg) {
		delete roughDlg;
		roughDlg = NULL;
	}

	roughDlg = new RoughnessDlg;
	roughDlg->Create(IDD_DLG_ROUGHNESS);
	roughDlg->ShowWindow(SW_SHOW);

	return 0;
}

void CWLIView::OnRecipeCreaterecipe() {
	// TODO: Add your command handler code here
	return;
	if (rcpDlg) {
		delete rcpDlg;
		rcpDlg = NULL;
	}
	rcpDlg = new RecipeDlg;
	if (rcpDlg) {
		rcpDlg->Create(IDD_RCP_DLG);
		rcpDlg->ShowWindow(SW_SHOW);
	}
	//07252023 - END
}

afx_msg LRESULT CWLIView::OnUmResultDlg(WPARAM wParam, LPARAM lParam) {
	//! Switch & Notify Result dialog
	if (rsltDlg) {
		CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Result")));
		if (pWnd) {
			::PostMessage(pWnd->GetSafeHwnd(), UM_RESULT_DLG, wParam, lParam);
		}
	}
	return 0;
}

afx_msg LRESULT CWLIView::OnUmAnalysisDlg(WPARAM wParam, LPARAM lParam) {
	//! Switch & Notify Result dialog
	if (analysisDlg) {
		CWnd* pWnd = cTab.GetSelectTab(cTab.GetTabIndexByName(_T("Analysis")));
		if (pWnd) {
			::PostMessage(pWnd->GetSafeHwnd(), UM_ANALYSIS_DLG, wParam, lParam);
		}
	}
	return 0;
}

//20250112 - Mahmudul Haque--------------
void CWLIView::setButtonIcon(int size)
{
	loadButton.SetIconByID(IDI_ICON1,size);
	saveButton.SetIconByID(IDI_ICON5, size);
	loadDataButton.SetIconByID(IDI_ICON1, size);
	settingButton.SetIconByID(IDI_ICON6, size);
	loginButton.SetIconByID(IDI_ICON2, size);
	cameraMotionButton.SetIconByID(IDI_ICON7, size);
}

//20250112 - Mahmudul Haque
//HBRUSH CWLIView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
//{
//	if (nCtlColor == CTLCOLOR_DLG)
//	{
//		return m_brushBack;
//	}
//	return CResizableFormView::OnCtlColor(pDC, pWnd, nCtlColor);
//}


HBRUSH CWLIView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// --- NEW CODE STARTS HERE ---
	// Check if the control requesting color is your specific static text
	if (pWnd->GetDlgCtrlID() == IDC_TIME_STAMP)
	{
		// Option A: Set the text background to Transparent (lets the brush show through)
		pDC->SetBkMode(TRANSPARENT);

		// Option B: OR Set the text background color explicitly (must match the brush)
		// pDC->SetBkColor(RGB(255, 255, 0)); 

		// Return the brush for the static control
		return m_brushBack;
	}
	// --- NEW CODE ENDS HERE ---


	// Your EXISTING code for the Dialog background
	if (nCtlColor == CTLCOLOR_DLG)
	{
		return m_brushBack;
	}

	return CResizableFormView::OnCtlColor(pDC, pWnd, nCtlColor);
}



void CWLIView::UpdateTimeLabel()
{
	CTime currentTime = CTime::GetCurrentTime();
	CString strTime = currentTime.Format(_T(" %m/%d/%Y \n %I:%M:%S %p"));

	// Ensure the ID here matches your Resource ID exactly
	if (GetDlgItem(IDC_TIME_STAMP) != NULL)
	{
		SetDlgItemText(IDC_TIME_STAMP, strTime);
	}
}


void CWLIView::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		UpdateTimeLabel();
	}
	CResizableFormView::OnTimer(nIDEvent);
}
//20250112 - Mahmudul Haque -------------

void CWLIView::camRun() {
	CAM::SCtx Ctx;
	CAM::CCamera* pCam = Dev.Cam.GetCamera(CAM::PRICAM);
	if (pCam != NULL) {
		Ctx.hWnd = m_cLiveVid.GetSafeHwnd();
		m_cLiveVid.GetClientRect(Ctx.rc);
		pCam->StopStream(Ctx, pCam->SCaM.ID);
		pCam->StartStream(Ctx, pCam->SCaM.ID);
	}
}




void CWLIView::Renumber() {
	pRcp->Renumber();
	pRcp->UpdateControl(rcpDlg->m_cPoint);
	m_cWaferMap.Redraw();
}

LRESULT CWLIView::OnAddPoint(WPARAM wP, LPARAM lP) {
	rcpDlg->Renumber();
	return 0;
}

LRESULT CWLIView::OnAddalgnpoint(WPARAM wP, LPARAM lP) {
	rcpDlg->Renumber();
	return 0;
}

LRESULT CWLIView::OnDeletepoint(WPARAM wP, LPARAM lP) {
	pRcp->UpdateControl(rcpDlg->m_cPoint);
	return 0;
}


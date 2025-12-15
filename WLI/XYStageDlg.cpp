// XYStageDlg.cpp : implementation file
//

#include "pch.h"

#include <thread>

#include "WLI.h"
#include "MoSetup.h"
#include "afxdialogex.h"
#include "XYStageDlg.h"

#include "WLIView.h" 
#include "MainFrm.h"   // To recognize CMainFrame


// CXYStageDlg dialog

IMPLEMENT_DYNAMIC(CXYStageDlg, CDialogEx)

CXYStageDlg::CXYStageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG11, pParent) {}

CXYStageDlg::~CXYStageDlg() {}

void CXYStageDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT19, Xpos);
	DDX_Text(pDX, IDC_EDIT22, Ypos);
	DDX_Text(pDX, IDC_EDIT23, Rpos);
	DDX_Text(pDX, IDC_EDIT24, Apos);
}

BEGIN_MESSAGE_MAP(CXYStageDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CXYStageDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CXYStageDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON42, &CXYStageDlg::OnBnClickedButton42)
	ON_BN_CLICKED(IDC_BUTTON43, &CXYStageDlg::OnBnClickedButton43)
	ON_BN_CLICKED(IDC_BUTTON8, &CXYStageDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON5, &CXYStageDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CXYStageDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON6, &CXYStageDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON44, &CXYStageDlg::OnBnClickedButton44)
	ON_BN_CLICKED(IDC_BUTTON45, &CXYStageDlg::OnBnClickedButton45)
	ON_BN_CLICKED(IDC_BUTTON3, &CXYStageDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CXYStageDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_CHECK11, &CXYStageDlg::OnBnClickedCheck11)
	ON_BN_CLICKED(IDC_BUTTON38, &CXYStageDlg::OnBnClickedButton38)
	ON_BN_CLICKED(IDC_BUTTON39, &CXYStageDlg::OnBnClickedButton39)
	ON_BN_CLICKED(IDC_BUTTON21, &CXYStageDlg::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON22, &CXYStageDlg::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON23, &CXYStageDlg::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON24, &CXYStageDlg::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON80, &CXYStageDlg::OnBnClickedButton80)
	ON_BN_CLICKED(IDC_BUTTON81, &CXYStageDlg::OnBnClickedButton81)
END_MESSAGE_MAP()



void CXYStageDlg::UpdatePositions() {
	CString str;
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	str.Format(_T("X:%.3f mm"), M[int(MOT::MAXIS::X)].now);
	GetDlgItem(IDC_EDIT1)->SetWindowTextW(str);
	str.Format(_T("Y:%.3f mm"), M[int(MOT::MAXIS::Y)].now);
	GetDlgItem(IDC_EDIT9)->SetWindowTextW(str);

	//::PostMessageW(hWndParent, UM_UPDATE_XYZ_VALUE, 0, 0);
	//AfxMessageBox(_T("call ashche bahire"));
	//CWnd* pTargetDlg = GetDlgItem(IDD_WLI_FORM);
	//if (pTargetDlg != nullptr && pTargetDlg->GetSafeHwnd() != nullptr)
	//{
	//	AfxMessageBox(_T("call ashche vitore"));
	//pTargetDlg->PostMessage(UM_UPDATE_XYZ_VALUE, 0, 0);
	//}



	CWnd* pMainWnd = AfxGetMainWnd();

	// Cast it to your MainFrame class
	CMainFrame* pMainFrame = dynamic_cast<CMainFrame*>(pMainWnd);

	if (pMainFrame != nullptr)
	{
		// Ask the Main Frame for the currently active View
		CView* pView = pMainFrame->GetActiveView();
		TRACE("Parent Class: %s\n", pView->GetRuntimeClass()->m_lpszClassName);//tabctrl

		// Verify this is actually your WLIView before sending
		if (pView && pView->IsKindOf(RUNTIME_CLASS(CWLIView)))
		{
			// SUCCESS: We found the view
			pView->PostMessage(UM_UPDATE_XYZ_VALUE, (WPARAM)1, (LPARAM)0);
		}
		else
		{
			// Fallback: If for some reason the view isn't active, 
			// you might want to try sending it to the MainFrame itself
			// pMainFrame->PostMessage(UM_UPDATE_XYZ_VALUE, (WPARAM)1, (LPARAM)0);
			TRACE("Could not find CWLIView!\n");
		}
	}


	//pParent->PostMessage(UM_UPDATE_XYZ_VALUE, (WPARAM)1, (LPARAM)0);


}

void CXYStageDlg::MoveR(MOT::MAXIS nAx, float inc, DWORD tout, bool bCheck) {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MO->MoveR(nAx, inc, tout, bCheck);
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

BOOL CXYStageDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();

	MO = Dev.MC.get();
	((CButton*)GetDlgItem(IDC_CHECK11))->SetCheck(bCheck);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CXYStageDlg::OnBnClickedButton1() {
	MoveR(MOT::MAXIS::Y, incXY, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton2() {
	MoveR(MOT::MAXIS::Y, incXY / 10.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton42() {
	MoveR(MOT::MAXIS::Y, incXY / 100.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton43() {
	MoveR(MOT::MAXIS::Y, -incXY / 100.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton8() {
	MoveR(MOT::MAXIS::Y, -incXY / 10.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton5() {
	MoveR(MOT::MAXIS::Y, -incXY, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton7() {
	MoveR(MOT::MAXIS::X, -incXY, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton6() {
	MoveR(MOT::MAXIS::X, -incXY / 10.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton44() {
	MoveR(MOT::MAXIS::X, -incXY / 100.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton45() {
	MoveR(MOT::MAXIS::X, incXY / 100.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton3() {
	MoveR(MOT::MAXIS::X, incXY / 10.f, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedButton4() {
	MoveR(MOT::MAXIS::X, incXY, 20000, bCheck);
}

void CXYStageDlg::OnBnClickedCheck11() {
	if (bCheck) bCheck = false; else bCheck = true;
}

void CXYStageDlg::OnBnClickedButton38() {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MO->stage.ResetXY0();
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

void CXYStageDlg::OnBnClickedButton39() {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MO->stage.SetZeroPos();
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

void CXYStageDlg::OnBnClickedButton21() {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MO->stage.Home(bCheck);
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

void CXYStageDlg::OnBnClickedButton22() {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MO->stage.ResetXY();
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

void CXYStageDlg::OnBnClickedButton23() {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	std::thread A(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::X, M[int(MOT::MAXIS::X)].max, 50000, true);
	std::thread B(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::Y, M[int(MOT::MAXIS::Y)].max, 50000, true);
	A.join(); B.join();
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

void CXYStageDlg::OnBnClickedButton24() {
	CWaitCursor wc;
	if (CMoSetupB::bBlock) return; CMoSetupB::bBlock = true;
	MOT::SMotPar* M = &MO->tilt.Mpar[0];
	std::thread A(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::X, M[int(MOT::MAXIS::X)].min, 50000, true);
	std::thread B(&MOT::CMotorCtrl::MoveA, MO, MOT::MAXIS::Y, M[int(MOT::MAXIS::Y)].min, 50000, true);
	A.join(); B.join();
	UpdatePositions();
	CMoSetupB::bBlock = false;
}

void CXYStageDlg::OnBnClickedButton80() {
	UpdateData(TRUE);
	MO->stage.Cart2Polar(Xpos, Ypos, Rpos, Apos);
	UpdateData(FALSE);
	MO->stage.GotoXY(Xpos, Ypos, 10000, true);
}

void CXYStageDlg::OnBnClickedButton81() {
	UpdateData(TRUE);
	MO->stage.Polar2Cart(Rpos, Apos, Xpos, Ypos);
	UpdateData(FALSE);
	MO->stage.GotoXY(Xpos, Ypos, 10000, true);
}
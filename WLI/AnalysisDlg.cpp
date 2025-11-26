// AnalysisDlg.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "AnalysisDlg.h"
#include "afxdialogex.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <locale>
#include <codecvt>
#include "Pegrpapi.h"
#include "filters.h"
#include <iomanip>
#include <algorithm>
#include <cmath>
#include "SRC\DosUtil.h"
using namespace std;

// AnalysisDlg dialog
#pragma comment(lib,"PEGRP64G.lib")

//10212024 / ARIFUL ISLAM

//IMPLEMENT_DYNAMIC(CSetupProp17Dlg, CDialogEx)

//[ alexander
COLORREF m_bgColor = RGB(255, 255, 255);
COLORREF m_lnColor = RGB(0, 128, 255);
COLORREF m_grColor = RGB(64, 64, 64);
COLORREF m_grColor2 = RGB(255, 128, 0);
COLORREF m_txtColor = RGB(16, 16, 16);
//]

IMPLEMENT_DYNAMIC(AnalysisDlg, CResizableDialog)

AnalysisDlg::AnalysisDlg(CWnd* pParent /*=nullptr*/)
	: CResizableDialog(IDD_ANALYSIS_DLG, pParent)
	, drawProfile(TRUE)
	, m_lineProfileX1(0)
	, m_lineProfileY1(0)
	, m_lineProfileX2(0)
	, m_lineProfileY2(0)
	, isArea(FALSE)
	, isHorz(TRUE)
	, distance(0.0F)
	, leftButtonClkCnt(0)
	, profileCnt(0)
	, profileCntDist(0)
	, isMM(FALSE)
	, m_step(1.0F)
	, isCtrlPressed(FALSE)
	, distLine(TRUE)
	, depthLine(FALSE)
	, isDistMarked(FALSE)
	, twoPointHeight(FALSE)
{

	m_fProfileHeight = 0.0;
	m_fLevel1 = 0.15;
	m_fLevel2 = 0.15;
	m_fk = 0.0;
	m_fb = 0.0;
	nProfileType = 0;
	nProfCnt = 0;
	pProfile = NULL;
	pProfileXData = NULL;
	pProfileYData = NULL;
	nTotalCnt = 0;
	fMin = 1e20;
	fMax = -1e20;
	nLeftBorder = 0;
	nRightBorder = 0;
	lowThresh = -1;
	highThresh = -1;
	m_xStep = 1;
	m_yStep = 1;

	//CString inifile;
	//DosUtil.GetLocalSysFile(inifile);
	CString inifile = CString(DosUtil.GetLocalSysFile().c_str());
	//m_xStep = DosUtil.ReadINI("Plot", "XStep", m_xStep, inifile);
	//m_xStep = 10;
	//m_yStep = DosUtil.ReadINI("Plot", "YStep", m_yStep, inifile);
	//m_xStep = DosUtil.ReadINI(_T("Plot"), _T("XStep"), static_cast<int>(m_xStep), inifile);
	m_xStep = DosUtil.ReadINI(_T("Plot"), _T("XStep"), m_xStep, static_cast<LPCTSTR>(inifile));
	m_yStep = DosUtil.ReadINI(_T("Plot"), _T("YStep"), m_yStep, static_cast<LPCTSTR>(inifile));

}

AnalysisDlg::~AnalysisDlg()
{
	if (m_filterOptionPopUp) {
		if (m_filterOptionPopUp->GetSafeHwnd()) {
			m_filterOptionPopUp->DestroyWindow();
		}
		delete m_filterOptionPopUp;
		m_filterOptionPopUp = nullptr;
	}
}

void AnalysisDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STEP, stepControl);
	DDX_Text(pDX, IDC_STEP, m_step);
}

BEGIN_MESSAGE_MAP(AnalysisDlg, CResizableDialog)

	ON_BN_CLICKED(IDC_DRAW_CHART, &AnalysisDlg::OnBnClickedDrawChart)
	ON_BN_CLICKED(IDC_FILTER_OPTIONS, &AnalysisDlg::OnBnClickedFilterOptions)
	ON_MESSAGE(WM_FILTER_APPLY, &AnalysisDlg::OnFilterApply)
	ON_MESSAGE(WM_MULTIPLYx1000, &AnalysisDlg::OnMultiplyx1000)
	ON_MESSAGE(WM_UM, &AnalysisDlg::OnUM)
	ON_MESSAGE(WM_THRESHOLD, &AnalysisDlg::OnThresholdApply)
	ON_BN_CLICKED(IDC_HISTOGRAM_CONTROL, &AnalysisDlg::OnBnClickedHistogramControl)
	ON_BN_CLICKED(IDC_ABSDIST, &AnalysisDlg::OnBnClickedAbsdist)
	ON_BN_CLICKED(IDC_RADIO_MINMAX, &AnalysisDlg::OnBnClickedRadioMinmax)
	ON_BN_CLICKED(IDC_RADIO_2POINTS, &AnalysisDlg::OnBnClickedRadio2points)
END_MESSAGE_MAP()

// AnalysisDlg message handlers

void AnalysisDlg::LocalToRecipe() {
}
void AnalysisDlg::RecipeToLocal() {

}

//AnalysisDlg* AnalysisDlg::analysisDlgPointer = NULL;
BOOL AnalysisDlg::OnInitDialog() {
	__super::OnInitDialog();
	// analysisDlgPointer = this;

	//[ setting position
	AddAnchor(IDC_DRAW_CHART, TOP_RIGHT);
	AddAnchor(IDC_FILTER_OPTIONS, TOP_RIGHT);
	AddAnchor(IDC_STEP_BUTTON, TOP_RIGHT);
	AddAnchor(IDC_STEP, TOP_RIGHT);
	AddAnchor(IDC_HISTOGRAM_CONTROL, TOP_RIGHT);
	AddAnchor(IDC_ABSDIST, TOP_RIGHT);
	AddAnchor(IDC_GB_STEPHEIGHT, TOP_RIGHT);
	AddAnchor(IDC_RADIO_MINMAX, TOP_RIGHT);
	AddAnchor(IDC_RADIO_2POINTS, TOP_RIGHT);
	//]
	UpdateData(FALSE);
	((CButton*)GetDlgItem(IDC_ABSDIST))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_HISTOGRAM_CONTROL))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_MINMAX))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_2POINTS))->SetCheck(FALSE);
	return true;
}

BOOL AnalysisDlg::PreTranslateMessage(MSG* pMsg) {
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_CONTROL) {
		// Set isCtrlPressed to TRUE when Ctrl key is pressed
		isCtrlPressed = TRUE;
	}
	else if (pMsg->message == WM_KEYUP && pMsg->wParam == VK_CONTROL) {
		// Set isCtrlPressed to FALSE when Ctrl key is released
		isCtrlPressed = FALSE;
	}

	// Call the base class function for default processing
	return CResizableDialog::PreTranslateMessage(pMsg);
}
int AnalysisDlg::euclideanDist(int x1, int y1, int x2, int y2) {
	return std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

BOOL AnalysisDlg::isPointOnLine(pair<int, int>p) {
	// check in multiprofile
	int x1, y1, x2, y2, x, y, collinearity;
	bool flag = false;
	x = p.first;
	y = p.second;
	for (int i = 0; i < multiProfile.size(); i++) {
		x1 = multiProfile[i].first.first.first;
		y1 = multiProfile[i].first.first.second;
		x2 = multiProfile[i].first.second.first;
		y2 = multiProfile[i].first.second.second;

		int total = euclideanDist(x1, y1, x2, y2);
		int d1 = euclideanDist(x1, y1, x, y);
		int d2 = euclideanDist(x2, y2, x, y);

		if (abs(total - (d1 + d2)) <= 5) {
			multiProfile.erase(multiProfile.begin() + i);
			heightTwoPt.erase(heightTwoPt.begin() + (i - 1));
			flag = true;
			if (profileCnt > 0)profileCnt--;
			//break;
		}

	}

	// check in multiprofileDist
	for (int i = 0; i < multiProfileDist.size(); i++) {
		x1 = multiProfileDist[i].first.first.first;
		y1 = multiProfileDist[i].first.first.second;
		x2 = multiProfileDist[i].first.second.first;
		y2 = multiProfileDist[i].first.second.second;

		int total = euclideanDist(x1, y1, x2, y2);
		int d1 = euclideanDist(x1, y1, x, y);
		int d2 = euclideanDist(x2, y2, x, y);

		if (abs(total - (d1 + d2)) <= 5) {
			multiProfileDist.erase(multiProfileDist.begin() + i);

			if (profileCntDist > 0)profileCntDist--;
			flag = true;
			//break;
		}
	}

	return flag ? TRUE : FALSE;
}

BOOL AnalysisDlg::OnCommand(WPARAM wParam, LPARAM lParam) {
	if (lParam != (LPARAM)m_hPEh && lParam != (LPARAM)m_hPE2 && lParam != (LPARAM)m_hPE3 && lParam != (LPARAM)m_hPEl)
		return CResizableDialog::OnCommand(wParam, lParam);

	// handle mouse events on Histogram

	//if (lParam == (LPARAM)m_hPEh) {
	//	HOTSPOTDATA hsd;
	//	PEvget(m_hPEh, PEP_structHOTSPOTDATA, &hsd);
	//	//KEYDOWNDATA hsd;
	//	double dX, dY;
	//	if (HIWORD(wParam) == PEWN_LBUTTONUP && hsd.nHotSpotType == PEHS_DATAPOINT) {
	//		PEvget(m_hPEh, PEP_fCURSORVALUEX, &dX);
	//		PEvget(m_hPEh, PEP_fCURSORVALUEY, &dY);
	//		mouseClickCount++;
	//		if (mouseClickCount & 1) {// odd click	
	//			PEvgetcellEx(m_hPEh, PEP_faXDATA, hsd.w1, hsd.w2, &thresh1);
	//			x1 = static_cast<float>(dX);
	//		}
	//		else {
	//			x2 = static_cast<float>(dX);
	//			PEvgetcellEx(m_hPEh, PEP_faXDATA, hsd.w1, hsd.w2, &thresh2);
	//		}
	//		if (mouseClickCount > 1) {
	//			lowThresh = min(thresh1, thresh2);
	//			highThresh = max(thresh1, thresh2);
	//			float lowX = min(x1, x2);
	//			float highX = max(x1, x2);
	//			BOOL checkVal = false;
	//			if (isClicked)checkVal = m_filterOptionPopUp->appliedFilters();
	//			if (!checkVal) {
	//				filterData = data;
	//			}
	//			else {
	//				filterData = m_filterOptionPopUp->filterData;
	//			}
	//			Histogram(freqCnt, lowX, highX);
	//			applyHistThreshold(lowThresh, highThresh, filterData);
	//			Create2D();
	//			Create3D();
	//			if (!isArea)lineProfile();
	//			else areaProfile();
	//		}
	//	}
	//}

	// handle mouse event on line profile
	//if (lParam == (LPARAM)m_hPEl) {
	//	TCHAR  buffer[128];
	//	double dX;
	//	PEvget(m_hPEl, PEP_fCURSORVALUEX, &dX);
	//	double dY;
	//	PEvget(m_hPEl, PEP_fCURSORVALUEY, &dY);
	//	double dZ;
	//	PEvget(m_hPEl, PEP_fCURSORVALUEZ, &dZ);
	//	POINT pt;
	//	HOTSPOTDATA hsd;
	//	PEvget(m_hPEl, PEP_ptLASTMOUSEMOVE, &pt);
	//	if (HIWORD(wParam) == PEWN_LBUTTONUP && hsd.nHotSpotType == PEHS_DATAPOINT) {
	//		// call PEgethotspot //
	//		PEgethotspot(m_hPEl, pt.x, pt.y);
	//		PEvget(m_hPEl, PEP_structHOTSPOTDATA, &hsd);
	//		PEnset(m_hPEl, PEP_dwTRACKINGTOOLTIPBKCOLOR, PERGB(0, 50, 50, 50));
	//		PEnset(m_hPEl, PEP_dwTRACKINGTOOLTIPTEXTCOLOR, PERGB(0, 100, 255, 50));
	//		if (isMM)sprintf_s(buffer, TEXT(" X=%.0f[px]\t Y=%.0f[px]\t Height=%.4f[um]"), dZ, dX, dY / 1000.0);
	//		else sprintf_s(buffer, TEXT(" X=%.0f[px]\t Y=%.0f[px]\t Height=%.4f[um]"), dZ, dX, dY);
	//		PEszset(m_hPEl, PEP_szTRACKINGTEXT, buffer);
	//		//}
	//		if (HIWORD(wParam) == PEWN_LBUTTONUP) {
	//			POINT pt;
	//			HOTSPOTDATA hsd;
	//			TCHAR buffer[128];
	//			TCHAR buffer2[128];
	//			// get last mouse location within control //
	//			PEvget(m_hPEl, PEP_ptLASTMOUSEMOVE, &pt);
	//			// call PEgethotspot //
	//			PEgethotspot(m_hPEl, pt.x, pt.y);
	//			// now look at HotSpotData structure //
	//			PEvget(m_hPEl, PEP_structHOTSPOTDATA, &hsd);
	//			if (hsd.nHotSpotType == PEHS_DATAPOINT) {
	//			}
	//		}
	//		if (HIWORD(wParam) == PEWN_CUSTOMTRACKINGDATATEXT)
	//		{
	//			// v9 features 
	//			double dX;
	//			PEvget(m_hPEl, PEP_fCURSORVALUEX, &dX);
	//			TCHAR  buffer[96];
	//			double dY;
	//			PEvget(m_hPEl, PEP_fCURSORVALUEY, &dY);
	//			_stprintf(buffer, TEXT("X Axis: %.3f  \nY Axis: %.3f"), dX, dY);
	//			if (dY < 0.0F)
	//			{
	//				//PEszset(m_hPEl, PEP_szTRACKINGTOOLTIPTITLE, TEXT("N E G A T I V E"));
	//				PEnset(m_hPEl, PEP_dwTRACKINGTOOLTIPBKCOLOR, PERGB(0, 0, 0, 0));
	//				PEnset(m_hPEl, PEP_dwTRACKINGTOOLTIPTEXTCOLOR, PERGB(0, 245, 0, 0));
	//			}
	//			else
	//			{
	//				//PEszset(m_hPEl, PEP_szTRACKINGTOOLTIPTITLE, TEXT("P O S I T I V E"));
	//				PEnset(m_hPEl, PEP_dwTRACKINGTOOLTIPBKCOLOR, PERGB(0, 50, 50, 50));
	//				PEnset(m_hPEl, PEP_dwTRACKINGTOOLTIPTEXTCOLOR, PERGB(0, 0, 225, 0));
	//			}
	//			PEszset(m_hPEl, PEP_szTRACKINGTEXT, buffer);
	//		}
	//	}
	//}

	//handle mouse event on 2D plot
	if (lParam == (LPARAM)m_hPE2) {
		TCHAR  buffer[128];
		double dX;
		PEvget(m_hPE2, PEP_fCURSORVALUEX, &dX);
		double dY;
		PEvget(m_hPE2, PEP_fCURSORVALUEY, &dY);
		double dZ;
		PEvget(m_hPE2, PEP_fCURSORVALUEZ, &dZ);
		POINT pt;
		HOTSPOTDATA hsd;
		PEvget(m_hPE2, PEP_ptLASTMOUSEMOVE, &pt);
		dX /= m_xStep;
		dY /= m_yStep;

		// call PEgethotspot //
		PEgethotspot(m_hPE2, pt.x, pt.y);

		// now look at HotSpotData structure //
		PEvget(m_hPE2, PEP_structHOTSPOTDATA, &hsd);
		PEnset(m_hPE2, PEP_dwTRACKINGTOOLTIPBKCOLOR, PERGB(0, 50, 50, 50));
		PEnset(m_hPE2, PEP_dwTRACKINGTOOLTIPTEXTCOLOR, PERGB(0, 100, 255, 50));
		// 20251118
		// _stprintf_s automatically chooses sprintf_s or swprintf_s based on Unicode/ANSI.
		if (isMM)_stprintf_s(buffer, TEXT("X=%.0f\t  Y=%.0f\t Height=%.3f [um]"), dX * m_xStep, dY * m_yStep, dZ);
		else _stprintf_s(buffer, TEXT("X=%.0f\t  Y=%.0f\t Height=%.3f [um]"), dX * m_xStep, dY * m_yStep, dZ);
		PEszset(m_hPE2, PEP_szTRACKINGTEXT, buffer);
		// left button event handle start 
		if (HIWORD(wParam) == PEWN_LBUTTONUP && isCtrlPressed) {
			m_lineProfileX1 = static_cast<int>(dX);
			m_lineProfileY1 = static_cast<int>(dY);
			pair<float, float>checkPoint = { m_lineProfileX1,m_lineProfileY1 };
			BOOL flag = FALSE;
			flag = isPointOnLine(checkPoint);
			if (flag) {
				Create2D();
				lineProfile();
			}
			leftButtonClkCnt = 0;
			//p1.reset();
		}
		else if (HIWORD(wParam) == PEWN_LBUTTONUP && isDistMarked) {
			m_lineProfileX1 = static_cast<int>(dX);
			m_lineProfileY1 = static_cast<int>(dY);
			pointDist.push_back({ m_lineProfileX1,m_lineProfileY1 });

			Create2D();
		}
		else if (HIWORD(wParam) == PEWN_LBUTTONUP && leftButtonClkCnt == 0) {
			drawProfile = TRUE;
			m_lineProfileX1 = static_cast<int>(dX);
			m_lineProfileY1 = static_cast<int>(dY);
			p1 = { m_lineProfileX1,m_lineProfileY1 };
			height1Val = static_cast<float>(dZ);
			//if(isCtrlPressed)isPointOnLine(p1);
			leftButtonClkCnt = 1;
		}
		else if (HIWORD(wParam) == PEWN_LBUTTONUP && leftButtonClkCnt == 1) {
			isDepth = TRUE;
			depthLine = TRUE;
			distLine = FALSE;
			//isDist = FALSE;
			m_lineProfileX2 = static_cast<int>(dX);
			m_lineProfileY2 = static_cast<int>(dY);
			p2 = { m_lineProfileX2,m_lineProfileY2 };
			if (twoPointHeight == TRUE)flagDepth = TRUE;
			else flagDepth = FALSE;
			multiProfile.push_back({ { p1,p2 },flagDepth });
			height2Val = static_cast<float>(dZ);
			heightTwoPt.push_back({ height1Val,height2Val });
			profileCnt++;
			leftButtonClkCnt = 0;
			Create2D();
			/*if (isArea)areaProfile();
			else */
			lineProfile();
			//AdditionalCalculations();
		}

		//left button event handle end

		// middle button event handle start
		//if (HIWORD(wParam) == PEWN_MBUTTONCLK) {
		//	drawProfile = TRUE;
		//	m_lineProfileX1 = static_cast<int>(dX);
		//	m_lineProfileY1 = static_cast<int>(dY);
		//	p1 = { m_lineProfileX1,m_lineProfileY1 };
		//}
		//else if (HIWORD(wParam) == PEWN_MBUTTONUP) {
		//	//isDepth = FALSE;
		//	isDist = TRUE;
		//	distLine = TRUE;
		//	depthLine = FALSE;
		//	profileCntDist++;
		//	m_lineProfileX2 = static_cast<int>(dX);
		//	m_lineProfileY2 = static_cast<int>(dY);
		//	p2 = { m_lineProfileX2,m_lineProfileY2 };
		//	//BOOL flag1 = isPointOnLine(p1);
		//	//BOOL flag2 = isPointOnLine(p2);
		//	multiProfileDist.push_back({ { p1,p2 },0 });
		//	Create2D();
		//	/*if (isArea)areaProfile();
		//	else*/
		//	lineProfile();
		//	//AdditionalCalculations();
		//}
		// middle button event handle end
		return true;
	}

	// handle mouse event on 3D plot
	if (lParam == (LPARAM)m_hPE3) {
		TCHAR  buffer[128];
		double dX;
		PEvget(m_hPE3, PEP_fCURSORVALUEX, &dX);
		double dY;
		PEvget(m_hPE3, PEP_fCURSORVALUEY, &dY);
		double dZ;
		PEvget(m_hPE3, PEP_fCURSORVALUEZ, &dZ);
		POINT pt;
		HOTSPOTDATA hsd;
		PEvget(m_hPE3, PEP_ptLASTMOUSEMOVE, &pt);
		dX /= m_xStep;
		dZ /= m_yStep;// not dY because dY is the data.. 
		// call PEgethotspot //
		PEgethotspot(m_hPE3, pt.x, pt.y);
		PEvget(m_hPE3, PEP_structHOTSPOTDATA, &hsd);
		PEnset(m_hPE3, PEP_dwTRACKINGTOOLTIPBKCOLOR, PERGB(0, 50, 50, 50));
		PEnset(m_hPE3, PEP_dwTRACKINGTOOLTIPTEXTCOLOR, PERGB(0, 100, 255, 50));

		if (isMM) {
			_stprintf_s(buffer, TEXT(" X=%.0f um\t Y=%.0f um\t Height=%.4f[um]"), dX * m_xStep, dZ * m_yStep, dY / 1000.0);
		}
		else {
			_stprintf_s(buffer, TEXT(" X=%.0f um\t Y=%.0f um\t Height=%.4f[um]"), dX * m_xStep, dZ * m_yStep, dY);
		}
		PEszset(m_hPE3, PEP_szTRACKINGTEXT, buffer);
	}

	if (lParam == (LPARAM)m_hPEl) {


		double dX, dY;
		PEvget(m_hPEl, PEP_fCURSORVALUEX, &dX);
		PEvget(m_hPEl, PEP_fCURSORVALUEY, &dY);

		HOTSPOTDATA hsd;
		PEvget(m_hPEl, PEP_structHOTSPOTDATA, &hsd);
		//KEYDOWNDATA hsd;
		if (HIWORD(wParam) == PEWN_LBUTTONUP && hsd.nHotSpotType == PEHS_DATAPOINT) {
			mouseClickCount++;
			if (mouseClickCount & 1) {// odd click	
				PEvgetcellEx(m_hPEl, PEP_faXDATA, hsd.w1, hsd.w2, &thresh1);
				x1 = static_cast<float>(dX);
				y1 = static_cast<float>(dY);
			}
			else {
				x2 = static_cast<float>(dX);
				y2 = static_cast<float>(dY);
				PEvgetcellEx(m_hPEl, PEP_faXDATA, hsd.w1, hsd.w2, &thresh2);
			}
			if (mouseClickCount > 1) {
				lowThresh = min(x1, x2);
				highThresh = max(x1, x2);
				lowY = min(y1, y2);
				highY = max(y1, y2);
				lineProfile();
			}
		}
	}
	return true;
}

void AnalysisDlg::calcFrequency(std::vector<std::vector<float>>& data) {
	freqCnt.clear();
	int mxXid = -1, mxYid = -1, mxCnt = 0;
	float mxCntVal = 0;
	for (int row = 0; row < data.size(); ++row) {
		for (int col = 0; col < data[row].size(); ++col) {
			float val = ceilf(data[row][col] * 100) / 100;
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(2) << val;
			std::string s = oss.str();
			val = std::stof(s);
			freqCnt[val]++;
			if (freqCnt[val] > mxCnt) {
				mxCnt = max(mxCnt, freqCnt[val]);
				mxCntVal = val;
				mxXid = row, mxYid = col;
			}
		}
	}
}

void AnalysisDlg::lineProfile(std::vector<float>profile) {
	if (m_hPEl)PEdestroy(m_hPEl);
	CStatic* pStaticText = (CStatic*)GetDlgItem(IDC_LINEPROFILE_VIEW);
	CRect rect;
	pStaticText->GetWindowRect(&rect);
	ScreenToClient(&rect);
	/*GetDlgItem(IDC_LINE_POPUP)->GetWindowRect(&itemRect);
	itemRect.left -= wndRect.left;
	itemRect.top -= wndRect.top;
	itemRect.right -= wndRect.left;
	itemRect.bottom -= wndRect.top;*/

	m_hPEl = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &rect, m_hWnd, 1001);


	//Set up cursor
	//PEnset(m_hPEl, PEP_nCURSORMODE, PECM_DATACROSS);

	//Set PlottingMethod //
	PEnset(m_hPEl, PEP_nPLOTTINGMETHOD, PEGPM_LINE);

	// Help see data points //
	//PEnset(m_hPEl, PEP_bMARKDATAPOINTS, TRUE);

	// This will allow you to move cursor by clicking data point //
	/*PEnset(m_hPEl, PEP_bMOUSECURSORCONTROL, TRUE);
	PEnset(m_hPEl, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(m_hPEl, PEP_nHOTSPOTSIZE, 50);*/

	PEnset(m_hPEl, PEP_bBITMAPGRADIENTMODE, FALSE);
	PEnset(m_hPEl, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);//PEQS_LIGHT_INSET

	// Enable Zooming //
	//PEnset(m_hPE2, PEP_nALLOWZOOMING, PEAZ_NONE);//HORZANDVERT
	//PEnset(m_hPE2, PEP_nMOUSEWHEELFUNCTION, PEMWF_HORZPLUSVERT_ZOOM);	
	//PEnset(m_hPEl, PEP_bMOUSEDRAGGINGX, TRUE);  // note that pan gestures require MouseDragging to be enabled 
	//PEnset(m_hPEl, PEP_bMOUSEDRAGGINGY, TRUE);

	// Enable MouseWheel Zoom Smoothness
	/*PEnset(m_hPEl, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 5);
	PEnset(m_hPEl, PEP_nPINCHZOOMSMOOTHNESS, 2);

	PEnset(m_hPEl, PEP_bZOOMWINDOW, TRUE);*/


	////Axis scale
	PEnset(m_hPEl, PEP_nXAXISSCALECONTROL, PEAC_NORMAL);
	PEnset(m_hPEl, PEP_nYAXISSCALECONTROL, PEAC_NORMAL);
	PEnset(m_hPEl, PEP_nZAXISSCALECONTROL, PEAC_NORMAL);



	// Pass Data //
	int nTotalCnt = static_cast<int>(profile.size());
	// build a profile
	float* pProfileYData;
	float mxVal = -100000, mnVal = 100000;
	pProfileYData = new float[nTotalCnt];

	PEnset(m_hPEl, PEP_nSUBSETS, 1);//nTotalCnt
	PEnset(m_hPEl, PEP_nPOINTS, nTotalCnt);

	// 20251118
	for (int i = 0; i < nTotalCnt; i++) {
		pProfileYData[i] = profile[i];
		mnVal = min(mnVal, profile[i]);
		mxVal = max(mxVal, profile[i]);
		// compute x value as double for safe formatting
		double xval = static_cast<double>(i) * 10.0;
		// format into CString using the TCHAR-aware macro for the literal
		CString xAxisVal;
		xAxisVal.Format(_T("%.2f"), xval); // use _T and %.2f
		/*TCHAR* xAxis;
		xAxis = _tcsdup(xAxisVal);
		PEvsetcell(m_hPEl, PEP_szaPOINTLABELS, i, TEXT(xAxis));*/
		PEvsetcell(m_hPEl, PEP_szaPOINTLABELS, i, (void*)(LPCTSTR)xAxisVal);
	}
	//PEvset(m_hPEl, PEP_faXDATA, pProfileXData, nTotalCnt);
	PEvset(m_hPEl, PEP_faYDATA, pProfileYData, nTotalCnt);
	//float fLevel1 = static_cast<float>(fMin + (fMax - fMin) * 0.20);
	//float fLevel2 = static_cast<float>(fMax - (fMax - fMin) * 0.20);

	// Manually Control Y Axis //
	PEnset(m_hPEl, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
	double d = static_cast<double>(mnVal - (abs(mnVal)));
	PEvset(m_hPEl, PEP_fMANUALMINY, &d, 1);
	d = static_cast<double>(mxVal + (mxVal));
	PEvset(m_hPEl, PEP_fMANUALMAXY, &d, 1);

	// Main title and sub title
	// 20251118
	CString title;
	title.Format(_T("Line Profile %d"), 1);
	TCHAR* mainTitlet;
	mainTitlet = _tcsdup(title);
	PEszset(m_hPEl, PEP_szMAINTITLE, mainTitlet);
	TCHAR subTitle[] = TEXT("||");
	PEszset(m_hPEl, PEP_szSUBTITLE, subTitle);
	// Axis label
	TCHAR szBufZ[] = TEXT("Height um");
	PEszset(m_hPEl, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufX[] = TEXT("X um");
	PEszset(m_hPEl, PEP_szXAXISLABEL, szBufX);

	//subset labels 
	PEvsetcell(m_hPEl, PEP_szaSUBSETLABELS, 0, (void*)TEXT("Distance"));

	/*PEnset(m_hPEl, PEP_nIMAGEADJUSTTOP, 10);
	PEnset(m_hPEl, PEP_nIMAGEADJUSTLEFT, 20);
	PEnset(m_hPEl, PEP_nIMAGEADJUSTRIGHT, 20);*/



	// subset colors, line type, point type
	DWORD dwArray[1] = { PERGB(255,5,249,253) };
	int nLineTypes[] = { PELT_THICKSOLID };
	int nPointTypes[] = { PEPT_DOTSOLID };

	PEvsetEx(m_hPEl, PEP_dwaSUBSETCOLORS, 0, 1, dwArray, 0);
	PEvset(m_hPEl, PEP_naSUBSETLINETYPES, nLineTypes, 1);
	PEvset(m_hPEl, PEP_naSUBSETPOINTTYPES, nPointTypes, 1);

	if (pProfileYData)
		delete[] pProfileYData;

	// v9 features 
	/*PEnset(m_hPEl, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPEl, PEP_nCURSORPROMPTSTYLE, PECPS_YVALUE);
	PEnset(m_hPEl, PEP_nCURSORPROMPTLOCATION, PECPL_TRACKING_TOOLTIP);

	PEnset(m_hPEl, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);
	PEnset(m_hPEl, PEP_bTRACKINGCUSTOMOTHERTEXT, TRUE);*/

	/*PEnset(m_hPEl, PEP_bALLOWTITLEHOTSPOTS, FALSE);
	PEnset(m_hPEl, PEP_bALLOWSUBSETHOTSPOTS, TRUE);
	PEnset(m_hPEl, PEP_bALLOWAXISLABELHOTSPOTS, FALSE);
	PEnset(m_hPEl, PEP_bALLOWGRIDNUMBERHOTSPOTSY, FALSE);
	PEnset(m_hPEl, PEP_bALLOWGRIDNUMBERHOTSPOTSX, FALSE);*/

	/*PEnset(m_hPEl, PEP_bMOUSECURSORCONTROLCLOSESTPOINT, TRUE);
	PEnset(m_hPEl, PEP_bALLOWDATAHOTSPOTS, TRUE);*/

	/*PEnset(m_hPEl, PEP_bCACHEBMP2, TRUE);
	PEnset(m_hPEl, PEP_bDRAWCURSORTOCACHE, TRUE);
	PEnset(m_hPEl, PEP_bIMPROVEDCURSOR, TRUE);
	PEnset(m_hPEl, PEP_nCURSORVLINETYPE, PELT_DASH);
	PEnset(m_hPEl, PEP_nCURSORHLINETYPE, PELT_DASH);
	PEnset(m_hPEl, PEP_dwCURSORCOLOR, PERGB(255, 255, 0, 0));*/

	PEnset(m_hPEl, PEP_bMOUSECURSORCONTROL, TRUE);
	PEnset(m_hPEl, PEP_nRENDERENGINE, PERE_DIRECT2D);
	// Enable Zooming //
	/*PEnset(m_hPEl, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
	PEnset(m_hPEl, PEP_nMOUSEWHEELFUNCTION, PEMWF_HORZPLUSVERT_ZOOM);
	PEnset(m_hPEl, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPEl, PEP_bGRIDBANDS, FALSE);
	PEnset(m_hPEl, PEP_nVERTSCROLLPOS, 1);
	float fZ = 2.00F; PEvset(m_hPEl, PEP_fMOUSEWHEELZOOMFACTOR, &fZ, 1);*/

}

//20250916
void AnalysisDlg::applyDespike(float* pProfileYData, int sz) {
	int windowSize = 10;
	int half = windowSize / 2;

	for (int i = half; i < sz - half; ++i) {
		std::vector<float> win;
		win.reserve(windowSize);
		for (int j = -half; j <= half; ++j) {
			win.push_back(pProfileYData[i + j]);
		}

		std::vector<float>tmp = win;
		std::nth_element(tmp.begin(), tmp.begin() + half, tmp.end());
		float median = tmp[half];

		pProfileYData[i] = median;
	}
}

void AnalysisDlg::applyDespikeVec(std::vector<std::vector<float>>& data) {
	int windowSize = 10;
	int half = windowSize / 2;
	int rows = data[0].size();
	int cols = data.size();

	for (int y = 1; y < rows-1; ++y) {
		for (int x = 1; x < cols-1; ++x) {
			float val = data[x][y];
			float kernel[9];
			int k = 0;
			for (int j = -1; j <= 1; j++) {
				for (int i = -1; i <= 1; i++) {
					kernel[k++] = data[x + i][y + j];
				}
			}
			std::nth_element(kernel, kernel + 4, kernel + 9);
			float median = kernel[4];

			data[x][y] = median;
		}
	}
}

void AnalysisDlg::lineProfile()
{
	UpdateData(TRUE);
	std::vector<std::vector<float>>profile;
	profile.clear();
	profile = filterData;// analysisdlgobject->data;
	if (profile.size() <= 0)
		return;
	if (m_hPEl)PEdestroy(m_hPEl);
	RECT wndRect, itemRect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_LINEPROFILE_VIEW)->GetWindowRect(&itemRect);
	itemRect.left -= wndRect.left;
	itemRect.top -= wndRect.top;
	itemRect.right -= wndRect.left;
	itemRect.bottom -= wndRect.top;
	//itemRect.bottom += 20;

	m_hPEl = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &itemRect, m_hWnd, 1001);


	//Set up cursor
	//PEnset(m_hPEl, PEP_nCURSORMODE, PECM_DATACROSS);

	//Set PlottingMethod //
	PEnset(m_hPEl, PEP_nPLOTTINGMETHOD, /*PEGPM_BAR*/PEGPM_POINTSPLUSLINE);

	// Help see data points //
	//PEnset(m_hPEl, PEP_bMARKDATAPOINTS, TRUE);

	// This will allow you to move cursor by clicking data point //
	PEnset(m_hPEl, PEP_bMOUSECURSORCONTROL, TRUE);
	PEnset(m_hPEl, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(m_hPEl, PEP_nHOTSPOTSIZE, 50);

	PEnset(m_hPEl, PEP_bBITMAPGRADIENTMODE, FALSE);
	PEnset(m_hPEl, PEP_nQUICKSTYLE, PEQS_DARK_NO_BORDER);//PEQS_LIGHT_INSET

	////Axis scale
	PEnset(m_hPEl, PEP_nXAXISSCALECONTROL, PEAC_NORMAL);
	PEnset(m_hPEl, PEP_nYAXISSCALECONTROL, PEAC_NORMAL);
	PEnset(m_hPEl, PEP_nZAXISSCALECONTROL, PEAC_NORMAL);



	// Pass Data //
	long  i = 0, j, k;
	long nTotalCnt;
	long Row, Col;
	int rowNumber = static_cast<int>(profile.size());
	int colNumber = static_cast<int>(profile[0].size());
	int dataLength = rowNumber * colNumber;

	int nX1, nX2, nY1, nY2, a1, b1, a2, b2;

	if (depthLine == TRUE) {
		nX1 = multiProfile.back().first.first.first;
		nY1 = multiProfile.back().first.first.second;
		nX2 = multiProfile.back().first.second.first;
		nY2 = multiProfile.back().first.second.second;
	}
	else if (distLine == TRUE) {
		nX1 = multiProfileDist.back().first.first.first;
		nY1 = multiProfileDist.back().first.first.second;
		nX2 = multiProfileDist.back().first.second.first;
		nY2 = multiProfileDist.back().first.second.second;
	}
	int nWidth = 1;

	nTotalCnt = static_cast<long>((long)sqrt((nX2 - nX1 + 1) * (nX2 - nX1 + 1)
		+ (nY2 - nY1 + 1) * (nY2 - nY1 + 1)));
	// build a profile
	float* pProfileXData, * pProfileYData;
	vector<float>dataVal;
	float mxVal = -1e9, mnVal = 1e9;
	//float fMin = 1e20, fMax = -1e20;
	pProfileXData = new float[nTotalCnt];
	pProfileYData = new float[nTotalCnt];

	PEnset(m_hPEl, PEP_nSUBSETS, 1);//nTotalCnt
	PEnset(m_hPEl, PEP_nPOINTS, nTotalCnt);

	/*std::ofstream file("Line Data.csv");
	if (!file.is_open()) {

	}
	file << "Xval" << "," << "LineData" << std::endl;*/

	double Di = sqrt(pow(nX2 - nX1, 2) + pow(nY2 - nY1, 2));
	double Dr = sqrt(pow((nX2 - nX1) * m_xStep, 2) + pow((nY2 - nY1) * m_yStep, 2));
	double scale = Dr / Di;
	int cnt = 0;
	for (i = 0; i < nTotalCnt; i++) {
		pProfileYData[i] = 0;
		k = 0;
		for (j = -nWidth / 2; j < -nWidth / 2 + nWidth; j++)
		{
			Row = (int)(nY1 + 1.0 * (nY2 - nY1 + 1) * i / nTotalCnt
				+ 1.0 * j * (nX2 - nX1 + 1) / nTotalCnt);
			Col = (int)(nX1 + 1.0 * (nX2 - nX1 + 1) * i / nTotalCnt
				- 1.0 * j * (nY2 - nY1 + 1) / nTotalCnt);
			if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber))
			{
				pProfileYData[i] += profile[Row][Col];
				k++; cnt++;
			}
		}
		if (k > 1)
		{
			pProfileYData[i] /= k;

		}
		if (pProfileYData[i] > mxVal) {
			mxVal = pProfileYData[i];
		}
		if (pProfileYData[i] < mnVal) {
			mnVal = pProfileYData[i];
		}
		dataVal.push_back(pProfileYData[i]);
		float xval = (static_cast<float>(i)) * scale;
		CString xAxisVal;
		xAxisVal.Format(_T("%0.2lf"), xval);
		//TCHAR* xAxis;
		//xAxis = _tcsdup(xAxisVal);
		//PEvsetcell(m_hPEl, PEP_szaPOINTLABELS, i, TEXT(xAxis));
		PEvsetcell(m_hPEl, PEP_szaPOINTLABELS, i, (void*)(LPCTSTR)xAxisVal);

	}
	applyDespike(pProfileYData,cnt);// 20250916
	//file.close();
	//PEvset(m_hPEl, PEP_faXDATA, pProfileXData, nTotalCnt);
	PEvset(m_hPEl, PEP_faYDATA, pProfileYData, nTotalCnt);

	// Manually Control Y Axis //
	/*PEnset(m_hPEl, PEP_nMANUALSCALECONTROLY, PEMSC_MINMAX);
	double d;
	if (mxVal < 0.0) {
		d = static_cast<double>(mnVal + (mnVal / 2.0));
		PEvset(m_hPEl, PEP_fMANUALMINY, &d, 1);
		d = static_cast<double>(mxVal + abs(mxVal) / 2.0);
		PEvset(m_hPEl, PEP_fMANUALMAXY, &d, 1);
	}
	else {
		d = static_cast<double>(mnVal - (abs(mnVal) / 2.0));
		PEvset(m_hPEl, PEP_fMANUALMINY, &d, 1);
		d = static_cast<double>(mxVal + (mxVal / 2.0));
		PEvset(m_hPEl, PEP_fMANUALMAXY, &d, 1);

	}*/

	// Main title and sub title
	TCHAR mainTitle[] = TEXT("|Line Profile|");
	TCHAR subTitle[] = TEXT("||");
	PEszset(m_hPEl, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPEl, PEP_szSUBTITLE, subTitle);
	// Axis label
	TCHAR szBufZ[] = TEXT("Height um");
	PEszset(m_hPEl, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufX[] = TEXT("X um");
	PEszset(m_hPEl, PEP_szXAXISLABEL, szBufX);

	//subset labels 
	// 20251118/Fahim/ typecast
	if (isDepth)PEvsetcell(m_hPEl, PEP_szaSUBSETLABELS, 0, (void*)TEXT("Height Difference"));
	else PEvsetcell(m_hPEl, PEP_szaSUBSETLABELS, 0, (void*)TEXT("Distance"));

	// subset colors, line type, point type
	DWORD dwArray[1] = { PERGB(255,5,249,253) };
	int nLineTypes[] = { PELT_THICKSOLID };
	int nPointTypes[] = { PEPT_DOTSOLID };

	PEvsetEx(m_hPEl, PEP_dwaSUBSETCOLORS, 0, 1, dwArray, 0);
	PEvset(m_hPEl, PEP_naSUBSETLINETYPES, nLineTypes, 1);
	PEvset(m_hPEl, PEP_naSUBSETPOINTTYPES, nPointTypes, 1);

	if (pProfileXData)
		delete[] pProfileXData;
	if (pProfileYData)
		delete[] pProfileYData;

	// v9 features 
	PEnset(m_hPEl, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPEl, PEP_nCURSORPROMPTSTYLE, PECPS_YVALUE);
	PEnset(m_hPEl, PEP_nCURSORPROMPTLOCATION, PECPL_TRACKING_TOOLTIP);

	PEnset(m_hPEl, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);
	PEnset(m_hPEl, PEP_bTRACKINGCUSTOMOTHERTEXT, TRUE);

	PEnset(m_hPEl, PEP_bALLOWTITLEHOTSPOTS, FALSE);
	PEnset(m_hPEl, PEP_bALLOWSUBSETHOTSPOTS, TRUE);
	PEnset(m_hPEl, PEP_bALLOWAXISLABELHOTSPOTS, FALSE);
	PEnset(m_hPEl, PEP_bALLOWGRIDNUMBERHOTSPOTSY, FALSE);
	PEnset(m_hPEl, PEP_bALLOWGRIDNUMBERHOTSPOTSX, FALSE);

	PEnset(m_hPEl, PEP_bMOUSECURSORCONTROLCLOSESTPOINT, TRUE);
	PEnset(m_hPEl, PEP_bALLOWDATAHOTSPOTS, TRUE);

	PEnset(m_hPEl, PEP_bMOUSECURSORCONTROL, TRUE);
	PEnset(m_hPEl, PEP_nRENDERENGINE, PERE_DIRECT2D);

	//bar drawing

	PEnset(m_hPEl, PEP_bSHOWANNOTATIONS, TRUE);

	if (lowThresh != -1 && highThresh != -1) {
		int nAIF = PEAIF_IN_FRONT;
		int nVTypes[] = { PELT_MEDIUMSOLID,PELT_MEDIUMSOLID };
		DWORD dwArray[] = { PERGB(255,255,0,0),PERGB(255,255,0,0) };
		double fXLoc[] = { lowThresh,highThresh };

		PEvset(m_hPEl, PEP_naVERTLINEANNOTATIONTYPE, nVTypes, 2);
		PEvset(m_hPEl, PEP_faVERTLINEANNOTATION, fXLoc, 2);
		PEvset(m_hPEl, PEP_dwaVERTLINEANNOTATIONCOLOR, dwArray, 2);
		PEvsetcell(m_hPEl, PEP_naVERTLINEANNOTATIONINFRONT, 0, &nAIF);

		CString low, high;
		TCHAR* lowBar, * highBar;
		int annotationCnt = 0, NoBold = 1;
		low.Format(_T("|t %0.1lf  "), lowThresh);
		lowBar = _tcsdup(low);
		PEvsetcell(m_hPEl, PEP_szaHORZLINEANNOTATIONTEXT, annotationCnt, lowBar);
		high.Format(_T("|t %0.1lf  "), highThresh);
		highBar = _tcsdup(high);
		annotationCnt++;
		PEvsetcell(m_hPEl, PEP_szaHORZLINEANNOTATIONTEXT, annotationCnt, highBar);

		// line from low bar to high bar for width================
		annotationCnt++;
		double x1d = (static_cast<double>(lowThresh));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1d);
		double y1d = (static_cast<double>(highY));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1d);
		int symbol = PEGAT_THICKSOLIDLINE;
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
		DWORD col2 = PERGB(255, 255, 255, 255);
		PEvsetcell(m_hPEl, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
		// 20251118
		PEvsetcell(m_hPEl, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);

		annotationCnt++;
		double x2d = (static_cast<double>(highThresh));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2d);
		double y2d = (static_cast<double>(highY));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2d);
		symbol = PEGAT_LINECONTINUE;
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
		col2 = 0;
		PEvsetcell(m_hPEl, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
		// 20251118 / Fahim/ typecast
		PEvsetcell(m_hPEl, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));
		//========================

		// width ===============================
		annotationCnt++;

		double xPos = (lowThresh)+(highThresh - lowThresh) / 2;
		double yPos = highY + 1;
		double widthDiff = (abs(lowThresh - highThresh)) * m_xStep;
		CString width;
		width.Format(_T("W:%0.2lf um"/* X = % d Y = % d */), widthDiff/*, x1 * 10, y1 * 10 /** 1000.0F*/);

		TCHAR* widthVal;
		widthVal = _tcsdup(width);

		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONX, annotationCnt, &xPos);
		//double yPos = (static_cast<double>((y1 + 1))) * 10;
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONY, annotationCnt, &yPos);
		symbol = PEGAT_NOSYMBOL;//PEGAT_LARGEDOTSOLID;// PEGAT_POINTER_ARROW_LARGE;
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
		col2 = PERGB(255, 100, 255, 100);
		PEvsetcell(m_hPEl, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
		PEvsetcell(m_hPEl, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, widthVal);
		symbol = TRUE;
		PEnset(m_hPEl, PEP_nGRAPHANNOTATIONTEXTSIZE, 150);
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONSHADOW, annotationCnt, &symbol);
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);
		//======================================

		/// height diff =======================
		annotationCnt++;
		x1d = (static_cast<double>(highThresh));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1d);
		y1d = (static_cast<double>(lowY));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1d);
		symbol = PEGAT_THICKSOLIDLINE;
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
		col2 = PERGB(255, 255, 255, 255);
		PEvsetcell(m_hPEl, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
		PEvsetcell(m_hPEl, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);

		annotationCnt++;
		x2d = (static_cast<double>(highThresh));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2d);
		y2d = (static_cast<double>(highY));
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2d);
		symbol = PEGAT_LINECONTINUE;
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
		col2 = 0;
		PEvsetcell(m_hPEl, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
		PEvsetcell(m_hPEl, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));


		annotationCnt++;
		xPos = highThresh + 2;
		yPos = lowY + (highY - lowY) / 2;
		double heighDiff = (abs(lowY - highY));
		CString depth;
		depth.Format(_T("D:%0.2lf um"/* X = % d Y = % d */), heighDiff/*, x1 * 10, y1 * 10 /** 1000.0F*/);

		TCHAR* depthVal;
		depthVal = _tcsdup(depth);

		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONX, annotationCnt, &xPos);
		//double yPos = (static_cast<double>((y1 + 1))) * 10;
		PEvsetcell(m_hPEl, PEP_faGRAPHANNOTATIONY, annotationCnt, &yPos);
		symbol = PEGAT_NOSYMBOL;//PEGAT_LARGEDOTSOLID;// PEGAT_POINTER_ARROW_LARGE;
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
		col2 = PERGB(255, 100, 255, 100);
		PEvsetcell(m_hPEl, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
		PEvsetcell(m_hPEl, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, depthVal);
		symbol = TRUE;
		PEnset(m_hPEl, PEP_nGRAPHANNOTATIONTEXTSIZE, 150);
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONSHADOW, annotationCnt, &symbol);
		PEvsetcell(m_hPEl, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);
		//==========================================
	}

}

void AnalysisDlg::Histogram(std::map<float, int> freqCnt, float lower = -1, float upper = -1) {
	if (m_hPEh)PEdestroy(m_hPEh);
	RECT wndRect, itemRect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_HISTOGRAM_VIEW)->GetWindowRect(&itemRect);
	itemRect.left -= wndRect.left;
	itemRect.top -= wndRect.top;
	itemRect.right -= wndRect.left;
	itemRect.bottom -= wndRect.top;

	m_hPEh = PEcreate(PECONTROL_GRAPH, WS_VISIBLE, &itemRect, m_hWnd, 1001);
	// Set up cursor //
	//PEnset(m_hPEh, PEP_nCURSORMODE, PECM_POINT);// PECM_POINT PECM_FLOATINGXY

	// This will allow you to move cursor by clicking data point //
	PEnset(m_hPEh, PEP_bMOUSECURSORCONTROL, TRUE);
	PEnset(m_hPEh, PEP_bALLOWDATAHOTSPOTS, TRUE);//TRUE
	//PEnset(m_hPEh, PEP_bALLOWGRAPHHOTSPOTS, TRUE);

	//Set PlottingMethod //
	PEnset(m_hPEh, PEP_nPLOTTINGMETHOD, PEGPM_BAR);

	////Axis scale
	PEnset(m_hPEh, PEP_nXAXISSCALECONTROL, PEAC_NORMAL);
	PEnset(m_hPEh, PEP_nYAXISSCALECONTROL, PEAC_NORMAL);
	PEnset(m_hPEh, PEP_nZAXISSCALECONTROL, PEAC_NORMAL);

	// Axis label
	TCHAR szBufX[] = TEXT("Height um");
	PEszset(m_hPEh, PEP_szXAXISLABEL, szBufX);
	TCHAR szBufZ[] = TEXT("Frequency Count");
	PEszset(m_hPEh, PEP_szYAXISLABEL, szBufZ);

	//// Enable mouse dragging //
	PEnset(m_hPEh, PEP_bMOUSEDRAGGINGX, TRUE);
	PEnset(m_hPEh, PEP_bMOUSEDRAGGINGY, TRUE);

	// Enable Bar Glass Effect //
	PEnset(m_hPEh, PEP_bBARGLASSEFFECT, TRUE);

	// Pass Data //

	long  index = 0, e;
	long nTotalCnt;
	e = static_cast<long>(0.0F);

	nTotalCnt = static_cast<long>(freqCnt.size());
	float* pMyXData = new float[nTotalCnt + 200];
	TCHAR* xAxislabel;
	PEnset(m_hPEh, PEP_nSUBSETS, 1);//nTotalCnt
	PEnset(m_hPEh, PEP_nPOINTS, nTotalCnt + 200);
	std::map<float, int>::iterator it = freqCnt.begin();
	float mxVal = 0, mxCnt = 0;

	//dummy values at front 
	for (index = 0; index < 100; ++index) {
		float fy;
		float fx;
		fx = 0.0F;
		fy = 0.0F;
		pMyXData[index] = fx;
		CString temp;
		temp.Format(_T("%0.3lf"), fx);
		xAxislabel = _tcsdup(temp);
		PEvsetcell(m_hPEh, PEP_szaPOINTLABELS, index, xAxislabel);//PEP_szaPOINTLABELS   PEP_faXDATA
		PEvsetcell(m_hPEh, PEP_faYDATA, index, &fy);
		index++;
		delete[]xAxislabel;
	}

	//actual values 
	for (index = 100; it != freqCnt.end(); it++) {
		float fy;
		float fx;
		fx = it->first;
		fy = static_cast<float>(it->second);
		pMyXData[index] = fx;
		CString temp;
		temp.Format(_T("%0.3lf"), it->first);
		xAxislabel = _tcsdup(temp);
		PEvsetcell(m_hPEh, PEP_szaPOINTLABELS, index, xAxislabel);//PEP_szaPOINTLABELS   PEP_faXDATA
		PEvsetcell(m_hPEh, PEP_faYDATA, index, &fy);
		index++;
		delete[]xAxislabel;
	}

	//dummy values at end 
	for (index; index < nTotalCnt + 200; ++index) {
		float fy;
		float fx;
		fx = 0.0F;
		fy = 0.0F;
		pMyXData[index] = fx;
		CString temp;
		temp.Format(_T("%0.3lf"), fx);
		xAxislabel = _tcsdup(temp);
		PEvsetcell(m_hPEh, PEP_szaPOINTLABELS, index, xAxislabel);//PEP_szaPOINTLABELS   PEP_faXDATA
		PEvsetcell(m_hPEh, PEP_faYDATA, index, &fy);
		index++;
		delete[]xAxislabel;
	}
	PEvset(m_hPEh, PEP_faXDATA, pMyXData, nTotalCnt + 200);
	if (pMyXData)
		delete[]pMyXData;

	// Main title and sub title
	TCHAR mainTitle[] = TEXT("|Height Distribution|");
	TCHAR subTitle[] = TEXT("");
	TCHAR subSubTitle[] = TEXT("");
	PEszset(m_hPEh, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPEh, PEP_szSUBTITLE, subTitle);
	PEvsetcell(m_hPEh, PEP_szaMULTISUBTITLES, 0, subSubTitle);

	PEnset(m_hPEh, PEP_nRENDERENGINE, PERE_DIRECT2D);

	// line Annotation
	PEnset(m_hPEh, PEP_bSHOWANNOTATIONS, TRUE);
	// Increase line annotation text size //
	PEnset(m_hPEh, PEP_nLINEANNOTATIONTEXTSIZE, 150);
	if (lower != -1 && upper != -1) {
		int nAIF = PEAIF_IN_FRONT;
		int nVTypes[] = { PELT_MEDIUMSOLID,PELT_MEDIUMSOLID };
		DWORD dwArray[] = { PERGB(255,255,0,0),PERGB(255,255,0,0) };
		double fXLoc[] = { lower,upper };

		PEvset(m_hPEh, PEP_naVERTLINEANNOTATIONTYPE, nVTypes, 2);
		PEvset(m_hPEh, PEP_faVERTLINEANNOTATION, fXLoc, 2);
		PEvset(m_hPEh, PEP_dwaVERTLINEANNOTATIONCOLOR, dwArray, 2);
		PEvsetcell(m_hPEh, PEP_naVERTLINEANNOTATIONINFRONT, 0, &nAIF);

		CString low, high;
		TCHAR* lowBar, * highBar;
		low.Format(_T("|t %0.1lf  "), lowThresh);
		lowBar = _tcsdup(low);
		PEvsetcell(m_hPEh, PEP_szaVERTLINEANNOTATIONTEXT, 0, lowBar);
		high.Format(_T("|t %0.1lf  "), highThresh);
		highBar = _tcsdup(high);
		PEvsetcell(m_hPEh, PEP_szaVERTLINEANNOTATIONTEXT, 1, highBar);
	}


	/*PEnset(m_hPEh, PEP_nIMAGEADJUSTRIGHT, 100);
	PEszset(m_hPEh, PEP_szTOPMARGIN, TEXT("X"));*/
}

void AnalysisDlg::drawLineOn2D(float x1, float y1, float x2, float y2, DWORD color, int annotationCnt/*, LPARAM lParam*/) {

	int symbol, NoBold = 1;
	symbol = PEGAT_THINSOLIDLINE;
	/*symbol = TRUE;
	PEnset(m_hPE2, PEP_nGRAPHANNOTATIONTEXTSIZE, 150);
	PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONSHADOW, annotationCnt, &symbol);*/
	// Draw annotation Line 
	PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1);
	PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1);
	PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
	PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &color);
	PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));

	annotationCnt++;
	//symbol = PEGAT_LINECONTINUE;
	//col2 = 0;

	PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2);
	PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2);
	PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
	PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &color);
	PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));
}

void AnalysisDlg::Create2D() {
	if (m_hPE2)PEdestroy(m_hPE2);
	RECT wndRect, itemRect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_2D_VIEW)->GetWindowRect(&itemRect);
	itemRect.left -= wndRect.left;
	itemRect.top -= wndRect.top;
	itemRect.right -= wndRect.left;
	itemRect.bottom -= wndRect.top;
	m_hPE2 = PEcreate(PECONTROL_SGRAPH, WS_VISIBLE, &itemRect, m_hWnd, 1001);//PECONTROL_SGRAPH

	// Enable middle mouse dragging //
	PEnset(m_hPE2, PEP_bMOUSEDRAGGINGX, TRUE);
	PEnset(m_hPE2, PEP_bMOUSEDRAGGINGY, TRUE);

	// Axis label
	TCHAR szBufX[] = TEXT("X (um)");
	PEszset(m_hPE2, PEP_szXAXISLABEL, szBufX);
	TCHAR szBufZ[] = TEXT("Y (um)");
	PEszset(m_hPE2, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufY[10];
	if (isMM)
		_tcscpy(szBufY, TEXT("Z (mm)"));
	else
		_tcscpy(szBufY, TEXT("Z (um)"));
	PEszset(m_hPE2, PEP_szZAXISLABEL, szBufY);

	//Annotation
	PEnset(m_hPE2, PEP_bALLOWANNOTATIONCONTROL, TRUE);
	PEnset(m_hPE2, PEP_nSHOWANNOTATIONTEXTMENU, PEMC_SHOW);
	PEnset(m_hPE2, PEP_bSHOWANNOTATIONTEXT, TRUE);
	// Add some various graph annotations //
	PEnset(m_hPE2, PEP_bSHOWANNOTATIONS, TRUE);
	PEnset(m_hPE2, PEP_nGRAPHANNOTATIONTEXTSIZE, 115);
	// Controls default placement of all annotations //
	PEnset(m_hPE2, PEP_bANNOTATIONSINFRONT, FALSE);

	// Set Contour Color regions //
	PEnset(m_hPE2, PEP_nCONTOURCOLORBLENDS, 4);
	PEnset(m_hPE2, PEP_nCONTOURCOLORSET, PECCS_GREEN_YELLOW_RED);//PECCS_BLUE_GREEN_YELLOW_RED
	PEnset(m_hPE2, PEP_nCONTOURCOLORALPHA, 255);
	PEnset(m_hPE2, PEP_bSHOWLEGEND, TRUE);
	PEnset(m_hPE2, PEP_bCONTOURSTYLELEGEND, TRUE);
	PEnset(m_hPE2, PEP_nLEGENDLOCATION, PELL_LEFT);
	PEnset(m_hPE2, PEP_nCONTOURLEGENDPRECISION, 1);//set the precision of the legend

	long Row, Col;
	int rowNumber = static_cast<int>(filterData.size() - 1);
	//int colNumber = static_cast<int>(filterData[0].size()-2);
	int colNumber = static_cast<int>(filterData[0].size() - 2);

	PEnset(m_hPE2, PEP_nSUBSETS, rowNumber);
	PEnset(m_hPE2, PEP_nPOINTS, colNumber);

	dataLength = rowNumber * colNumber;//ARIF COMMENT

	float* pMyXData = new float[dataLength];
	float* pMyYData = new float[dataLength];
	float* pMyZData = new float[dataLength];

	int pos = 0, isColored;
	for (Row = 0; Row < rowNumber; Row++) {
		for (Col = 0; Col < colNumber; Col++) {
			pMyXData[pos] = (static_cast<float>(Col)) * m_xStep; //X axis
			pMyYData[pos] = (static_cast<float>(Row)) * m_yStep; //Z axis
			pMyZData[pos] = filterData[Row][Col]; //Data for plot
			if (filterData[Row][Col] == 0.00F) {
				/*DWORD color;
				color = PERGB(65, 0, 0, 255);
				isColored = PEvsetcellEx(m_hPE2, PEP_dwaPOINTCOLORS, Row, Col, &color);
				TRACE("%d\n", isColored);*/
				pMyZData[pos] = avgVal;
			}
			pos++;
		}
	}
	//Line Profile starts

	if (!isArea) {
		int pos = 0, i = 0, j = 0;
		int nTotalCnt, cnt = 0;
		int x1, y1, x2, y2, w, annotationCnt = 0;
		PEnset(m_hPE2, PEP_nGRAPHANNOTATIONTEXTSIZE, 700);
		// height difference of two point
		// ARIF ADD Distance also in this mouse event---02192025
		if (isDepth == TRUE) {
			for (int profile = 1; profile < profileCnt; profile++, annotationCnt++) {
				x1 = multiProfile[profile].first.first.first;
				y1 = multiProfile[profile].first.first.second;
				x2 = multiProfile[profile].first.second.first;
				y2 = multiProfile[profile].first.second.second;

				///=============
				double hmx, hmn, mnXPos = 1e9, mnYPos = 1e9, mxYPos = -1e9, mxXPos = -1e9;
				//================

				//height difference of the two selected points
				CString height;
				if (/*twoPointHeight == TRUE*/multiProfile[profile].second) {
					heightDiff = (abs(heightTwoPt[profile - 1].first - heightTwoPt[profile - 1].second));
					/*if (isMM)height.Format("Height diff= %0.2lf mm", heightDiff);
					else height.Format("Height diff= %0.2lf um", heightDiff * 1000.0F);*/
					height.Format(_T("HDtp:%0.2lf um"/* X = % d Y = % d */), heightDiff/*, x1 * 10, y1 * 10 /** 1000.0F*/);

				}
				else
				{
					float mnHeight = 1e9;
					float mxHeight = -1e9;
					w = 1;
					nTotalCnt = static_cast<long>((long)sqrt((x2 - x1 + 1) * (x2 - x1 + 1) + (y2 - y1 + 1) * (y2 - y1 + 1)));
					for (i = 0; i < nTotalCnt; i++)
					{
						for (j = -w / 2; j <= w / 2; j++)
						{
							Row = (int)(y1 + 1.0 * (y2 - y1) * i / nTotalCnt + 1.0 * j * (x2 - x1) / nTotalCnt);
							Col = (int)(x1 + 1.0 * (x2 - x1) * i / nTotalCnt - 1.0 * j * (y2 - y1) / nTotalCnt);
							if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber))
							{
								pos = colNumber * Row + Col;


								float dataVal = pMyZData[pos];
								if (dataVal < mnHeight) {
									mnHeight = dataVal;
									mnXPos = Col;// need to verify
									mnYPos = Row;// need to verify
									hmn = dataVal;
								}

								if (dataVal > mxHeight) {
									mxHeight = dataVal;
									mxXPos = Col;// need to verify
									mxYPos = Row;// need to verify
									hmx = dataVal;
								}
							}
						}
					}
					cnt++;

					heightDiff = (abs(mxHeight - mnHeight));
					//CString height;
					/*if (isMM)height.Format("Height diff= %0.2lf mm", heightDiff);
					else*/ height.Format(_T("HD:%0.2lf um"/* X = % d Y = % d */), heightDiff/*, x1 * 10, y1 * 10 /** 1000.0F*/);
				}

				TCHAR* heightDiffVal;
				heightDiffVal = _tcsdup(height);

				double xPos = min(x1, x2);
				double yPos = min(y1, y2);
				xPos += abs(x1 - x2) / 2;
				yPos += abs(y1 - y2) / 2;

				xPos *= m_xStep;
				yPos *= m_yStep;

				//// start and end point mark ====================================================
				//annotationCnt++;
				int NoBold = 1;
				double x1d = (static_cast<double>(x1)) * m_xStep;
				double y1d = (static_cast<double>(y1)) * m_yStep;
				int symbol = PEGAT_DOTSOLID;
				DWORD col2 = PERGB(255, 0, 0, 0);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT("S"));
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);

				annotationCnt++;
				double x2d = (static_cast<double>(x2)) * m_xStep;
				double y2d = (static_cast<double>(y2)) * m_yStep;
				symbol = PEGAT_DOTSOLID;
				col2 = PERGB(255, 0, 0, 0);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT("E"));


				////===============================================================================

				///=================annotation line ======================================
				annotationCnt++;
				x1d = (static_cast<double>(x1)) * m_xStep;
				y1d = (static_cast<double>(y1)) * m_yStep;
				x2d = (static_cast<double>(x2)) * m_xStep;
				y2d = (static_cast<double>(y2)) * m_yStep;
				symbol = PEGAT_THICKSOLIDLINE;
				col2 = PERGB(255, 255, 255, 255);


				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);

				annotationCnt++;
				symbol = PEGAT_LINECONTINUE;
				col2 = 0;
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));


				///========================================================================
				// HD Value Print ================================
				annotationCnt++;
				symbol = PEGAT_NOSYMBOL;//PEGAT_LARGEDOTSOLID;// PEGAT_POINTER_ARROW_LARGE;
				col2 = PERGB(255, 0, 198, 198);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &xPos);
				//double yPos = (static_cast<double>((y1 + 1))) * 10;
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &yPos);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, heightDiffVal);
				symbol = TRUE;
				PEnset(m_hPE2, PEP_nGRAPHANNOTATIONTEXTSIZE, 150);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONSHADOW, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);
				// ===================================

			}

		}
		//distance between two points

		if (isDist == TRUE) {
			for (int profile = 1; profile < profileCntDist; profile++, annotationCnt++) {
				x1 = multiProfileDist[profile].first.first.first;
				y1 = multiProfileDist[profile].first.first.second;
				x2 = multiProfileDist[profile].first.second.first;
				y2 = multiProfileDist[profile].first.second.second;

				///=============
				double hm, hl;
				//================
				w = 1;
				nTotalCnt = static_cast<long>((long)sqrt((x2 - x1 + 1) * (x2 - x1 + 1) + (y2 - y1 + 1) * (y2 - y1 + 1)));
				for (i = 0; i < nTotalCnt; i++) {
					for (j = -w / 2; j <= w / 2; j++)
					{
						Row = (int)(y1 + 1.0 * (y2 - y1) * i / nTotalCnt + 1.0 * j * (x2 - x1) / nTotalCnt);
						Col = (int)(x1 + 1.0 * (x2 - x1) * i / nTotalCnt - 1.0 * j * (y2 - y1) / nTotalCnt);
						if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber))
						{
							pos = colNumber * Row + Col;
							if (i == nTotalCnt / 2 && j == 0) {
								hm = pMyZData[pos];
							}
							if (i + 1 == nTotalCnt) {
								hl = pMyZData[pos];
							}
							//pMyZData[pos] = 0.0F;
						}
					}
					cnt++;
				}
				distance = static_cast<long>((long)sqrt((x2 - x1 + 1) * (x2 - x1 + 1) + (y2 - y1 + 1) * (y2 - y1 + 1)));

				double Di = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
				double Dr = sqrt(pow((x2 - x1) * m_xStep, 2) + pow((y2 - y1) * m_yStep, 2));
				double scale = Dr / Di;
				distance *= scale;

				CString dist;
				if (isMM)dist.Format(_T("d=%0.4lf um X=%.0lf,Y=%.0lf"), (distance/**1000*/)*m_step, x1 * m_xStep, y1 * m_yStep);
				else dist.Format(_T("d=%0.4lf um X=%.0lf,Y=%.0lf"), (distance/**1000*/)*m_step, x1 * m_xStep, y1 * m_yStep);
				TCHAR* distValue;
				distValue = _tcsdup(dist);

				// distance value print annotation 
				double xPos = min(x1, x2);
				double yPos = min(y1, y2);
				xPos += abs(x1 - x2) / 2;
				yPos += abs(y1 - y2) / 2;

				xPos *= m_xStep;
				yPos *= m_yStep;
				annotationCnt++;
				int NoBold = 1;
				int symbol = PEGAT_NOSYMBOL;//PEGAT_LARGEDOTSOLID;// PEGAT_POINTER_ARROW_LARGE;
				DWORD col2 = PERGB(255, 0, 198, 198);
				symbol = TRUE;

				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &xPos);
				//double yPos = (static_cast<double>((y1 + 1))) * 10;
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &yPos);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, distValue);
				PEnset(m_hPE2, PEP_nGRAPHANNOTATIONTEXTSIZE, 150);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONSHADOW, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);


				// new type annotation
				//// start and end point mark ====================================================
				annotationCnt++;
				double x1d = (static_cast<double>(x1)) * m_xStep;
				double y1d = (static_cast<double>(y1)) * m_yStep;
				symbol = PEGAT_DOTSOLID;
				col2 = PERGB(255, 0, 0, 0);

				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT("S"));
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);

				annotationCnt++;
				double x2d = (static_cast<double>(x2)) * m_xStep;
				double y2d = (static_cast<double>(y2)) * m_yStep;
				symbol = PEGAT_DOTSOLID;
				col2 = PERGB(255, 0, 0, 0);

				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT("E"));

				////===============================================================================

				///=================annotation line ======================================

				// Draw some lines //
				annotationCnt++;
				x1d = (static_cast<double>(x1)) * m_xStep;
				y1d = (static_cast<double>(y1)) * m_yStep;
				symbol = PEGAT_THICKSOLIDLINE;
				col2 = PERGB(255, 255, 255, 255);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x1d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y1d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONBOLD, annotationCnt, &NoBold);

				annotationCnt++;
				x2d = (static_cast<double>(x2)) * m_xStep;
				y2d = (static_cast<double>(y2)) * m_yStep;
				symbol = PEGAT_LINECONTINUE;
				col2 = 0;

				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &x2d);
				PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &y2d);
				PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, (void*)TEXT(""));

			}
		}

		if (isDistMarked == TRUE) {
			//annotationCnt = 0;
			//CString centerPoint;
			//centerPoint.Format("");
			//TCHAR* centerValue;
			//centerValue = _tcsdup(centerPoint);
			//double xCenter = (static_cast<double>(colNumber / 2)) * m_xStep;
			//double yCenter = (static_cast<double>((rowNumber / 2))) * m_yStep;
			//int sym = PEGAT_LARGEPLUS;
			//DWORD colCenter = PERGB(255, 255, 255, 255);

			//PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &xCenter);
			//PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &yCenter);
			//PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &sym);
			//PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &colCenter);
			//PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, centerValue);
			//annotationCnt++;


			//for (int point = 0; point < pointDist.size(); ++point) {
			//	int x = pointDist[point].first;
			//	int y = pointDist[point].second;

			//	float xDistance = (x - colNumber / 2) * m_xStep;
			//	float yDistance = (y - rowNumber / 2) * m_yStep;

			//	CString dist;
			//	if (isMM)dist.Format("X=%0.0lf mm Y=%0.0lf mm", xDistance, yDistance);
			//	else dist.Format("X=%0.0lf um Y=%0.0lf um", xDistance, yDistance);
			//	TCHAR* distValue;
			//	distValue = _tcsdup(dist);
			//	double xPos = (static_cast<double>(x)) * m_xStep;
			//	double yPos = (static_cast<double>((y + 1))) * m_yStep;
			//	DWORD col2 = PERGB(255, 255, 0, 255);
			//	sym = PEGAT_LARGEDOTSOLID;// PEGAT_THINSOLIDLINE;// ;

			//	PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &xPos);
			//	PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &yPos);
			//	PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &sym);
			//	PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
			//	PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, centerValue);

			//	drawLineOn2D(10/*xCenter*/, 20/*yCenter*/, xPos, yPos, col2, ++annotationCnt);


				//PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONX, annotationCnt, &xPos);
				//PEvsetcell(m_hPE2, PEP_faGRAPHANNOTATIONY, annotationCnt, &yPos);
				//int symbol = PEGAT_POINTER_ARROW_SMALL;/*PEGAT_LARGEDOTSOLID;*/
				//PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONTYPE, annotationCnt, &symbol);
				//PEvsetcell(m_hPE2, PEP_dwaGRAPHANNOTATIONCOLOR, annotationCnt, &col2);
				//PEvsetcell(m_hPE2, PEP_szaGRAPHANNOTATIONTEXT, annotationCnt, distValue);
				//symbol = TRUE;
				//PEvsetcell(m_hPE2, PEP_naGRAPHANNOTATIONSHADOW, annotationCnt, &symbol);
			//}
		}
	}
	//Line Profile Ends

	//area profile starts
	else {
		int x1, y1, x2, y2, h, w;
		x1 = m_lineProfileX1;
		y1 = m_lineProfileY1;
		x2 = m_lineProfileX2;
		y2 = m_lineProfileY2;
		w = abs(x1 - x2);
		h = abs(y1 - y2);
		y1 = y1 - h;
		// Top and bottom edges
		for (int i = 0; i < w; i++) {
			// Top edge
			int Row = y1;
			int Col = x1 + i;
			if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber)) {
				pos = colNumber * Row + Col;
				pMyZData[pos] = 0.0F;
			}
			// Bottom edge
			Row = y1 + h - 1;
			Col = x1 + i;
			if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber)) {
				pos = colNumber * Row + Col;
				pMyZData[pos] = 0.0F;
			}
		}
		// Left and right edges
		for (int i = 1; i < h - 1; i++) {
			// Left edge
			int Row = y1 + i;
			int Col = x1;
			if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber)) {
				pos = colNumber * Row + Col;
				pMyZData[pos] = 0.0F;
			}
			// Right edge
			Row = y1 + i;
			Col = x1 + w - 1;
			if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber)) {
				pos = colNumber * Row + Col;
				pMyZData[pos] = 0.0F;
			}
		}
	}
	//area profile ends


	//Perform the actual transfer of data //
	PEvset(m_hPE2, PEP_faXDATA, pMyXData, dataLength);
	PEvset(m_hPE2, PEP_faYDATA, pMyYData, dataLength);//height data
	PEvset(m_hPE2, PEP_faZDATA, pMyZData, dataLength);


	if (pMyXData)
		delete pMyXData;
	if (pMyYData)
		delete pMyYData;
	if (pMyZData)
		delete pMyZData;


	// Set Various Other Properties ///
	PEnset(m_hPE2, PEP_bBITMAPGRADIENTMODE, TRUE);
	PEnset(m_hPE2, PEP_nQUICKSTYLE, PEQS_LIGHT_NO_BORDER);
	PEnset(m_hPE2, PEP_nBORDERTYPES, PETAB_NO_BORDER);

	// This will allow you to move cursor by clicking data point //
	PEnset(m_hPE2, PEP_bMOUSECURSORCONTROL, TRUE);
	PEnset(m_hPE2, PEP_bALLOWDATAHOTSPOTS, TRUE);

	// Set the plotting method //
	PEnset(m_hPE2, PEP_bALLOWCONTOURCOLORS, TRUE);
	PEnset(m_hPE2, PEP_bALLOWCONTOURLINES, TRUE);
	PEnset(m_hPE2, PEP_bALLOWCONTOURCOLORSSHADOWS, TRUE);

	PEnset(m_hPE2, PEP_bCONTOURLINESCOLORED, TRUE);
	PEnset(m_hPE2, PEP_nPLOTTINGMETHOD, PEGPM_CONTOURCOLORS);
	PEnset(m_hPE2, PEP_nDATASHADOWMENU, PEMC_HIDE);

	PEnset(m_hPE2, PEP_bGRIDINFRONT, TRUE);
	PEnset(m_hPE2, PEP_nGRIDLINECONTROL, PEGLC_BOTH);
	PEnset(m_hPE2, PEP_nGRIDSTYLE, PEGS_DOT);
	PEnset(m_hPE2, PEP_bPREPAREIMAGES, TRUE);
	PEnset(m_hPE2, PEP_bCACHEBMP, TRUE);
	PEnset(m_hPE2, PEP_nZOOMSTYLE, PEZS_RO2_NOT);

	// Disable other non contour plotting method //
	PEnset(m_hPE2, PEP_bALLOWLINE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWPOINT, FALSE);
	PEnset(m_hPE2, PEP_bALLOWBAR, FALSE);
	PEnset(m_hPE2, PEP_bALLOWAREA, FALSE);
	PEnset(m_hPE2, PEP_bALLOWSPLINE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWSPLINEAREA, FALSE);
	PEnset(m_hPE2, PEP_bALLOWPOINTSPLUSLINE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWPOINTSPLUSSPLINE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWBESTFITCURVE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWBESTFITLINE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWSTICK, FALSE);

	// Main title and sub title
	TCHAR mainTitle[] = TEXT("");
	TCHAR subTitle[] = TEXT("");
	TCHAR subSubTitle[] = TEXT("");//\n Height[um]
	PEszset(m_hPE2, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPE2, PEP_szSUBTITLE, subTitle);

	// Enable Zooming //
	PEnset(m_hPE2, PEP_nALLOWZOOMING, PEAZ_HORZANDVERT);
	PEnset(m_hPE2, PEP_nMOUSEWHEELFUNCTION, PEMWF_HORZPLUSVERT_ZOOM);
	PEnset(m_hPE2, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPE2, PEP_bGRIDBANDS, FALSE);
	float fZ = 2.00F; PEvset(m_hPE2, PEP_fMOUSEWHEELZOOMFACTOR, &fZ, 1);

	// Set small font size //
	PEnset(m_hPE2, PEP_nFONTSIZE, PEFS_SMALL);
	PEnset(m_hPE2, PEP_bFIXEDFONTS, TRUE);



	// Disable appropriate tabs //
	PEnset(m_hPE2, PEP_bALLOWAXISPAGE, FALSE);
	PEnset(m_hPE2, PEP_bALLOWSTYLEPAGE, TRUE);
	PEnset(m_hPE2, PEP_bALLOWSUBSETSPAGE, FALSE);

	PEnset(m_hPE2, PEP_bSCROLLINGVERTZOOM, TRUE);
	PEnset(m_hPE2, PEP_bSCROLLINGHORZZOOM, TRUE);

	PEnset(m_hPE2, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
	PEnset(m_hPE2, PEP_bMAINTITLEBOLD, TRUE);
	PEnset(m_hPE2, PEP_bSUBTITLEBOLD, TRUE);
	PEnset(m_hPE2, PEP_bLABELBOLD, TRUE);

	// Set export defaults //
	PEnset(m_hPE2, PEP_nDPIX, 600);
	PEnset(m_hPE2, PEP_nDPIY, 600);

	PEnset(m_hPE2, PEP_nEXPORTSIZEDEF, PEESD_NO_SIZE_OR_PIXEL);
	PEnset(m_hPE2, PEP_nEXPORTTYPEDEF, PEETD_PNG);
	PEnset(m_hPE2, PEP_nEXPORTDESTDEF, PEEDD_CLIPBOARD);
	TCHAR exportX[] = TEXT("1280");
	TCHAR exportY[] = TEXT("768");
	PEszset(m_hPE2, PEP_szEXPORTUNITXDEF, exportX);
	PEszset(m_hPE2, PEP_szEXPORTUNITYDEF, exportY);
	PEnset(m_hPE2, PEP_nEXPORTIMAGEDPI, 300);

	PEnset(m_hPE2, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPE2, PEP_nCURSORPROMPTSTYLE, PECPS_ZVALUE);

	// v9 features
	PEnset(m_hPE2, PEP_nCURSORPROMPTLOCATION, 2);
	PEnset(m_hPE2, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);

	// v9 features
	PEnset(m_hPE2, PEP_bGRAPHBMPALWAYS, TRUE);
	PEnset(m_hPE2, PEP_dwGRAPHBACKCOLOR, 5);
	PEnset(m_hPE2, PEP_nGRAPHBMPSTYLE, PEBS_BITBLT_ZOOMING);

	// v9 features

	PEnset(m_hPE2, PEP_nRENDERENGINE, PERE_DIRECT3D);
	PEnset(m_hPE2, PEP_nCOMPOSITE2D3D, PEC2D_FOREGROUND);

	PEnset(m_hPE2, PEP_bFORCE3DXVERTICEREBUILD, TRUE);
	PEnset(m_hPE2, PEP_bFORCE3DXNEWCOLORS, TRUE);

}

void AnalysisDlg::Create3D() {
	if (m_hPE3)PEdestroy(m_hPE3);
	RECT wndRect, itemRect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_3D_VIEW)->GetWindowRect(&itemRect);
	itemRect.left -= wndRect.left;
	itemRect.top -= wndRect.top;
	itemRect.right -= wndRect.left;
	itemRect.bottom -= wndRect.top;

	m_hPE3 = PEcreate(PECONTROL_3D, WS_VISIBLE, &itemRect, m_hWnd, 1001);

	// Enable smoother rotation and zooming //
	PEnset(m_hPE3, PEP_nSCROLLSMOOTHNESS, 6);
	PEnset(m_hPE3, PEP_nMOUSEWHEELZOOMSMOOTHNESS, 4);
	PEnset(m_hPE3, PEP_nPINCHZOOMSMOOTHNESS, 3);


	// Zoom faster //
	float fZF = 10000.0F;
	PEvset(m_hPE3, PEP_fMOUSEWHEELZOOMFACTOR3D, &fZF, 1);
	PEvset(m_hPE3, PEP_fPINCHZOOMFACTOR3D, &fZF, 1);

	// Enable DegreePrompting, to view rotation, zoom, light location to aid
	// in determining different default values for such properties //
	PEnset(m_hPE3, PEP_bDEGREEPROMPTING, TRUE);
	PEnset(m_hPE3, PEP_nROTATIONSPEED, 68);
	PEnset(m_hPE3, PEP_nROTATIONINCREMENT, PERI_INCBY1);
	PEnset(m_hPE3, PEP_nROTATIONDETAIL, PERD_FULLDETAIL);
	PEnset(m_hPE3, PEP_nROTATIONMENU, PEMC_GRAYED);
	PEnset(m_hPE3, PEP_nDEGREEOFROTATION, 180);

	//Set PlottingMethod //
	PEnset(m_hPE3, PEP_nPLOTTINGMETHOD, 4);  //4 Surface with contours
	//PEnset(m_hPE3, PEP_nCPLOTTINGMETHOD, 3);
	PEnset(m_hPE3, PEP_bALLOWSURFACECONTOUR, TRUE);

	//Axis scale
	PEnset(m_hPE3, PEP_nXAXISSCALECONTROL, PEAC_NORMAL);//axis type
	PEnset(m_hPE3, PEP_nYAXISSCALECONTROL, PEAC_NORMAL);//axis type
	PEnset(m_hPE3, PEP_nZAXISSCALECONTROL, PEAC_NORMAL);//axis type

	// Axis label
	TCHAR szBufX[10];
	if (isMM)
		_tcscpy(szBufX, TEXT("X mm"));
	else
		_tcscpy(szBufX, TEXT("X um"));
	PEszset(m_hPE3, PEP_szXAXISLABEL, szBufX);
	TCHAR szBufZ[10];
	if (isMM)
		_tcscpy(szBufZ, TEXT("Z mm"));
	else
		_tcscpy(szBufZ, TEXT("Z um"));
	PEszset(m_hPE3, PEP_szYAXISLABEL, szBufZ);
	TCHAR szBufY[10];
	if (isMM)
		_tcscpy(szBufY, TEXT("Y (mm)"));
	else
		_tcscpy(szBufY, TEXT("Y (um)"));
	PEszset(m_hPE3, PEP_szZAXISLABEL, szBufY);

	// Set a light rotation location //
	Point3D p3d;
	p3d.X = 250.50F; p3d.Y = 50.5F; p3d.Z = 50.25F;
	PEvset(m_hPE3, PEP_struct3DXLIGHT0, &p3d, 1);

	// Set eye/camera distance, or Zoom amount //
	float fMyZoom = -25.0F;
	PEvset(m_hPE3, PEP_f3DXZOOM, &fMyZoom, 1);

	// Enable mouse dragging //
	PEnset(m_hPE3, PEP_bMOUSEDRAGGINGX, TRUE);
	PEnset(m_hPE3, PEP_bMOUSEDRAGGINGY, TRUE);

	// Reduce the terrain height with respect to surface area
	float fX = 10.0F;  PEvset(m_hPE3, PEP_fGRIDASPECTX, &fX, 1);
	float fZ = 10.0F;  PEvset(m_hPE3, PEP_fGRIDASPECTZ, &fZ, 1);
	PEnset(m_hPE3, PEP_n3DXFITCONTROLSHAPE, FALSE);

	// Pass Data //
	long Row, Col, o = 0, e, nQ;
	long nTargetRows, nTargetCols;
	e = static_cast<long>(0.0F);

	nTargetRows = static_cast<long>(filterData.size() - 1);
	nTargetCols = static_cast<long>(filterData[0].size() - 2);

	/*nTargetRows = static_cast<long>(1000);
	nTargetCols = static_cast<long>(1000);*/
	nQ = (nTargetRows - 1) * (nTargetCols - 1);
	float* pMyXData = NULL;
	float* pMyYData = NULL;
	float* pMyZData = NULL;

	pMyXData = new float[nQ]; // v9.5
	pMyYData = new float[nQ];
	pMyZData = new float[nQ]; // v9.5

	long rowcounter, colcounter;
	//BOOL foundtargetindex = FALSE;
	rowcounter = 0;
	colcounter = 0;
	//int cnt = 0;

	for (Row = 0; Row < nTargetRows - 1; Row++) {
		for (Col = 0; Col < nTargetCols - 1; Col++) {
			pMyXData[o] = (static_cast<float>(Col)) * m_xStep;
			pMyZData[o] = (static_cast<float>(Row)) * m_yStep;
			pMyYData[o] = filterData[Row][Col];
			if (filterData[Row][Col] == 0.00) {
				pMyYData[o] = avgVal;
			}
			o++;
		}
	}

	PEnset(m_hPE3, PEP_nSUBSETS, nTargetRows - 1);
	PEnset(m_hPE3, PEP_nPOINTS, nTargetCols - 1);
	// Perform the actual transfer of data //
	PEvset(m_hPE3, PEP_faXDATA, pMyXData, nQ);
	PEvset(m_hPE3, PEP_faYDATA, pMyYData, nQ);
	PEvset(m_hPE3, PEP_faZDATA, pMyZData, nQ);
	PEnset(m_hPE3, PEP_nDUPLICATEXDATA, PEDD_POINTINCREMENT);  // v9.5
	PEnset(m_hPE3, PEP_nDUPLICATEZDATA, PEDD_SUBSETINCREMENT);  // v9.5

	if (pMyXData)
		delete pMyXData;
	if (pMyYData)
		delete pMyYData;
	if (pMyZData)
		delete pMyZData;

	//Color Bar [Legend]
	PEnset(m_hPE3, PEP_bSHOWLEGEND, FALSE);
	PEnset(m_hPE3, PEP_nLEGENDLOCATION, PELL_LEFT);

	//color bar and contour
	PEnset(m_hPE3, PEP_nCONTOURCOLORBLENDS, 4); //8 // this must be set before COLORSET, COLORSET ALWAYS LAST
	PEnset(m_hPE3, PEP_nCONTOURCOLORSET, PECCS_GREEN_YELLOW_RED);//PECCS_BLUE_GREEN_RED //PECCS_BLUE_GREEN_YELLOW_RED
	PEnset(m_hPE3, PEP_nSHOWCONTOUR, PESC_NONE);//PESC_BOTTOMLINES
	PEnset(m_hPE3, PEP_nCONTOURLEGENDPRECISION, 1);//set the precision of the legend

	PEnset(m_hPE3, PEP_nAUTOMINMAXPADDING, 0);
	PEnset(m_hPE3, PEP_bSURFACENULLDATAGAPS, TRUE);
	PEnset(m_hPE3, PEP_bCONTOURSTYLELEGEND, TRUE);//TRUE

	//Set various other properties //
	//font
	PEnset(m_hPE3, PEP_bFIXEDFONTS, TRUE);
	PEnset(m_hPE3, PEP_szLABELFONT, 10);
	PEnset(m_hPE3, PEP_nFONTSIZE, PEFS_MEDIUM);


	// Main title and sub title
	TCHAR mainTitle[] = TEXT("");
	TCHAR subTitle[] = TEXT("");
	TCHAR subSubTitle[] = TEXT("");
	PEszset(m_hPE3, PEP_szMAINTITLE, mainTitle);
	PEszset(m_hPE3, PEP_szSUBTITLE, subTitle);
	PEvsetcell(m_hPE3, PEP_szaMULTISUBTITLES, 0, subSubTitle);

	PEnset(m_hPE3, PEP_bBITMAPGRADIENTMODE, TRUE);//TRUE
	PEnset(m_hPE3, PEP_nQUICKSTYLE, PEQS_LIGHT_LINE);

	PEnset(m_hPE3, PEP_bPREPAREIMAGES, TRUE);
	PEnset(m_hPE3, PEP_bCACHEBMP, TRUE);
	PEnset(m_hPE3, PEP_bFOCALRECT, FALSE);
	PEnset(m_hPE3, PEP_nSHOWBOUNDINGBOX, PESBB_NEVER);
	PEnset(m_hPE3, PEP_nSHADINGSTYLE, PESS_COLORSHADING);

	PEnset(m_hPE3, PEP_nIMAGEADJUSTLEFT, 0);
	PEnset(m_hPE3, PEP_nIMAGEADJUSTRIGHT, 0);
	PEnset(m_hPE3, PEP_nIMAGEADJUSTBOTTOM, 0);

	PEnset(m_hPE3, PEP_nTEXTSHADOWS, PETS_BOLD_TEXT);
	PEnset(m_hPE3, PEP_bLABELBOLD, TRUE);

	//! There are different plotting method values for each case of PolyMode  //
	PEnset(m_hPE3, PEP_bALLOWWIREFRAME, FALSE);
	PEnset(m_hPE3, PEP_nCONTOURMENU, PEMC_HIDE);

	// Set export defaults //
	PEnset(m_hPE3, PEP_nDPIX, 600);
	PEnset(m_hPE3, PEP_nDPIY, 600);
	PEnset(m_hPE3, PEP_nEXPORTSIZEDEF, PEESD_NO_SIZE_OR_PIXEL);
	PEnset(m_hPE3, PEP_nEXPORTTYPEDEF, PEETD_PNG);
	PEnset(m_hPE3, PEP_nEXPORTDESTDEF, PEEDD_CLIPBOARD);
	TCHAR exportX[] = TEXT("1280");
	TCHAR exportY[] = TEXT("768");
	PEszset(m_hPE3, PEP_szEXPORTUNITXDEF, exportX);
	PEszset(m_hPE3, PEP_szEXPORTUNITYDEF, exportY);
	PEnset(m_hPE3, PEP_nEXPORTIMAGEDPI, 300);
	PEnset(m_hPE3, PEP_bALLOWTEXTEXPORT, FALSE);
	PEnset(m_hPE3, PEP_bALLOWEMFEXPORT, FALSE);
	PEnset(m_hPE3, PEP_bALLOWWMFEXPORT, FALSE);
	PEnset(m_hPE3, PEP_bALLOWSVGEXPORT, FALSE);

	PEnset(m_hPE3, PEP_bALLOWDATAHOTSPOTS, TRUE);
	PEnset(m_hPE3, PEP_bCURSORPROMPTTRACKING, TRUE);
	PEnset(m_hPE3, PEP_bTRACKINGCUSTOMDATATEXT, TRUE);
	PEnset(m_hPE3, PEP_nCURSORPROMPTSTYLE, 2);
	PEnset(m_hPE3, PEP_nDATAPRECISION, 2);
	PEnset(m_hPE3, PEP_dw3DHIGHLIGHTCOLOR, PERGB(255, 255, 0, 0));

	PEnset(m_hPE3, PEP_nDATASHADOWMENU, PEMC_SHOW);

	PEnset(m_hPE3, PEP_bALLOWANNOTATIONCONTROL, TRUE);
	PEnset(m_hPE3, PEP_nSHOWANNOTATIONTEXTMENU, PEMC_SHOW);
	PEnset(m_hPE3, PEP_bSHOWANNOTATIONTEXT, TRUE);
	PEnset(m_hPE3, PEP_nLINEANNOTATIONTEXTSIZE, 110);

	double d;  	int t;
	//int aCnt = 0;

	PEnset(m_hPE3, PEP_bSHOWANNOTATIONS, TRUE);
	PEnset(m_hPE3, PEP_bSHOWGRAPHANNOTATIONS, TRUE);
	PEnset(m_hPE3, PEP_bLEFTJUSTIFICATIONOUTSIDE, TRUE);
	PEnset(m_hPE3, PEP_bGRAPHANNOTATIONSYMBOLOBSTACLES, TRUE);
	PEnset(m_hPE3, PEP_nGRAPHANNOTATIONTEXTSIZE, 110);
	PEnset(m_hPE3, PEP_bALLOWGRAPHANNOTHOTSPOTS, FALSE);

	// create a custom menu to toggle PEP_nVIEWINGMODE
	TCHAR customText[] = TEXT("");
	PEvsetcell(m_hPE3, PEP_szaCUSTOMMENUTEXT, 0, customText);
	t = PECMS_CHECKED; PEvsetcellEx(m_hPE3, PEP_naCUSTOMMENUSTATE, 0, 0, &t);
	t = PECML_ABOVE_SEPARATOR; PEvsetcell(m_hPE3, PEP_naCUSTOMMENULOCATION, 0, &t);
	PEvsetcell(m_hPE3, PEP_szaMULTISUBTITLES, 1, customText);

	Point3D p3;  // Set a default ViewingAt location to first graph annotation, index 0
	PEvgetcell(m_hPE3, PEP_faGRAPHANNOTATIONX, 0, &d);  p3.X = (float)d;
	PEvgetcell(m_hPE3, PEP_faGRAPHANNOTATIONY, 0, &d);  p3.Y = (float)d;
	PEvgetcell(m_hPE3, PEP_faGRAPHANNOTATIONZ, 0, &d);  p3.Z = (float)d;
	PEvset(m_hPE3, PEP_structVIEWINGAT, &p3, 1);

	PEnset(m_hPE3, PEP_nRENDERENGINE, PERE_DIRECT3D);
	PEnset(m_hPE3, PEP_bFORCE3DXVERTICEREBUILD, TRUE);
	PEnset(m_hPE3, PEP_bFORCE3DXANNOTVERTICEREBUILD, TRUE);
}

afx_msg LRESULT AnalysisDlg::OnMultiplyx1000(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	isMM = true;
	if (filterData.size()) {
		Create2D();
		Create3D();
		lineProfile();
	}
	return 0;
}

afx_msg LRESULT AnalysisDlg::OnUM(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	isMM = false;
	if (filterData.size()) {
		Create2D();
		Create3D();
		lineProfile();
	}
	return 0;
}

afx_msg LRESULT AnalysisDlg::OnFilterApply(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	if (!data.size())return 0;
	Cfilters filter;
	if (!isThreshed) {
		filterData = data;
	}
	for (int i = 0; i < m_filterOptionPopUp->FilterList.size(); i++) {
		switch (m_filterOptionPopUp->FilterList[i]) {
		case CFilterOption::Smooth0:
			break;
		case CFilterOption::Smooth3x3Q:
			filter.ApplySmoothing(filterData, 3);
			break;
		case CFilterOption::Smooth5x5Q:
			filter.ApplySmoothing(filterData, 5);
			break;
		case CFilterOption::Smooth7x7Q:
			filter.ApplySmoothing(filterData, 7);
			break;
		case CFilterOption::Smooth3x3:
			filter.ApplySmoothing2(filterData, 3);
			break;
		case CFilterOption::Smooth5x5:
			filter.ApplySmoothing2(filterData, 5);
			break;
		case CFilterOption::Smooth7x7:
			filter.ApplySmoothing2(filterData, 7);
			break;
		case CFilterOption::RemoveOutlierF:
			filter.removeOutliers(filterData);
			break;
		case CFilterOption::SMLF:
			//filter.ApplySML(filterData);
			break;
		case CFilterOption::LSF:
			filter.ApplyLSF(filterData);
		case CFilterOption::DESPIKE:
			filter.ApplyDespike(filterData);
		case CFilterOption::FFT:
			filter.ApplyFFT(filterData);
		default:
			break;
		}
	}
	mouseClickCount = 0;
	calcFrequency(filterData);
	//Histogram(freqCnt);
	Create2D();
	Create3D();
	lineProfile();
	return 0;
}

afx_msg LRESULT AnalysisDlg::OnThresholdApply(WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);

	isThreshed = true;
	OnFilterApply(0, 0);
	float m_lower = m_filterOptionPopUp->threshold.first;
	float m_upper = m_filterOptionPopUp->threshold.second;

	int rows = static_cast<int>(filterData.size());
	int cols = static_cast<int>(filterData[0].size());

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			if (filterData[i][j]<m_lower || filterData[i][j]>m_upper) {
				float sum = 0.0F;
				int cnt = 0;
				int row = i, col = j;
				// go left
				while (row - 1 >= 0) {
					row--;
					if (filterData[row][col] <= m_upper && filterData[row][col] >= m_lower) {
						sum += filterData[row][col];
						cnt++;
						break;
					}
				}
				// go right
				row = i;
				while ((row + 1 < rows)) {
					row++;
					if (filterData[row][col] <= m_upper && filterData[row][col] >= m_lower) {
						sum += filterData[row][col];
						cnt++;
						break;
					}
				}
				//go up
				row = i;
				while (col - 1 >= 0) {
					col--;
					if (filterData[row][col] <= m_upper && filterData[row][col] >= m_lower) {
						sum += filterData[row][col];
						cnt++;
						break;
					}
				}
				// go down
				col = j;
				while (col + 1 < cols) {
					col++;
					if (filterData[row][col] <= m_upper && filterData[row][col] >= m_lower) {
						sum += filterData[row][col];
						cnt++;
						break;
					}
				}
				filterData[i][j] = (sum / (float)cnt);
			}
		}
	}

	calcFrequency(filterData);
	//Histogram(freqCnt);
	Create2D();
	Create3D();
	lineProfile();

	return 0;
}

void AnalysisDlg::ShowProfile(int nLeftBorder, int nRightBorder) {
	CDC* dc = NULL;
	CBitmap bmp;
	CDC dcMem;
	CPen pen;
	CPen pen2;
	CPen pen3;
	HBITMAP oldBmp = NULL;
	HPEN oldPen = NULL;
	POINT* m_points;
	size_t		m_pointsCnt;

	RECT wndRect, rect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_LINEPROFILE_VIEW)->GetWindowRect(&rect);
	rect.left -= wndRect.left;
	rect.top -= wndRect.top;
	rect.right -= wndRect.left;
	rect.bottom -= wndRect.top;

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 500;
	rc.bottom = 360;

	dc = GetDC();
	dcMem.CreateCompatibleDC(dc);

	bmp.CreateCompatibleBitmap(dc, rc.right - rc.left + 1, rc.bottom - rc.top + 1);
	oldBmp = (HBITMAP)dcMem.SelectObject(bmp);

	dcMem.FillSolidRect(rc.left, rc.top, rc.right, rc.bottom, m_bgColor);

	pen3.CreatePen(PS_DASH, 1, m_grColor);
	oldPen = (HPEN)dcMem.SelectObject(pen3);
	int i;
	for (i = 1; i <= 3; i++)
	{
		dcMem.MoveTo(0, rc.top + (int)(0.25 * i * (rc.bottom - rc.top + 1)));
		dcMem.LineTo(rc.right, rc.top + (int)(0.25 * i * (rc.bottom - rc.top + 1)));
	}
	for (i = 1; i <= 3; i++)
	{
		dcMem.MoveTo(rc.left + (int)(0.25 * i * (rc.right - rc.left + 1)), 0);
		dcMem.LineTo(rc.left + (int)(0.25 * i * (rc.right - rc.left + 1)), rc.bottom);
	}
	///
	pen2.CreatePen(PS_SOLID, 1, m_grColor2);
	oldPen = (HPEN)dcMem.SelectObject(pen2);
	if (m_fLevel1 > 0)
	{
		dcMem.MoveTo(0, rc.bottom - (int)(m_fLevel1 * (rc.bottom - rc.top + 1)));
		dcMem.LineTo((int)rc.right, rc.bottom - (int)(m_fLevel1 * (rc.bottom - rc.top + 1)));
	}
	if (m_fLevel2 > 0)
	{
		dcMem.MoveTo(0, rc.top + (int)(m_fLevel2 * (rc.bottom - rc.top + 1)));
		dcMem.LineTo(rc.right, rc.top + (int)(m_fLevel2 * (rc.bottom - rc.top + 1)));
	}

	for (i = 1; i < 5; i++)
	{
		if ((m_fProfileMark[i] > 0) && (m_fProfileMark[i] < 1))
		{
			dcMem.MoveTo(rc.left + (int)(m_fProfileMark[i] * (rc.right - rc.left + 1)), rc.top);
			dcMem.LineTo(rc.left + (int)(m_fProfileMark[i] * (rc.right - rc.left + 1)), rc.bottom);
		}
	}
	///
	pen.CreatePen(PS_SOLID, 1, m_lnColor);
	oldPen = (HPEN)dcMem.SelectObject(pen);
	/*	m_points = new POINT[nTotalCnt];
		for (i = 0; i < nTotalCnt; i++)
		{
			m_points[i].x = rc.left + (rc.right - rc.left + 1) * pProfileXData[i] / nTotalCnt;
			m_points[i].y = rc.bottom - (pProfileYData[i] - fMin) * (rc.bottom - rc.top) / (fMax - fMin);
		}*/
	m_points = new POINT[nRightBorder - nLeftBorder];
	for (i = 0; i < nRightBorder - nLeftBorder; i++)
	{
		m_points[i].x = rc.left + (rc.right - rc.left + 1) * pProfileXData[i + nLeftBorder] / (nRightBorder - nLeftBorder);
		m_points[i].y = rc.bottom - (pProfileYData[i + nLeftBorder] - fMin) * (rc.bottom - rc.top) / (fMax - fMin);
	}
	if (m_points)
		//		dcMem.Polyline(m_points, nTotalCnt);
		dcMem.Polyline(m_points, nRightBorder - nLeftBorder);

	if (oldPen) dcMem.SelectObject(oldPen);
	wchar_t str[20];
	for (i = 1; i <= 3; i++)
	{
		swprintf(str, 6, L"%.3f", (fMax - 0.25 * i * (fMax - fMin)));
		CString text(str);
		dcMem.TextOut(0, rc.top + (int)(0.25 * i * (rc.bottom - rc.top + 1)) - 8, text);
	}
	for (i = 1; i <= 3; i++)
	{
		//		swprintf(str, 4, L"%.0f", (0.25 * i * nTotalCnt));
		swprintf(str, 4, L"%.0f", nLeftBorder + (0.25 * i * (nRightBorder - nLeftBorder)));
		CString text(str);
		dcMem.TextOut(rc.left + (int)(0.25 * i * (rc.right - rc.left + 1)) - 20, rc.bottom - 16, text);
	}
	//			
	if (oldPen) dcMem.SelectObject(oldPen);

	dc->BitBlt(rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1, &dcMem, 0, 0, SRCCOPY);

	if (oldBmp) dcMem.SelectObject(oldBmp);

	dcMem.DeleteDC();
	pen.DeleteObject();
	//		pen2.DeleteObject();
	pen3.DeleteObject();
	bmp.DeleteObject();
	ReleaseDC(dc);
	oldBmp = NULL;
	oldPen = NULL;

	dc = NULL;
}

void AnalysisDlg::AdditionalCalculations() {
	UpdateData(TRUE);
	std::vector<std::vector<float>>profile;
	profile.clear();
	profile = filterData;// analysisdlgobject->data;
	if (profile.size() <= 0)
		return;
	RECT wndRect, rect;
	this->GetWindowRect(&wndRect);
	GetDlgItem(IDC_LINEPROFILE_VIEW)->GetWindowRect(&rect);
	rect.left -= wndRect.left;
	rect.top -= wndRect.top;
	rect.right -= wndRect.left;
	rect.bottom -= wndRect.top;

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = 500;
	rc.bottom = 360;

	// Pass Data //
	long  i = 0, j, k;
	long Row, Col;
	int rowNumber = static_cast<int>(profile.size());
	int colNumber = static_cast<int>(profile[0].size());
	int dataLength = rowNumber * colNumber;

	int nX1 = m_lineProfileX1;
	int nX2 = m_lineProfileX2;
	int nY1 = m_lineProfileY1;
	int nY2 = m_lineProfileY2;
	int nWidth = 10;

	nTotalCnt = static_cast<long>((long)sqrt((nX2 - nX1 + 1) * (nX2 - nX1 + 1)
		+ (nY2 - nY1 + 1) * (nY2 - nY1 + 1)));
	nLeftBorder = 0;
	nRightBorder = nTotalCnt - 1;

	if (pProfileXData)
	{
		delete[] pProfileXData;
		pProfileXData = NULL;
	}
	if (pProfileYData)
	{
		delete[] pProfileYData;
		pProfileYData = NULL;
	}
	fMin = 1e20; fMax = -1e20;
	pProfileXData = new float[nTotalCnt];
	pProfileYData = new float[nTotalCnt];

	for (i = 0; i < nTotalCnt; i++) {
		pProfileYData[i] = 0;
		k = 0;
		for (j = -nWidth / 2; j < -nWidth / 2 + nWidth; j++)
		{
			Row = (int)(nY1 + 1.0 * (nY2 - nY1) * i / nTotalCnt
				+ 1.0 * j * (nX2 - nX1) / nTotalCnt);
			Col = (int)(nX1 + 1.0 * (nX2 - nX1) * i / nTotalCnt
				- 1.0 * j * (nY2 - nY1) / nTotalCnt);
			if ((Row >= 0) && (Row < rowNumber) && (Col >= 0) && (Col < colNumber))
			{
				pProfileYData[i] += profile[Row][Col];
				k++;
			}
		}
		if (k > 1)
			pProfileYData[i] /= k;
		if (pProfileYData[i] < fMin)
			fMin = pProfileYData[i];
		if (pProfileYData[i] > fMax)
			fMax = pProfileYData[i];
		pProfileXData[i] = i;
	}

	nProfCnt = nTotalCnt;
	if (pProfile)
		delete[] pProfile;
	pProfile = new float[nProfCnt];
	memcpy((float*)pProfile, (float*)pProfileYData, nTotalCnt * sizeof(float));

	float fLevel1 = fMin + (fMax - fMin) * 0.15;
	float fLevel2 = fMax - (fMax - fMin) * 0.15;
	int i1, i2, i3, i4, i5, i6;
	i = 0;
	m_fProfileMark[0] = 0;
	i1 = 0;

	while ((pProfileYData[i] < fLevel1) && (i < nTotalCnt)) { i++; }
	m_fProfileMark[1] = 1.0 * i / nTotalCnt;
	i2 = i;
	while ((pProfileYData[i] < fLevel2) && (i < nTotalCnt)) { i++; }
	m_fProfileMark[2] = 1.0 * i / nTotalCnt;
	i3 = i;
	while ((pProfileYData[i] > fLevel2) && (i < nTotalCnt)) { i++; }
	m_fProfileMark[3] = 1.0 * i / nTotalCnt;
	i4 = i;
	while ((pProfileYData[i] > fLevel1) && (i < nTotalCnt)) { i++; }
	m_fProfileMark[4] = 1.0 * i / nTotalCnt;
	i5 = i;

	m_fProfileMark[5] = 1;
	i6 = nTotalCnt;

	if (m_bBestfit)
	{
		float S1 = 0, Sx = 0, Sy = 0, Sxx = 0, Sxy = 0;
		for (i = i1; i < i2; i++)
		{
			S1 += 1;
			Sx += pProfileXData[i];
			Sy += pProfileYData[i];
			Sxx += pProfileXData[i] * pProfileXData[i];
			Sxy += pProfileXData[i] * pProfileYData[i];
		}
		for (i = i5; i < i6; i++)
		{
			S1 += 1;
			Sx += pProfileXData[i];
			Sy += pProfileYData[i];
			Sxx += pProfileXData[i] * pProfileXData[i];
			Sxy += pProfileXData[i] * pProfileYData[i];
		}
		float det = Sxx * S1 - Sx * Sx;
		float detk = Sxy * S1 - Sy * Sx;
		float detb = Sxx * Sy - Sx * Sxy;
		if (det == 0)
		{
			m_fk = 0;
			m_fb = 0;
		}
		else
		{
			m_fk = detk / det;
			m_fb = detb / det;
			fMin = 1e20;
			fMax = -1e20;
			for (i = 0; i < nTotalCnt; i++)
			{
				pProfileYData[i] -= (m_fk * pProfileXData[i] + m_fb);
				if (pProfileYData[i] < fMin)
					fMin = pProfileYData[i];
				if (pProfileYData[i] > fMax)
					fMax = pProfileYData[i];
			}
		}
	}
	else
	{
		m_fk = m_fb = 0.0;
	}

	float fSum1 = 0, fSum2 = 0;
	k = 0;
	for (i = i3; i < i4; i++)
	{
		fSum1 += pProfileYData[i];
		k++;
	}
	if (k > 1)
		fSum1 /= k;
	k = 0;
	for (i = i1; i < i2; i++)
	{
		fSum2 += pProfileYData[i];
		k++;
	}
	for (i = i5; i < i6; i++)
	{
		fSum2 += pProfileYData[i];
		k++;
	}
	if (k > 1)
		fSum2 /= k;
	m_fProfileHeight = fSum1 - fSum2;

	ShowProfile(0, nTotalCnt - 1);
}

void AnalysisDlg::showCharts() {

	if (data.size()) {
		((CButton*)GetDlgItem(IDC_ABSDIST))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_HISTOGRAM_CONTROL))->EnableWindow(TRUE);
		multiProfile.push_back({ { {m_lineProfileX1,m_lineProfileY1}, {m_lineProfileX2,m_lineProfileY2} }, 0 });
		multiProfileDist.push_back({ { {m_lineProfileX1,m_lineProfileY1}, {m_lineProfileX2,m_lineProfileY2} },0 });
		profileCnt = 1;
		profileCntDist = 1;
		pointDist.clear();
		Create2D();
		Create3D();
		//Histogram(freqCnt);
		//lineProfile();
	}
}

void AnalysisDlg::readData() {
	data.clear();
	freqCnt.clear();
	multiProfile.clear();
	multiProfileDist.clear();
	heightTwoPt.clear();
	isThreshed = false;
	isMM = false;
	mnVal = 1000.0F;
	mxVal = -10000.0F;
	mouseClickCount = 0;
	dataLength = 0;
	int row = 0, col = 0, mnX = -1, mnY = -1;
	CString FilePath;	//Height Data CSV File Path
	//2D/3D DRAWING BY ARIF IN DIFFERENT FUNCTION -> ARIF

	CFileDialog dlg(TRUE, _T(".csv"), NULL, OFN_FILEMUSTEXIST, _T("CSV Files (*.csv)|*.csv||"), this);
	//dlg.m_ofn.lpstrInitialDir = GetLocalPath() + "DATA\\HeightData";
	if (dlg.DoModal() == IDOK) {
		FilePath = dlg.GetPathName();
		std::ifstream file(FilePath);
		std::string line;

		while (std::getline(file, line)) {
			std::istringstream lineStream(line);
			std::string value;
			std::vector<float>vec;
			col = 0;
			while (std::getline(lineStream, value, ',')) {
				//if (value == "") {
				//	if (col >= 120 && col <= 150)value = "-17.47";//ARIF ADD for Uwe's plot. 
				//	else value = "-54.00";
				//}
				float val;
				val = static_cast<float>(stof(value));
				/*if (val < -(42-10) || val > (42-10)) {
					val = NAN;
				}*/
				vec.push_back(val);
				mxVal = max(mxVal, val);
				if (val < mnVal) {
					mnVal = val;
					mnX = row + 1, mnY = col + 1;
				}

				col++;
			}
			//dataLength += vec.size();
			data.push_back(vec);
			vec.clear();
			row++;

		}
	}

	if (data.size() > 0) {
		//for (int row = 0; row < data.size(); ++row) {
		//	for (int col = 0; col < data[row].size(); ++col) {
		//		/*data[row][col] -= mxVal;
		//		data[row][col] = abs(data[row][col]);
		//		data[row][col] *= 1000;*/
		//		////if (data[row][col] == -100.00)data[row][col] = PEP_bNULLDATAGAPS;
		//		float val = data[row][col];
		//		freqCnt[val]++;
		//	}
		//}
		//applyDespikeVec(data);
		filterData = data;
		avgVal = (mxVal + mnVal) / 2.0;
	}
}




void AnalysisDlg::OnBnClickedDrawChart()
{
	// TODO: Add your control notification handler code here
	readData();
	if (data.size() > 0) {
		if (m_hPEl)PEdestroy(m_hPEl);
		if (m_hPE2)PEdestroy(m_hPE2);
		if (m_hPE3)PEdestroy(m_hPE3);
		if (data.size() > 1)
		{
			showCharts();
		}
		else
		{
			lineProfile(data[0]);
		}

	}
}


void AnalysisDlg::OnBnClickedFilterOptions()
{
	// TODO: Add your control notification handler code here
	if (!m_filterOptionPopUp) {
		m_filterOptionPopUp = new CFilterOption();
		m_filterOptionPopUp->pHWnd = GetSafeHwnd();
		m_filterOptionPopUp->Create(IDD_DLG_FILTERS);
		m_filterOptionPopUp->ShowWindow(SW_SHOW);
		isClicked = true;
		// 20251118 Fahim
	}
	else {
		m_filterOptionPopUp->ShowWindow(SW_SHOW);
		m_filterOptionPopUp->SetFocus();
	}
}

void AnalysisDlg::OnBnClickedHistogramControl()
{
	// TODO: Add your control notification handler code here
	bool histogram = ((CButton*)GetDlgItem(IDC_HISTOGRAM_CONTROL))->GetCheck();
	if (histogram) {
		calcFrequency(filterData);
		Histogram(freqCnt);
	}
	else {
		if (m_hPEh)PEdestroy(m_hPEh);
	}
}

void AnalysisDlg::OnBnClickedAbsdist()
{
	// TODO: Add your control notification handler code here
	if (filterData.size() > 0) {
		isDistMarked = !isDistMarked;
		Create2D();
	}
}

void AnalysisDlg::OnBnClickedRadioMinmax()
{
	twoPointHeight = FALSE;
}

void AnalysisDlg::OnBnClickedRadio2points()
{
	twoPointHeight = TRUE;
}


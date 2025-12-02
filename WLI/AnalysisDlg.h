#pragma once
#include "afxdialogex.h"
#include "SRC/ResizableDialog.h"
#include <SRC/XTabCtrl.h>

#include "FilterOption.h" // 20251711 added by Mohammad Fahim Hossain
#include <utility> 
#include <map>
#include <vector>
#include "MeasurementDlg.h"//20251127

// CSetupProp17Dlg dialog added by Mahedi Kamal 10212024
class CInfoPanelMain;
class CFilterOption;
#define WM_FILTER_APPLY	(WM_USER + 1111)
#define WM_THRESHOLD (WM_USER+1112)
#define WM_MULTIPLYx1000 (WM_USER+1113)
#define WM_UM (WM_USER+1114)

class AnalysisDlg : public CResizableDialog
{

	DECLARE_DYNAMIC(AnalysisDlg)
	HWND m_hPE2, m_hPE3, m_hPEh, m_hPEl, m_hPEa;

public:
	AnalysisDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AnalysisDlg();
	void LocalToRecipe();
	void RecipeToLocal();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANALYSIS_DLG };
#endif

protected:
	//10212024 / ARIFUL ISLAM 2D, 3D plot, Line profile
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg LRESULT OnFilterApply(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMultiplyx1000(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUM(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnThresholdApply(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	std::vector<std::vector<float>>data;
	CXTabCtrl m_Tab;
	static AnalysisDlg* analysisDlgPointer;
	afx_msg void OnBnClickedDrawChart();

	CFilterOption* m_filterOptionPopUp = nullptr;
	float mnVal, mxVal, avgVal;
	int mouseClickCount;
	bool isClicked = 0;
	int dataLength;
	float thresh1, thresh2, lowThresh, highThresh, x1, x2, y1, y2, lowY, highY;
	bool isThreshed;
	bool isMM;
	int m_lineProfileX1, m_lineProfileY1;
	int m_lineProfileX2, m_lineProfileY2;
	float distance;
	float heightDiff;
	int leftButtonClkCnt;
	int profileCnt, profileCntDist, pointDistCnt;
	float height1Val, height2Val;//for two point height diff
	std::vector<std::pair<float, float> > heightTwoPt;
	int m_xStep, m_yStep;
	bool flagDepth;

	// 20251118
	std::pair<float, float>p1, p2;
	BOOL drawProfile;
	BOOL isArea;
	BOOL isHorz;
	BOOL isDual;
	BOOL isDist, isDepth;
	BOOL distLine, depthLine;
	BOOL isDistMarked;


	//[alexander line profile 
	float m_fProfileMark[6];
	float m_fProfileHeight;
	int m_nProfileWidth;
	bool m_bBestfit;
	float m_fk;
	float m_fb;
	float m_fLevel1;
	float m_fLevel2;
	int nProfileType;
	long nProfCnt;
	float* pProfile;
	float* pProfileXData, * pProfileYData;
	long nTotalCnt;
	float fMin, fMax;
	int nLeftBorder, nRightBorder;
	BOOL isCtrlPressed;

	void AdditionalCalculations(/*std::vector<std::vector<float>>& data*/);
	void ShowProfile(int nLeftBorder, int nRightBorder);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//]

	std::vector<std::vector<float> > filterData;
	std::map<float, int>freqCnt;
	std::vector < std::pair<std::pair<std::pair<float, float>, std::pair<float, float>>, bool>>multiProfile; // 0-> min/max, 1-> 2point
	std::vector < std::pair<std::pair<std::pair<float, float>, std::pair<float, float>>, bool>>multiProfileDist; // 0-> min/max, 1-> 2point
	std::vector<std::pair<float, float>>pointDist;

	void readData();
	void showCharts();
	void Create2D();
	void Create3D();
	void lineProfile();
	void lineProfile(std::vector<float>profile);
	void Histogram(std::map<float, int>freqCnt, float lower, float upper);
	void calcFrequency(std::vector<std::vector<float>>& img);
	void applyHistThreshold(float a, float b, std::vector<std::vector<float>>& data);
	BOOL isPointOnLine(std::pair<int, int>p);
	int euclideanDist(int x1, int y1, int x2, int y2);

	afx_msg void OnBnClickedFilterOptions();
	CEdit stepControl;
	float m_step;

	afx_msg void OnBnClickedHistogramControl();
	afx_msg void OnBnClickedAbsdist();
	BOOL twoPointHeight;
	afx_msg void OnBnClickedRadioMinmax();
	afx_msg void OnBnClickedRadio2points();
	void drawLineOn2D(float x1, float y1, float x2, float y2, DWORD color, int annotationCnt/*, LPARAM lParam*/);
	void applyDespike(float* pProfileYData, int sz);//20250916
	void applyDespikeVec(std::vector<std::vector<float>>& data);
	afx_msg LRESULT onUmAnalysisDlg(WPARAM wParam, LPARAM lParam);//20251127
	void showDirect2D3D(CString path);//20251127
	void readDataFromFile(CString fileName);//20251127
	MeasurementDlg measurementDlgPtr;

	//20251124 -----------------
	// State variables
	// Real-time mouse tracking state
	BOOL m_bIsSelectingLine;     // TRUE when waiting for second click
	int m_previewX1, m_previewY1; // First click (grid coordinates)
	int m_previewX2, m_previewY2; // Current mouse position (grid coordinates)

	BOOL isInsidePlot = false;

	// Helper methods
	void CreateTemporaryLineProfileWindow();
	void UpdateLineProfileGraph(int x1, int y1, int x2, int y2);
	void DrawPreviewLineOn2D(int x1, int y1, int x2, int y2);
	void UpdateLineProfileGraph_Recreate(int nX1, int nY1, int nX2, int nY2);
	// 20251124 ------------------
	
	//20251201 ============================
	Cfilters filter;
	std::vector<std::vector<float>>RBFData;
	//=====================================
};

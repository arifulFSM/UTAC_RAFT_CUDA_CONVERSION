#pragma once
#include "SRC/ResizableDialog.h"
#include "CardPanel.h"
#include "IconButton.h"
#include "IconBorderButton.h"
#include "ColorButton.h"
#include "StyleStatic.h"
#include "ObjectiveLensCtrl.h"
#include "ColorSliderCtrl.h"
#include "CustomRadioButton.h"
#include "MyListCtrl.h"
// CAnalysisNewDlg dialog

//Old Analysis Tab's content  //20251210
#include "afxdialogex.h"
#include <SRC/XTabCtrl.h>
#include "FilterOption.h" // 20251711 added by Mohammad Fahim Hossain
#include <utility> 
#include <map>
#include <vector>
#include "MeasurementDlg.h"//20251127

class CInfoPanelMain;
class CFilterOption;
#define WM_FILTER_APPLY	(WM_USER + 1111)
#define WM_THRESHOLD (WM_USER+1112)
#define WM_MULTIPLYx1000 (WM_USER+1113)
#define WM_UM (WM_USER+1114)

class CAnalysisNewDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CAnalysisNewDlg)
	HWND m_hPE2, m_hPE3, m_hPEh, m_hPEl, m_hPEa;

public:
	CAnalysisNewDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAnalysisNewDlg();
	void LocalToRecipe();//20251210
	void RecipeToLocal();//20251210

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANALYSIS_DLG_NEW };
#endif

protected:
	void PopulateList();
	BOOL OnToolButtonClicked(UINT nID);
	//10212024 / ARIFUL ISLAM 2D, 3D plot, Line profile 
	//20251210
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
	CIconButton m_toolButton2Points;
	CIconButton m_toolButtonHorizontalLine;
	CIconButton m_toolButtonVerticalLine;
	CIconButton m_toolButtonLine;
	CIconButton m_toolButtonParallel;
	CIconButton m_toolButtonPerpendicular;
	CIconButton m_toolButtonBox;
	CIconButton m_toolButtonCircle;


private:
	UINT m_nSelectedToolID; // Holds the ID of the currently active button
public:
	CCardPanel m_profileToolCardPanel;
	afx_msg void OnStnClickedProfileCardPanel();
	CCardPanel m_lineRoughnessCardPanel;
	CStyleStatic m_measureTypeText;
	CCustomRadioButton m_MeasurementTypeRadio1;
	CCustomRadioButton m_MeasurementTypeRadio2;
	CCustomRadioButton m_MeasurementTypeRadio3;
	CStyleStatic m_cutoffFilterTitle;
	CStyleStatic m_lamdaLow;
	CStyleStatic m_lamdaHigh;
	CStyleStatic m_microMeter;
	CStyleStatic m_microMeter2;

	CMyListCtrl m_listCtrl;
	afx_msg void OnLvnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);

	//20251210
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
	float m_xStep, m_yStep;
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


	////[alexander line profile 
	//float m_fProfileMark[6];
	//float m_fProfileHeight;
	//int m_nProfileWidth;
	//bool m_bBestfit;
	//float m_fk;
	//float m_fb;
	//float m_fLevel1;
	//float m_fLevel2;
	//int nProfileType;
	long nProfCnt;
	float* pProfile;
	float* pProfileXData, * pProfileYData;
	long nTotalCnt;
	float fMin, fMax;
	int nLeftBorder, nRightBorder;
	BOOL isCtrlPressed;

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//]

	std::vector<std::vector<float> > filterData;
	std::map<float, int>freqCnt;
	std::vector < std::pair<std::pair<std::pair<float, float>, std::pair<float, float>>, bool>>multiProfile; // 0-> min/max, 1-> 2point
	std::vector < std::pair<std::pair<std::pair<float, float>, std::pair<float, float>>, bool>>multiProfileDist; // 0-> min/max, 1-> 2point
	std::vector<std::pair<float, float>>pointDist;

	void loadData();
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
	afx_msg LRESULT onUmAnalysisDlg(WPARAM wParam, LPARAM lParam);//20251127
	void showDirect2D3D(CString path);//20251127
	void readDataFromFile(CString fileName);//20251127
	MeasurementDlg measurementDlgPtr;

	//20251124 / Fahim / -----------------
	// State variables
	// Real-time mouse tracking state
	BOOL m_bIsSelectingLine;     // TRUE when waiting for second click
	int m_previewX1, m_previewY1; // First click (grid coordinates)
	int m_previewX2, m_previewY2; // Current mouse position (grid coordinates)

	BOOL isInsidePlot = false;

	// Helper methods
	void UpdateLineProfileGraph(int x1, int y1, int x2, int y2);
	void DrawPreviewLineOn2D(int x1, int y1, int x2, int y2);
	// 20251124 ------------------

	//20251201 ============================
	Cfilters filter;
	std::vector<std::vector<float>>RBFData;
	//=====================================
	// 20251202 / Fahim / Line Profile update
	// Line drawing state
	int m_lineDrawingState;  // 0=none, 1=drawing line1, 2=drawing line2, 3=both complete
	double m_line1X1, m_line1Y1, m_line1X2, m_line1Y2;  // First line coordinates
	double m_line2X1, m_line2Y1, m_line2X2, m_line2Y2;  // Second line coordinates
	BOOL m_isDrawingCurrentLine;  // TRUE while dragging
	double m_currentLineStartX, m_currentLineStartY;  // Temp storage during drag

	void DrawLineAnnotation(double x1, double y1, double x2, double y2, int startIdx, int endIdx, DWORD color);
	void DrawLineAnnotations();
	void ClearLineAnnotations();
	double CalculateLineMedianX(double x1, double x2);
	void DisplayDistanceBetweenLines();
	//======================================
	// 20251204 / Fahim / Angle measurement distance and width
	BOOL IsLineHorizontal(double angle);
	void DisplayWidthBetweenLines();    // For HORIZONTAL lines 
	double GetPixelAngle(double x1, double y1, double x2, double y2);
	// 20251218 / Fahim / Circle Profiling
	BOOL m_bIsSelectingCircle;
	POINT m_ptCircleCenter;       // Center in Screen Pixels
	int   m_iCircleRadius;        // Radius in Screen Pixels
	void DrawPreviewCircleOn2D(POINT centerPx, int radiusPx); // Draw using Pixel inputs
	void CircleProfile(POINT centerPx, int radiusPx); // Profile using Pixel inputs
	// 20251222
	void ShowCircleProfile(std::vector<float>& profileData);

protected:
	// 20251223 / Fahim / Refactoring the line profile drawing methods
	void InitializeProfileChart();
	void UpdateProfileChart(const std::vector<double>& xData, const std::vector<float>& yData,
		CString mainTitle, CString subTitle,
		CString xAxisLabel, CString yAxisLabel);
	void ResetProfileAnnotations(); // Clears any old annotations (lines, text) from previous tools
	void DeselectActiveTool();
public:
	// 20251223 / Fahim / Horizontal Line Tool
	void DrawPreviewHorizontalLine(double yGraph);
	void HorizontalProfile(double yGraph);
	// 20251223 / Vertical Line Tool
	void DrawPreviewVerticalLine(double xGraph);
	void VerticalProfile(double xGraph);
	// 20251224 / Fahim / Extended Line Tool 
	BOOL m_bIsSelectingExtendedLine;
	double m_dExtendedLineStartX;
	double m_dExtendedLineStartY;
	// Calculates the start/end points of a line passing through (x1,y1)-(x2,y2)  // extended to the data boundaries.
	void CalculateExtendedEndpoints(double x1, double y1, double x2, double y2,
		double& outX1, double& outY1,
		double& outX2, double& outY2);
	void DrawPreviewExtendedLine(double x1, double y1, double x2, double y2);
	void ExtendedLineProfile(double x1, double y1, double x2, double y2);
	// 20251224 / Fahim / Parallel Line Tool 
	int    m_nParallelState; // 0=Idle, 1=Drawing Ref Line, 2=Positioning Parallel Line
	double m_dParaRefX1, m_dParaRefY1; // Reference Start
	double m_dParaRefX2, m_dParaRefY2; // Reference End
	void DrawPreviewParallelTool(double currX, double currY);  // Draws both Reference and Parallel lines depending on state
	void ParallelToolProfile(double currX, double currY);  // Generates profile for either Reference or Parallel line
};

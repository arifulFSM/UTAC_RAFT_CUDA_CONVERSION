#pragma once
#include "afxdialogex.h"
#include "C2DPlotDlg.h"
#include "C3DPlotDlg.h"
#include "SRC/ResizableDialog.h"
#include <SRC/XTabCtrl.h>
#include "ResultRAFT.h"

// AnalysisDlg dialog

class C2DPlotDlg;
class C3DPlotDlg;

class AnalysisDlg : public CResizableDialog
{

	C2DPlotDlg* tdPlotDlg = nullptr;
	C3DPlotDlg* threeDPlotDlg = nullptr;


	DECLARE_DYNAMIC(AnalysisDlg)

public:
	AnalysisDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~AnalysisDlg();
	CResultRAFT* pAanalysis;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANALYSIS_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	std::vector<std::vector<float>>data;
	afx_msg void OnBnClickedButtonLoadData();
	CXTabCtrl m_Tab;
	static AnalysisDlg* analysisDlgPointer;
	afx_msg LRESULT OnUmAnalysisDlg(WPARAM wParam, LPARAM lParam);
};

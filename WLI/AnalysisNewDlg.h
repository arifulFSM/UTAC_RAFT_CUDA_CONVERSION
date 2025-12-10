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

class CAnalysisNewDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(CAnalysisNewDlg)

public:
	CAnalysisNewDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CAnalysisNewDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANALYSIS_DLG_NEW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	BOOL OnInitDialog();

	void setButtonIcon(int size);

	void PopulateList();

	BOOL OnToolButtonClicked(UINT nID);

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
};

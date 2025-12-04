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
// COperationDlg dialog

class COperationDlg : public CResizableDialog
{
	DECLARE_DYNAMIC(COperationDlg)

public:
	COperationDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~COperationDlg();


	BOOL OnInitDialog();

	void setButtonIcon(int size);


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPERATION_DLG };
#endif

protected:

	//CBrush m_brushBack;
	//HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);



	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CCardPanel m_stageMotionPanelCard;
	CIconBorderButton topLeftButton;
	CIconBorderButton upButton;
	CIconBorderButton topRightButton;
	CIconBorderButton leftButton;
	CIconBorderButton homeButton;
	CIconBorderButton rightButton;
	CIconBorderButton bottomLeftButton;
	CIconBorderButton downButton;
	CIconBorderButton bottomRightButton;
	CIconBorderButton zUpButton;
	CIconBorderButton zDownButton;
	CCardPanel autoFocusCardPanel;
	afx_msg void OnNMCustomdrawProgress1(NMHDR* pNMHDR, LRESULT* pResult);
	CCardPanel measurementExecutionPanel;


	CColorButton m_measureButton;
	CColorButton m_stopMeasureButton;
	CStyleStatic m_loadedWaferRecipeName;
	CStyleStatic m_currentZ;
	CStyleStatic m_lowZ;
	CStyleStatic m_highZ;
	CStyleStatic m_currentZValue;
	CStyleStatic m_lowZValue;
	CStyleStatic m_highZValue;
	CColorButton m_runAFButton;
	CStyleStatic m_activeRecipe;
	CStyleStatic m_speedTitle;
	CObjectiveLensCtrl m_lensCtrl;
	CCardPanel m_lensCardPanel;
	afx_msg void OnStnClickedLensCardPanel();
	CStyleStatic m_brightness;
	CStyleStatic m_exposure;
	CStyleStatic m_progress_percentage;
	CColorSliderCtrl m_zValueSlider;
	CColorSliderCtrl m_brightnessSlider;
	CColorSliderCtrl m_exposureSlider;
	CCustomRadioButton m_FineRadioButton;
	CCustomRadioButton m_fastRadioButton;
};

#pragma once
#include "SRC/ResizableDialog.h"
#include "IconButton.h"
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

	BOOL OnToolButtonClicked(UINT nID);

	DECLARE_MESSAGE_MAP()
public:
	CIconButton m_toolButton1;
	CIconButton m_toolButton2;
	CIconButton m_toolButton3;
	CIconButton m_toolButton4;
	CIconButton m_toolButton5;
	CIconButton m_toolButton6;
	CIconButton m_toolButton7;
	CIconButton m_toolButton8;


private:
	UINT m_nSelectedToolID; // Holds the ID of the currently active button
};

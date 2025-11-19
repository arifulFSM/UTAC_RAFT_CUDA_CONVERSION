#pragma once
#include "afxdialogex.h"
#include "AnalysisDlg.h"
#include <map>
#include <iostream>
#include <vector>


// CFilterOption dialog

//10222024 / ARIFUL ISLAM

// 20251118 / added in WLI from UTAC by Mohammad Fahim Hossain

class CFilterOption : public CDialogEx
{
	DECLARE_DYNAMIC(CFilterOption)

public:
	CFilterOption(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFilterOption();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_FILTERS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void Smoothing3();
	afx_msg void Smoothing5();
	afx_msg void Smoothing7();
	afx_msg void RemoveOutlier();
	afx_msg void NoSmooth();
	DECLARE_MESSAGE_MAP()
public:
	enum Filter {
		Smooth0,
		Smooth3x3,
		Smooth5x5,
		Smooth7x7,
		Smooth3x3Q,
		Smooth5x5Q,
		Smooth7x7Q,
		RemoveOutlierF,
		SMLF,
		LSF,
		DESPIKE,
		FFT

	};
	std::vector<int>FilterList;
	void RemoveSmooth();
	void RemoveFilter(int Filter);

	HWND pHWnd;
	virtual BOOL OnInitDialog();
	//static CFilterOption* filterOptionPointer;
	std::vector<std::vector<float>>orgData;
	std::vector<std::vector<float>>filterData;
	std::map<float, int>freqCnt;
	bool outlier, smooth3, smooth5, smooth7, smooth3Q, smooth5Q, smooth7Q, sml, lsf, despike, fft;
	float m_lowThresh, m_highThresh;
	std::pair<float, float>threshold;

	afx_msg void OnBnClickedApplyThreshold();

	BOOL appliedFilters();
	afx_msg void OnBnClickedSml();
	afx_msg void OnBnClickedApplyFilter();
	CEdit m_high;
	CEdit m_low;
	afx_msg void OnBnClickedLsf();
	afx_msg void OnBnClickedSmoothq3();
	afx_msg void OnBnClickedSmoothq5();
	afx_msg void OnBnClickedSmoothq7();
	afx_msg void OnBnClickedMultiply1000();
	afx_msg void OnBnClickedUm();
	afx_msg void OnBnClickedDespike();
	afx_msg void OnBnClickedFft();
};

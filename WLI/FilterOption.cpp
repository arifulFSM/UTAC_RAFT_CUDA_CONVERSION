// FilterOption.cpp : implementation file
//

#include "pch.h"
#include "WLI.h"
#include "afxdialogex.h"
#include "FilterOption.h"
#include "AnalysisDlg.h"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
// CFilterOption dialog
#include "Resource.h"
#include "resource.h"

//10222024 / ARIFUL ISLAM
// 20251118 / added in WLI from UTAC by Mohammad Fahim Hossain


// CFilterOption dialog

IMPLEMENT_DYNAMIC(CFilterOption, CDialogEx)

CFilterOption::CFilterOption(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_FILTERS, pParent)
{

}

CFilterOption::~CFilterOption()
{
}

BOOL CFilterOption::OnInitDialog() {
	CDialogEx::OnInitDialog();
	__super::OnInitDialog();
	//filterOptionPointer = this;
	/*((CButton*)GetDlgItem(IDC_REMOVE_OUTLIER))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SML))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SMOOTH3))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SMOOTH5))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_SMOOTH7))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_NO_SMOOTH))->SetCheck(TRUE);*/
	//GetDlgItem(IDC_APPLY_THRESHOLD)->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_NO_SMOOTH))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_MULTIPLY1000))->SetCheck(FALSE);
	((CButton*)GetDlgItem(IDC_UM))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_SML))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_DESPIKE))->ShowWindow(SW_HIDE);
	//m_lowThresh = mnVal;
	//m_highThresh = mxVal;

	return true;
}


void CFilterOption::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_LOW, m_lowThresh);
	DDX_Text(pDX, IDC_HIGH, m_highThresh);
	DDX_Control(pDX, IDC_HIGH, m_high);
	DDX_Control(pDX, IDC_LOW, m_low);
}


BEGIN_MESSAGE_MAP(CFilterOption, CDialogEx)
	ON_BN_CLICKED(IDC_NO_SMOOTH, &CFilterOption::NoSmooth)
	ON_BN_CLICKED(IDC_SMOOTH3, &CFilterOption::Smoothing3)
	ON_BN_CLICKED(IDC_SMOOTH5, &CFilterOption::Smoothing5)
	ON_BN_CLICKED(IDC_SMOOTH7, &CFilterOption::Smoothing7)
	ON_BN_CLICKED(IDC_REMOVE_OUTLIER, &CFilterOption::RemoveOutlier)
	ON_BN_CLICKED(IDC_APPLY_THRESHOLD, &CFilterOption::OnBnClickedApplyThreshold)
	ON_BN_CLICKED(IDC_SML, &CFilterOption::OnBnClickedSml)
	ON_BN_CLICKED(IDC_APPLY_FILTER, &CFilterOption::OnBnClickedApplyFilter)
	ON_BN_CLICKED(IDC_LSF, &CFilterOption::OnBnClickedLsf)
	ON_BN_CLICKED(IDC_SMOOTHQ3, &CFilterOption::OnBnClickedSmoothq3)
	ON_BN_CLICKED(IDC_SMOOTHQ5, &CFilterOption::OnBnClickedSmoothq5)
	ON_BN_CLICKED(IDC_SMOOTHQ7, &CFilterOption::OnBnClickedSmoothq7)
	ON_BN_CLICKED(IDC_MULTIPLY1000, &CFilterOption::OnBnClickedMultiply1000)
	ON_BN_CLICKED(IDC_UM, &CFilterOption::OnBnClickedUm)
	ON_BN_CLICKED(IDC_DESPIKE, &CFilterOption::OnBnClickedDespike)
	ON_BN_CLICKED(IDC_FFT, &CFilterOption::OnBnClickedFft)
END_MESSAGE_MAP()


// CFilterOption message handlers

BOOL CFilterOption::appliedFilters() {
	outlier = ((CButton*)GetDlgItem(IDC_REMOVE_OUTLIER))->GetCheck();
	smooth3 = ((CButton*)GetDlgItem(IDC_SMOOTH3))->GetCheck();
	smooth5 = ((CButton*)GetDlgItem(IDC_SMOOTH5))->GetCheck();
	smooth7 = ((CButton*)GetDlgItem(IDC_SMOOTH7))->GetCheck();
	smooth3Q = ((CButton*)GetDlgItem(IDC_SMOOTHQ3))->GetCheck();
	smooth5Q = ((CButton*)GetDlgItem(IDC_SMOOTHQ5))->GetCheck();
	smooth7Q = ((CButton*)GetDlgItem(IDC_SMOOTHQ7))->GetCheck();
	sml = ((CButton*)GetDlgItem(IDC_SML))->GetCheck();

	if (!outlier && !((smooth3 || smooth5 || smooth7) && (smooth3Q || smooth5Q || smooth7Q)) || !sml) {
		return 0;
	}
	else {
		return 1;
	}
}

void CFilterOption::NoSmooth() {
	// TODO: Add your control notification handler code here
	RemoveSmooth();
	FilterList.push_back(Smooth0);
}

void CFilterOption::Smoothing3() {
	RemoveSmooth();
	FilterList.push_back(Smooth3x3);
}

void CFilterOption::Smoothing5() {
	RemoveSmooth();
	FilterList.push_back(Smooth5x5);
}

void CFilterOption::Smoothing7() {
	RemoveSmooth();
	FilterList.push_back(Smooth7x7);
}


void CFilterOption::OnBnClickedSmoothq3()
{
	RemoveSmooth();
	FilterList.push_back(Smooth3x3Q);
}
void CFilterOption::OnBnClickedSmoothq5()
{
	RemoveSmooth();
	FilterList.push_back(Smooth5x5Q);
}


void CFilterOption::OnBnClickedSmoothq7()
{
	RemoveSmooth();
	FilterList.push_back(Smooth7x7Q);
}

void CFilterOption::RemoveSmooth() {
	for (int i = 0; i < FilterList.size(); i++) {
		switch (FilterList[i]) {
		case Smooth0:
			//remove from filter list
			FilterList.erase(FilterList.begin() + i);
			break;
		case Smooth3x3:
			FilterList.erase(FilterList.begin() + i);
			break;
		case Smooth5x5:
			FilterList.erase(FilterList.begin() + i);
			break;
		case Smooth7x7:
			FilterList.erase(FilterList.begin() + i);
			break;
		case Smooth3x3Q:
			FilterList.erase(FilterList.begin() + i);
			break;
		case Smooth5x5Q:
			FilterList.erase(FilterList.begin() + i);
			break;
		case Smooth7x7Q:
			FilterList.erase(FilterList.begin() + i);
			break;
		default:
			break;
		}
	}
}

void CFilterOption::RemoveFilter(int Filter) {
	for (int i = 0; i < FilterList.size(); i++) {
		if (Filter == FilterList[i]) {
			FilterList.erase(FilterList.begin() + i);
			return;
		}
	}
}

void CFilterOption::RemoveOutlier() {
	outlier = ((CButton*)GetDlgItem(IDC_REMOVE_OUTLIER))->GetCheck();
	if (outlier) {
		FilterList.push_back(RemoveOutlierF);
	}
	else {
		RemoveFilter(RemoveOutlierF);
	}
}

void CFilterOption::OnBnClickedApplyThreshold() {
	CString low, high;
	m_low.GetWindowText(low);
	m_high.GetWindowText(high);
	m_lowThresh = static_cast<float>(_ttof(low));
	m_highThresh = static_cast<float>(_ttof(high));
	threshold.first = m_lowThresh;
	threshold.second = m_highThresh;

	::PostMessage(pHWnd, WM_THRESHOLD, 0, 0);
}

void CFilterOption::OnBnClickedSml() {
	sml = ((CButton*)GetDlgItem(IDC_SML))->GetCheck();
	if (sml) {
		FilterList.push_back(SMLF);
	}
	else {
		RemoveFilter(SMLF);
	}
}

void CFilterOption::OnBnClickedApplyFilter() {
	::PostMessage(pHWnd, WM_FILTER_APPLY, 0, 0);
}

void CFilterOption::OnBnClickedLsf()
{
	lsf = ((CButton*)GetDlgItem(IDC_LSF))->GetCheck();
	if (lsf) {
		FilterList.push_back(LSF);
	}
	else {
		RemoveFilter(LSF);
	}
}

void CFilterOption::OnBnClickedDespike()
{
	despike = ((CButton*)GetDlgItem(IDC_DESPIKE))->GetCheck();
	if (despike) {
		FilterList.push_back(DESPIKE);
	}
	else {
		RemoveFilter(DESPIKE);
	}
}
void CFilterOption::OnBnClickedMultiply1000()
{
	::PostMessage(pHWnd, WM_MULTIPLYx1000, 0, 0);
}


void CFilterOption::OnBnClickedUm()
{
	::PostMessage(pHWnd, WM_UM, 0, 0);
}




void CFilterOption::OnBnClickedFft()
{
	fft = ((CButton*)GetDlgItem(IDC_FFT))->GetCheck();
	if (fft) {
		FilterList.push_back(FFT);
	}
	else {
		RemoveFilter(FFT);
	}

}




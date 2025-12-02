#pragma once
#include<opencv2/opencv.hpp>
//10222024 / ARIFUL ISLAM
// 20251118 / added in WLI from UTAC by Mohammad Fahim Hossain

class Cfilters
{
	double* pfPos = NULL;
	double* pfSig = NULL;
	double* pfSignal = NULL;
	double* pfSignalIm = NULL;
	double* pfResult = NULL;
	double* pfResultIm = NULL;
	int m_nMin = 200;
	int m_nMax = 16384;
	int m_nFFT = 16384;
	float m_fExpFilterWidth = 10;
	int nData = 0;
	int m_nX = 10;
	int m_nY = 10;
	//POINT* m_points=NULL;
	size_t m_pointsCnt;

	float calculateMostFreqVal(std::vector<std::vector<float>> img);
	float calculateStandardDeviation(std::vector<std::vector<float>>& img);
	void convertArray(std::vector<std::vector<float>>& img);
	void revertArray(double* pfSig, std::vector<std::vector<float>>& img);
	void FilterArray(double* pfPos, double* pfSig, int nMin, int nMax);
	int GetFFTpoints(int nFFT);
public:
	//cv::Mat convertVectorToMat(std::vector < std::vector<float>>img);
	//std::vector<std::vector<float>> convertMatToVector(cv::Mat& mat);
	void ApplySmoothing(std::vector<std::vector<float>>& img, int kernel);
	void ApplySmoothing2(std::vector<std::vector<float>>& img, int kernel);
	void removeOutliers(std::vector<std::vector<float>>& img);
	void removeOutliers(std::vector<float>& HeightData, int wd, int ht);
	void ApplySML(std::vector < std::vector<float>>& img);
	void ApplyLSF(std::vector<std::vector<float>>& img);
	void ApplyDespike(std::vector<std::vector<float>>& img);
	void ApplyFFT(std::vector<std::vector<float>>& img);
	void ApplyDespikeRowColWise(cv::Mat& ImCV);

	//20251201
	void iterativeAverageFill(int maxIterations, std::vector<float>& HeightData, int ht, int wd);
	/*void RBFInterpolation(std::vector<std::vector<float>>& img);
private:
	float RBFInterpolatePoint(const std::vector<std::vector<float>>& img, int ht, int wd, int px, int py, int kRadius, float cShape);
	bool SolveLinearSystem(std::vector<std::vector<float>>& A, std::vector<float>& b, std::vector<float>& x);*/
};

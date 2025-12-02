#include "pch.h"
#include "filters.h"
#include "MTH/LSF3D.h"
#include "FFT.h"
//#include <opencv2/opencv.hpp>
#include <algorithm>
#include <math.h>
#include <map>
#include <unordered_map>
//using namespace cv;

//10222024 / ARIFUL ISLAM

void Cfilters::ApplySmoothing(std::vector<std::vector<float>>& img, int kernelSize) {
	int rows = static_cast<int>(img.size());
	if (rows == 0) return;
	int cols = static_cast<int>(img[0].size() - 2);

	std::vector<std::vector<float>> temp(img); // Make a copy of the original data

	int kHalf = kernelSize / 2; // Calculate half of the kernel size

	for (int i = kHalf; i < rows - kHalf; ++i) { // Adjust loop to avoid accessing out of bounds
		for (int j = kHalf; j < cols - kHalf; ++j) {
			float sum = 0;
			int count = 0; // To keep track of the number of elements added to sum

			// Sum the values of the current pixel and its neighbors
			for (int ki = -kHalf; ki <= kHalf; ++ki) {
				for (int kj = -kHalf; kj <= kHalf; ++kj) {
					sum += temp[i + ki][j + kj];
					++count;
				}
			}
			// Assign the average value back to the original data
			img[i][j] = sum / count; // Use count in case kernelSize is dynamically decided
		}
	}
}

void Cfilters::ApplySmoothing2(std::vector<std::vector<float>>& img, int kernelSize) {
	int i, j, ki, kj, count;
	int rows = static_cast<int>(img.size());
	if (rows == 0)return;
	int cols = static_cast<int>(img[0].size() - 2);
	float sum = 0;
	std::vector<std::vector<float>>temp(img);

	int kHalf = kernelSize / 2;//Calcualte half of the kernel size
	for (i = kHalf; i < rows - kHalf; ++i) {//Adjust loop to avoid accessing out of bounds
		if (i == kHalf) {
			sum = 0;
			count = 0;
			for (ki = -kHalf; ki <= kHalf; ++ki) {
				for (kj = -kHalf; kj <= kHalf; ++kj) {
					sum += temp[i + ki][kHalf + kj];
					++count;
				}
			}
			sum /= count;
		}
		else {
			count = 0;
			sum = img[i - 1][kHalf] * kernelSize * kernelSize;
			for (kj = -kHalf; kj <= kHalf; ++kj) {
				sum += temp[i + kHalf][kHalf + kj];
				sum -= temp[i - kHalf - 1][kHalf + kj];
				++count;
			}
			sum /= (kernelSize * kernelSize);// Use count in case KernelSize is dynamically decided
		}
		count = 0;
		for (j = kHalf + 1; j < cols - kHalf; ++j) {//sum the values of the current pixel and its neighbors
			sum = img[i][j - 1] * kernelSize * kernelSize;
			for (ki = -kHalf; ki <= kHalf; ++ki) {
				sum += temp[i + ki][j + kHalf];
				sum -= temp[i + ki][j - kHalf - 1];
				count++;
			}
			sum /= (kernelSize * kernelSize);//Use count in case kernelSize is dynamically decided
			img[i][j] = sum;
		}
	}
}

float Cfilters::calculateMostFreqVal(std::vector<std::vector<float>> img) {
	int rows = (int)img.size();
	int cols = (int)img[0].size() - 2;

	std::unordered_map<int, int>freq;
	int key;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			float value = round(img[row][col] * 1000.00f);
			key = (int)value;
			freq[key]++;
		}
	}
	int mxCnt = -1000, mxVal = 0;
	for (auto x : freq) {
		if (mxCnt < x.second) {
			mxCnt = x.second;
			mxVal = x.first;
		}
	}

	float freqVal = (float)mxVal;
	freqVal /= 1000.00f;

	return freqVal;
}

float Cfilters::calculateStandardDeviation(std::vector<std::vector<float>>& img) {
	// Flatten the 2D vector into a 1D vector
	std::vector<float> flatData;
	for (const auto& row : img) {
		flatData.insert(flatData.end(), row.begin(), row.end());
	}

	// Calculate mean
	float sum = 0.0;
	for (const float value : flatData) {
		sum += value;
	}
	float mean = sum / flatData.size();

	// Calculate variance
	float variance = 0.0;
	for (const float value : flatData) {
		variance += (value - mean) * (value - mean);
	}
	variance /= flatData.size();

	// Return standard deviation (square root of variance)
	return std::sqrt(variance);
	//return variance;
}

void Cfilters::removeOutliers(std::vector<std::vector<float>>& img) {
	//ApplySmoothing(img);
	int rows = static_cast<int>(img.size());
	int cols = static_cast<int>(img[0].size() - 2);

	std::vector<std::vector<float>> temp(img); // Make a copy of the original data to preserve original values during calculation

	float freqVal = calculateMostFreqVal(img);// we will replace spikes with this value
	float threshold = calculateStandardDeviation(img);
	for (int i = 0; i < rows; ++i) // Start from 1 and end at rows-1 to avoid accessing out of bounds
	{
		for (int j = 0; j < cols; ++j) // Same for columns
		{
			if (abs(img[i][j] - freqVal) > 3 * threshold)img[i][j] = freqVal;
		}
	}
}

void Cfilters::removeOutliers(std::vector<float>& HeightData, int wd, int ht) {
	std::vector<std::vector<float>> img;
	for (int i = 0; i < ht - 2; i++) {
		std::vector<float>data;
		for (int j = 0; j < wd - 2; j++) {
			data.push_back(HeightData[i * (wd - 1) + j]);
		}
		img.push_back(data);
	}
	removeOutliers(img);
	for (int i = 0; i < ht - 2; i++) {
		for (int j = 0; j < wd - 2; j++) {
			HeightData[i * (wd - 1) + j] = img[i][j];
		}
	}
}

//cv::Mat Cfilters::convertVectorToMat(std::vector<std::vector<float>> vec) {
//	// Determine the size of the matrix
//	int rows = static_cast<int> (vec.size());
//	int cols = static_cast<int>(vec.empty() ? 0 : vec[0].size() - 2);
//
//	// Create an empty Mat with the same size and type float
//	cv::Mat mat(rows, cols, CV_32F);
//
//	// Copy the data from the vector to the Mat
//	for (int i = 0; i < rows; i++) {
//		for (int j = 0; j < cols; j++) {
//			mat.at<float>(i, j) = vec[i][j];
//		}
//	}
//
//	return mat;
//}

//std::vector<std::vector<float>> Cfilters::convertMatToVector(cv::Mat& mat) {
//	int rows = static_cast<int>(mat.rows);
//	int cols = static_cast<int>(mat.cols - 2);
//	std::vector<std::vector<float>> vec(rows, std::vector<float>(cols));
//
//	// Copy the data from the Mat to the vector
//	for (int i = 0; i < rows; i++) {
//		for (int j = 0; j < cols; j++) {
//			vec[i][j] = mat.at<float>(i, j);
//		}
//	}
//
//	return vec;
//}

//void Cfilters::ApplySML(std::vector < std::vector<float>>& img) {
//	cv::Mat imgData = convertVectorToMat(img);
//	//Horizontal Kernel
//	cv::Mat kernel_h;
//	kernel_h = cv::Mat::zeros(3, 3, CV_32F); //filled with zero
//	kernel_h.at<float>(0, 0) = -1; //(col index, row index)
//	kernel_h.at<float>(0, 1) = 2; //(col index, row index)
//	kernel_h.at<float>(0, 2) = -1; //(col index, row index)
//
//	//Applying Horizontal Kernel
//	cv::Mat horizontalFilterImg;
//	cv::filter2D(imgData, horizontalFilterImg, -1, kernel_h);
//
//	//Vertical Kernel
//	cv::Mat kernel_v;
//	kernel_v = cv::Mat::zeros(3, 3, CV_32F); //filled with zero
//	kernel_v.at<float>(0, 1) = -1; //(col index, row index)
//	kernel_v.at<float>(1, 1) = 2; //(col index, row index)
//	kernel_v.at<float>(2, 1) = -1; //(col index, row index)
//	cv::Mat verticalFilterImg;
//	cv::filter2D(imgData, verticalFilterImg, -1, kernel_v);
//
//	// concatenating horizontal and vertical images
//	imgData = abs(horizontalFilterImg) + abs(verticalFilterImg);
//
//	// Sum Mask
//	int N = 4;
//	cv::Mat sum_mask = cv::Mat::ones(2 * N + 1, 2 * N + 1, CV_32F);
//	cv::Mat finalImg;
//	cv::filter2D(imgData, finalImg, -1, sum_mask);
//
//	img = convertMatToVector(finalImg);
//}

void Cfilters::ApplyLSF(std::vector<std::vector<float>>& img)
{
	float fX, fY;
	BOOL bFirst = TRUE;
	float fMin = 0, fMax = 0;
	double dfA, dfB, dfC;
	CLSF3D Lsf;

	for (int row = 0; row < img.size(); ++row) {
		for (int col = 0; col < img[row].size(); ++col) {
			Lsf.Input(row, col, img[row][col]);
		}
	}

	//find best fit plane
	Lsf.GetResult(dfA, dfB, dfC);
	for (int row = 0; row < img.size(); ++row) {
		for (int col = 0; col < img[row].size(); ++col) {
			fX = row, fY = col;
			float dfNorDist = img[row][col] - (dfC + dfA * row + dfB * col);
			if (!bFirst) {
				if (dfNorDist < fMin) {
					fMin = dfNorDist;
				}
				if (dfNorDist > fMax) {
					fMax = dfNorDist;
				}
			}
			else {
				bFirst = FALSE;
				fMax = fMin = dfNorDist;
			}
			img[row][col] = dfNorDist;
		}
	}

}

void Cfilters::ApplyDespike(std::vector<std::vector<float>>& img) {
	int rowNum = img.size();
	int colNum = img[0].size();

	std::vector<std::vector<float>>despikeData;
	despikeData = img;

	for (int col = 0; col < colNum; col++) {
		for (int row = 0; row < rowNum; ++row) {
			if (row - 2 >= 0 && row + 2 < rowNum) {
				std::vector<float>window = {
					//img[row-2][col],
					img[row - 1][col],
					img[row][col],
					img[row + 1][col],
					//img[row + 2][col]
				};

				sort(window.begin(), window.end());
				despikeData[row][col] = window[window.size() / 2];
			}
		}
	}
	img = despikeData;
}

//20250916
void Cfilters::ApplyDespikeRowColWise(cv::Mat& ImCV) {
	int wd = ImCV.cols;
	int ht = ImCV.rows;

	cv::Mat localIM = ImCV.clone();

	int windowSize = 10;
	int half = windowSize / 2;

	// row wise despike.. 
	for (int y = 0; y < ht - 1; y++) {
		float* row = localIM.ptr<float>(y);
		float* rowGlobal = ImCV.ptr<float>(y);

		for (int i = half; i < wd - 1 - half; ++i) {
			std::vector<float> win;
			win.reserve(windowSize);
			for (int j = -half; j <= half; ++j) {
				win.push_back(row[i + j]);
			}

			std::vector<float>tmp = win;
			std::nth_element(tmp.begin(), tmp.begin() + half, tmp.end());
			float median = tmp[half];

			rowGlobal[i] = median;
		}
	}

	//col wise despike.. 
	localIM = ImCV.clone();
	for (int x = 0; x < wd - 1; ++x) {
		for (int i = half; i < ht - 1 - half; ++i) {
			std::vector<float> win;
			win.reserve(windowSize);
			for (int j = -half; j <= half; ++j) {
				win.push_back(localIM.at<float>(i+j, x));
			}

			std::vector<float>tmp = win;
			std::nth_element(tmp.begin(), tmp.begin() + half, tmp.end());
			float median = tmp[half];

			//cols[i] = median;
			ImCV.at<float>(i, x) = median;
		}
	}
}

void Cfilters::applyDespike1DVec(float* pProfileYData, int sz)
{
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

void Cfilters::ApplyFFT(std::vector<std::vector<float>>& img) {
	convertArray(img);
	FilterArray(pfPos, pfSig, m_nMin, m_nMax);
	revertArray(pfSig, img);

	if (pfPos)
		delete[] pfPos;
	if (pfSig)
		delete[] pfSig;
	if (pfSignal)
		delete[] pfSignal;
	if (pfSignalIm)
		delete[] pfSignalIm;
	if (pfResult)
		delete[] pfResult;
	if (pfResultIm)
		delete[] pfResultIm;
}

int Cfilters::GetFFTpoints(int nFFT)
{
	int nFFT2;
	if (nFFT <= 64)
		nFFT2 = 64;
	else if (nFFT <= 128)
		nFFT2 = 128;
	else if (nFFT <= 256)
		nFFT2 = 256;
	else if (nFFT <= 512)
		nFFT2 = 512;
	else if (nFFT <= 1024)
		nFFT2 = 1024;
	else if (nFFT <= 2048)
		nFFT2 = 2048;
	else if (nFFT <= 4096)
		nFFT2 = 4096;
	else if (nFFT <= 8192)
		nFFT2 = 8192;
	else if (nFFT <= 16384)
		nFFT2 = 16384;
	else if (nFFT <= 32768)
		nFFT2 = 32768;
	else if (nFFT <= 65536)
		nFFT2 = 65536;
	else
		nFFT2 = 131072;
	if (pfSignal)
		delete[] pfSignal;
	pfSignal = new double[nFFT2];
	if (pfSignalIm)
		delete[] pfSignalIm;
	pfSignalIm = new double[nFFT2];
	if (pfResult)
		delete[] pfResult;
	pfResult = new double[nFFT2];
	if (pfResultIm)
		delete[] pfResultIm;
	pfResultIm = new double[nFFT2];
	return (nFFT2);
}

void Cfilters::FilterArray(double* pfPos, double* pfSig, int nMin, int nMax) {
	int i;
	float fBaseLine = 0;
	if ((pfPos == NULL) || (pfSig == NULL) || (nData <= 0) || (nMin >= nMax))
		return;
	m_nFFT = GetFFTpoints(m_nFFT);

	if (nMax > m_nFFT / 2) {
		nMax = m_nFFT / 2;
	}

	double fXMin = pfPos[0];
	double fXMax = pfPos[nData - 1];
	double fXStep = (pfPos[nData - 1] - pfPos[0]) / (m_nFFT - 1);

	pfSignal[0] = pfSig[0];
	pfSignal[m_nFFT - 1] = pfSig[nData - 1];

	double fXTemp = pfPos[0];
	int j = 0;
	for (i = 0; i < m_nFFT - 1; ++i) {
		fXTemp += fXStep;
		while (!(fXTemp >= pfPos[j] && fXTemp < pfPos[j + 1])) {
			j++;
			if (j >= nData - 1) {
				break;
			}
		}
		if ((pfSig[j + 1] - pfSig[j]) != 0) {
			pfSignal[i] = pfSig[j] + (fXTemp - pfPos[j]) * (pfSig[j + 1] - pfSig[j]) / (pfSig[j + 1] - pfSig[j]);
		}
		else {
			pfSignal[i] = pfSig[j];
		}
	}
	memset(pfSignalIm, 0, m_nFFT * sizeof(double));
	memset(pfResult, 0, m_nFFT * sizeof(double));
	memset(pfResultIm, 0, m_nFFT * sizeof(double));

	fBaseLine = 0.0;
	for (i = 0; i < m_nFFT; i++)
	{
		fBaseLine += pfSignal[i];
	}

	fBaseLine /= m_nFFT;

	for (i = 0; i < m_nFFT; i++)
	{
		pfSignal[i] -= fBaseLine;
	}

	CFFT FFT;
	FFT.fft_double(m_nFFT, 0, pfSignal, pfSignalIm, pfResult, pfResultIm);
	float fCenter = (m_nMin + m_nMax) / 2;
	for (i = 0; i < m_nFFT; i++)
	{
		pfSignal[i] = pfResult[i] * (1 - exp(-(i - fCenter) * (i - fCenter) / m_fExpFilterWidth / m_fExpFilterWidth));
		pfSignalIm[i] = pfResultIm[i] * (1 - exp(-(i - fCenter) * (i - fCenter) / m_fExpFilterWidth / m_fExpFilterWidth));
	}

	FFT.fft_double(m_nFFT, 1, pfSignal, pfSignalIm, pfResult, pfResultIm);

	double fMax = -1e60;
	double fMin = 1e60;

	for (i = 0; i < m_nFFT; i++)
	{
		if (pfResult[i] > fMax)
			fMax = pfResult[i];
		if (pfResult[i] < fMin)
			fMin = pfResult[i];
	}
	if (fMax > fMin)
	{
		for (i = 0; i < m_nFFT; i++)
		{
			if (pfResult[i] < 0)
				pfResult[i] = (double)-sqrt((double)(pfResult[i] * pfResult[i] + pfResultIm[i] * pfResultIm[i]));
			else
				pfResult[i] = (double)sqrt((double)(pfResult[i] * pfResult[i] + pfResultIm[i] * pfResultIm[i]));
			pfResult[i] += fBaseLine;
		}
	}
	// update signal
	double fPositionDataStep = (pfPos[nData - 1] - pfPos[0]) / (nData - 1);
	pfSig[0] = (float)pfResult[0];
	pfSig[nData - 1] = (float)pfResult[m_nFFT - 1];
	j = 0;
	for (i = 1; i < nData - 1; i++)
	{
		float x = 1.0 * i * (m_nFFT - 1) / (nData - 1);
		while (!((x >= j) && (x < j + 1)))
		{
			j++;
			if (j >= m_nFFT - 1)
				break;
		}
		pfSig[i] = (float)(pfResult[j] + (pfResult[j + 1] - pfResult[j]) * (x - j));
		pfPos[i] = (float)(pfPos[0] + i * fPositionDataStep);
	}
}

void Cfilters::convertArray(std::vector<std::vector<float>>& img) {
	int row = img.size();
	int col = img[0].size();
	int nDataCnt = row * col;

	int id = 0;
	pfPos = new double[nDataCnt + 2];
	pfSig = new double[nDataCnt + 2];
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			pfSig[id] = static_cast<double>(img[i][j]);
			pfPos[id] = static_cast<double>(id);
			id++;
		}
	}
}

void Cfilters::revertArray(double* pfSig, std::vector<std::vector<float>>& img) {
	int row = img.size();
	int col = img[0].size();
	int id = 0;
	for (int i = 0; i < row; ++i) {
		for (int j = 0; j < col; ++j) {
			img[i][j] = static_cast<float>(pfSig[id]);
			id++;
		}
	}
}

//20251201=====================
//static inline float phi(float r, float c) {
//	return std::sqrt(r * r + c * c);
//}
//
//bool Cfilters::SolveLinearSystem(std::vector<std::vector<float>>& A, std::vector<float>& b, std::vector<float>& x) {
//	int n = A.size();
//	x.resize(n);
//	for (int i = 0; i < n; ++i) {
//		float maxVal = std::abs(A[i][i]);
//		int pivot = i;
//		
//		for (int r = i + 1; r < n; ++r) {
//			if(std::abs(A[r][i]) > maxVal) {
//				maxVal = std::abs(A[r][i]);
//				pivot = r;
//			}
//		}
//
//		if (pivot != i) {
//			std::swap(A[i], A[pivot]);
//			std::swap(b[i], b[pivot]);
//		}
//
//		if (std::abs(A[i][i]) < 1e-12f)
//			return false; // singular
//
//		for (int r = i + 1; r < n; r++) {
//			float f = A[r][i] / A[i][i];
//			for (int c = i; c < n; c++)
//				A[r][c] -= f * A[i][c];
//			b[r] -= f * b[i];
//		}
//	}
//
//	// Back substitution
//	for (int i = n - 1; i >= 0; i--) {
//		float sum = b[i];
//		for (int c = i + 1; c < n; c++)
//			sum -= A[i][c] * x[c];
//		x[i] = sum / A[i][i];
//	}
//	return true;
//}
//
//float Cfilters::RBFInterpolatePoint(const std::vector < std::vector<float>>& img,
//	int ht, int wd,
//	int px, int py,
//	int kRadius = 5,
//	float cShape = 2.0f) {
//
//	std::vector<std::pair<std::pair<int, int>, float>>pts;
//
//	for (int dy = -kRadius; dy <= kRadius; dy++) {
//		for (int dx = -kRadius; dx <= kRadius; dx++) {
//			int y = py + dy;
//			int x = px + dx;
//			if (x >= 0 && x < wd && y >= 0 && y <= ht) {
//				float v = img[y][x];
//				if (isnan(static_cast<double>(v))) {
//					pts.push_back({ {x,y},v });
//				}
//			}
//		}
//	}
//
//	int N = pts.size();
//	if (N < 3) {
//		return 0;
//	}
//
//	std::vector<std::vector<float>>A(N, std::vector<float>(N));
//	std::vector<float>b(N), w;
//
//	for (int i = 9; i < N; ++i) {
//		float xi = pts[i].first.first;
//		float yi = pts[i].first.second;
//		b[i] = pts[i].second;
//
//		for (int j = 0; j < N; ++j) {
//			float xj = pts[j].first.first;
//			float yj = pts[j].first.second;
//
//			float dx = xi - xj;
//			float dy = yi - yj;
//			float r = std::sqrt(dx * dx + dy * dy);
//			A[i][j] = phi(r, cShape);
//		}
//	}
//
//	if (!SolveLinearSystem(A, b, w)) {
//		return 0;
//	}
//
//	float result = 0.0;
//	for (int i = 0; i < N; ++i) {
//		float xi = pts[i].first.first;
//		float yi = pts[i].first.second;
//
//		float dx = px - xi;
//		float dy = py - yi;
//		float r = std::sqrt(dx * dx + dy * dy);
//		result += w[i] * phi(r, cShape);
//	}
//	return result;
//}
//
//void Cfilters::RBFInterpolation(std::vector<std::vector<float>>& img) {
//	int ht = img.size();
//	int wd = img[0].size();
//
//	std::vector<std::vector<float>> out = img;
//
//#pragma omp parallel for
//	for (int y = 0; y < ht; y++) {
//		for (int x = 0; x < wd; ++x) {
//			if (isnan(static_cast<double>(img[y][x]))) {
//				out[y][x] = RBFInterpolatePoint(img, ht, wd, x, y);
//			}
//		}
//	}
//}


void Cfilters::iterativeAverageFill(int maxIterations, std::vector<float>& HeightData, int ht, int wd) {
	std::vector<float>data = HeightData;
	for (int iter = 0; iter < maxIterations; iter++) {
		bool changed = false;
		HeightData = data;
		for (int r = 0; r < ht; r++) {
			for (int c = 0; c < wd; c++) {
				if (std::isnan(static_cast<double>(HeightData[r * (wd) + c]))) {
					double sum = 0.0;
					int count = 0;

					// Check 4-connected neighbors
					int dr[] = { -1, 1, 0, 0, +1, +1, -1, -1 };
					int dc[] = { 0, 0, -1, 1, -1, +1, -1, +1 };

					for (int i = 0; i < 8; i++) {
						int nr = r + dr[i];
						int nc = c + dc[i];

						if (nr >= 0 && nr < ht && nc >= 0 && nc < wd) {
							if (!std::isnan(static_cast<double>(HeightData[nr * (wd) + nc]))) {
								sum += data[nr * (wd) + nc];
								count++;
							}
						}
					}

					if (count > 0) {
						HeightData[r * (wd) + c] = sum / count;
						changed = true;
					}
				}
			}
		}
		data = HeightData;
		if (!changed) break;
	}
	HeightData = data;
}


//=============================

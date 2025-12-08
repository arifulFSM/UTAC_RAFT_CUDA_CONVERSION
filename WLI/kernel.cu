#include"kernel.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "opencv2/core/cuda.hpp"
#include <chrono>
#include <sstream>
#include <fstream>

//#include "../PSI/sdef.h"
#define MaxImages 5000
#define BADDATA -999

__device__ void SetI(int idx, int* i1, int* i2, int inc3, int numImgs) {
	*i1 = idx - inc3; if (*i1 < 0)*i1 = 0;
	*i2 = idx + inc3; if (*i2 > numImgs)*i2 = numImgs;
}

__device__ bool EnsureValid(int inc, int* i1, int* i2,int numImgs) {
	int inc2 = 2 * inc;
	if (*i1 < inc2)*i1 = inc2; if (*i2 > (numImgs - inc2))*i2 = numImgs - inc2;
	if ((*i2 - *i1) < 4 * inc) return false;
	return true;
}

__device__ void PhasePV5(float* d_pPHS1, float* d_pVIS1, float* p3,int st,int ed,int inc,int inc2) {
	float N, D;
	float sn = 1.9999831f;
	for (int i = st; i < ed; i++) {
		N = sn * (p3[i-inc] - p3[i+inc]);
		D = p3[i] * 2 - p3[i+inc2] - p3[i-inc2];
		d_pPHS1[i] = (float)atan2(N, D);
		d_pVIS1[i] = (float)sqrtf(N * N + D * D);
	}
}

__device__ float PeakPhas(float* d_pPHS1,float* d_HeightData, int st,int ed) {

	float sumx = 0.0;
	float sumx2 = 0.0;
	float sumxy = 0.0;
	float sumy = 0.0;
	float sumy2 = 0.0;
	int N = 0;
	float M, C, R;

	for (int i = st; i <= ed; ++i/*, v++, z++*/) {
		float x =/* *z; */d_HeightData[i];
		float y = /**v;*/ d_pPHS1[i];

		sumx += x;
		sumx2 += (x * x);
		sumxy += (x * y);
		sumy += y;
		sumy2 += (y * y);
		N++;
	}

	// calc LSF
	float denom = (N * sumx2 - sumx * sumx);
	if (denom == 0) {
		// singular matrix. can't solve the problem.
		M = 0; C = 0; R = 0;
		//return;
	}

	M = (N * sumxy - sumx * sumy) / denom;
	C = (sumy * sumx2 - sumx * sumxy) / denom;
	R = (sumxy - sumx * sumy / N) /    /* compute correlation coeff */
		sqrt((sumx2 - (sumx * sumx) / N) *
			(sumy2 - (sumy * sumy) / N));

	if (M)return -C / M;
	else
		return 0.0f;
}

__global__ void CollectZCHKernel(
	unsigned char* d_imgs
	, const int numImages
	, int ht, int wd
	, int inc
	, float* d_OutImg
	, float* d_HeightData
) {
	int x = blockDim.x * blockIdx.x + threadIdx.x;
	int y = blockDim.y * blockIdx.y + threadIdx.y;

	if (x >= wd || y >= ht) return;

	float sum = 0.0f;
	/*float chR[MaxImages];
	float chG[MaxImages];
	float chB[MaxImages];*/
	float chGray[MaxImages];
	float d_pPHS1[MaxImages];
	//float d_pVIS1[MaxImages];
	float fMax = 0.0, fMin = 1000.0;
	long long int iMax = -1, iMin = -1;
	long long int idx;

	long long int pixIndexBase = (y * wd + x) * 3;
	for (long long int i = 0; i < numImages; ++i)
	{
		long long int imgOffset = i * ht * wd * 3;
		unsigned char* pixPtr = d_imgs + imgOffset + pixIndexBase;
		float gray = ((*pixPtr) + *(pixPtr+1) + *(pixPtr+2)) / 3.0;

		/*chR[i] = pixPtr[2];
		chG[i] = pixPtr[1];
		chB[i] = pixPtr[0];*/
		chGray[i] = gray;
		d_pPHS1[i] = 0.0;
		//d_pVIS1[i] = 0.0;

		if (gray > fMax)
		{
			fMax = gray;
			iMax = i;
		}
		else if (gray < fMin)
		{
			fMin = gray;
			iMin = i;
		}
		sum += gray;
	}

	idx = iMax;
	
	//SetI(idx,inc3,sz) function implementation;
	int i1, i2, inc2, inc3, inc4;
	inc2 = 2 * inc; inc3 = 3 * inc; inc4 = 4 * inc;
	i1 = 4, i2 = numImages-4;
	SetI(idx, &i1, &i2, inc3, numImages);

	//PhasePV5 algorithm implementation to calcualte pPHS1 and pVIS1
	int st, ed;
	bool flagEns = EnsureValid(inc, &i1, &i2, numImages);

	st = i1, ed = i2;
	//PhasePV5(d_pPHS1, d_pVIS1, chGray, st, ed, inc, inc2);// device function
	float N, D;
	float sn = 1.9999831f;
	float visibilityThreshold = 700, visibility;
	bool visibilityHigh= false;
	for (int i = st; i < ed; i++) {
		N = sn * (chGray[i - inc] - chGray[i + inc]);
		D = chGray[i] * 2 - chGray[i + inc2] - chGray[i - inc2];
		d_pPHS1[i] = /*(float)*/atan2(N, D);
		//d_pVIS1[i] = /*(float)*/sqrtf(N * N + D * D);
		if ((N * N + D * D) > visibilityThreshold) {
			visibilityHigh |= 1;
		}
	}

	//PeakPhas algorith implementation to calcualte the rsl
	float rsl;// = PeakPhas(d_pPHS1, d_HeightData, idx - 2, idx + 2);
	float sumx = 0.0;
	float sumx2 = 0.0;
	float sumxy = 0.0;
	float sumy = 0.0;
	float sumy2 = 0.0;
	int cnt = 0;
	float M, C, R;
	st = idx - 2, ed = idx + 2;
	for (int i = st; i <= ed; ++i) {
		float x = d_pPHS1[i]; 
		float y = d_HeightData[i];

		sumx += x;
		sumx2 += (x * x);
		sumxy += (x * y);
		sumy += y;
		sumy2 += (y * y);
		cnt++;
	}

	// calc LSF
	float denom = (cnt * sumx2 - sumx * sumx);
	if (denom == 0) {
		// singular matrix. can't solve the problem.
		M = 0; C = 0; R = 0;
		//return;
	}
	else {
		M = (cnt * sumxy - sumx * sumy) / denom;
		C = (sumy * sumx2 - sumx * sumxy) / denom;
		R = (sumxy - sumx * sumy / cnt) /
			sqrt((sumx2 - (sumx * sumx) / cnt) *
				(sumy2 - (sumy * sumy) / cnt));
	}

	

	//if (M)rsl= -float( - C / M);
	if (M)rsl= -float(C);
	else
		rsl=0.0f;
	if (rsl > 200.f) rsl = BADDATA;
	
	if (!visibilityHigh) {
		d_OutImg[y * wd + x] = BADDATA;
	}
	else
		d_OutImg[y * wd + x] = rsl;
	//d_OutImg[y * wd + x] = 5.0;// sum / float(numImages);
}


extern "C" void MatsToBuffer(std::vector < std::pair<cv::Mat, float>>& mats, std::vector<unsigned char>& buf) {
	assert(!mats.empty());
	long long int numImgs = (int)mats.size();
	long long int ht = mats[0].first.rows;
	long long int wd = mats[0].first.cols;
	long long int channels = mats[0].first.channels();
	assert(channels == 3);
	buf.resize(numImgs * ht * wd * channels);

	long long int imageSizeBytes = ht * wd * channels;

	for (long long int i = 0; i < numImgs; ++i) {
		const cv::Mat& m = mats[i].first;
		assert(m.type() == CV_8UC3);
		if (m.isContinuous()) {
			std::memcpy(buf.data() + i * imageSizeBytes, m.data, imageSizeBytes);
		}
		else {
			for (int r = 0; r < ht; ++r) {
				const unsigned char* srcRow = m.ptr<unsigned char>(r);
				unsigned char* dstRow = buf.data() + i * imageSizeBytes + (size_t)r * (size_t)wd * channels;
				std::memcpy(dstRow, srcRow, (size_t)wd * channels);
			}
		}
	}
}

extern "C" void PSO(std::vector<std::pair<cv::Mat, float>>& CVImgs,cv::Mat& CVIm16um, int inc) {

	auto start_time = std::chrono::steady_clock::now();
	int numImgs = (int)CVImgs.size();
	int wd = CVImgs[0].first.cols;
	int ht = CVImgs[0].first.rows;
	int channel = CVImgs[0].first.channels();

	float* pHeightData = new float[numImgs];
	//float* pHeightDataGPU = new float[numImgs];
	for (int i = 0; i < numImgs; ++i)
	{
		pHeightData[i] = CVImgs[i].second;
	}
	
	std::vector<unsigned char>h_images;
	//MatsToBuffer(CVImgs, h_images);
	long long int singleImg = CVImgs[0].first.total() * CVImgs[0].first.elemSize();;
	long long int totalBytes = numImgs * singleImg;// h_images.size(); 
	//long long int singleImg = (size_t)wd * (size_t)ht * channel * sizeof(unsigned char);

	//allocate device memory
	unsigned char* d_images=nullptr;
	float* d_out;
	float* d_HeightData;
	size_t outBytes = (size_t)wd * (size_t)ht * sizeof(float);
	float* h_out = new float[(size_t)wd * (size_t)ht];
	
	
	cudaMalloc(&d_images, totalBytes);
	cudaMalloc(&d_out, outBytes);
	cudaMalloc(&d_HeightData, (size_t)numImgs * sizeof(float));
	 
	//copy data from host to device
	for (long long int i = 0; i < numImgs; ++i) {
	cudaMemcpy((unsigned char*)d_images+i*singleImg, CVImgs[i].first.data, singleImg, cudaMemcpyHostToDevice);
	}
	//cudaMemcpy(d_images, h_images.data(), totalBytes, cudaMemcpyHostToDevice);
	cudaMemcpy(d_HeightData, pHeightData/*.data()*/, (size_t)numImgs * sizeof(float), cudaMemcpyHostToDevice);

	//launch kernel
	dim3 threadsPerBlock(16, 16);
	dim3 blocksPerGrid((wd + threadsPerBlock.x - 1) / threadsPerBlock.x, (ht + threadsPerBlock.y - 1) / threadsPerBlock.y);
	//cudaDeviceSetLimit(cudaLimitStackSize, 32768);
	CollectZCHKernel << <blocksPerGrid, threadsPerBlock >> > (d_images,numImgs,ht,wd, inc,d_out, d_HeightData/*, d_pPHS1, d_pVIS1*/);
	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess) {
		printf("Kernel Error: %s\n", cudaGetErrorString(err));
	}
	cudaDeviceSynchronize();

	//cudaMemcpy(h_out, d_out, ht * wd * sizeof(float), cudaMemcpyDeviceToHost);
	cudaMemcpy(CVIm16um.ptr<float>(), d_out, ht * wd * sizeof(float), cudaMemcpyDeviceToHost);

	// print to csv
	//std::string folder = "C:/WLIN/BMP/DATA/";
	//std::string zFilePath = folder + "OutImg.csv";
	//std::ofstream zFile(zFilePath);
	//if (!zFile.is_open()) {
	//	//AfxMessageBox(L"Failed to open Z positions file", MB_ICONERROR);
	//	//return;
	//}
	//for (int i = 0; i < ht; i++) {
	//	for (int j = 0; j < wd; j++) {
	//		float val = h_out[i * wd + j];
	//		zFile << val << ",";
	//		CVIm16um.at<float>(i, j) = val;
	//	}
	//	zFile << std::endl;
	//	//printf("\n");
	//}

	//zFile.close();
	cudaFree(d_images);
	cudaFree(d_out);
	cudaFree(d_HeightData);
	delete[] pHeightData;
	delete[] h_out;
	h_images.clear();
	
}
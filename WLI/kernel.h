#pragma once
#include <vector>
#include <opencv2/core.hpp>

//#include "../PSI/Frng.h"
//#include "../PSI/Roi.h"

extern "C" {
	void cudaAdd(int* c, const int* a, const int* b, size_t size);
	void MatsToBuffer(std::vector < std::pair<cv::Mat, float>>& mats,std::vector<unsigned char>& buf);
	void PSO(std::vector<std::pair<cv::Mat, float>>& CVImgs,cv::Mat& CVIm16um, int inc);
}
#include "stdafx.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFFT
{
public:
	CFFT();
	virtual ~CFFT();

protected:
	bool IsPowerOfTwo(unsigned int p_nX);
	unsigned int NumberOfBitsNeeded(unsigned int p_nSamples);
	double Index_to_frequency(unsigned int p_nBaseFreq, unsigned int p_nSamples, unsigned int p_nIndex);
	unsigned int ReverseBits(unsigned int p_nIndex, unsigned int p_nBits);
public:
	void fft_double(unsigned int p_nSamples, bool p_bInverseTransform, double* p_lpRealIn, double* p_lpImagIn, double* p_lpRealOut, double* p_lpImagOut);
	void fft_double2(unsigned int p_nSamples, bool p_bInverseTransform, double* pdata);
};
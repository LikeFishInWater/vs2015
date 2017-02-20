#include "Complex.h"

#pragma once

struct RandSeed
{
	unsigned long IX;
	unsigned long IY;
	unsigned long IZ;
};

class CChannel
{
public:
	CChannel();
	~CChannel();
	CComplex *SymbolSeq;
	RandSeed RS;
	int RandomSeed;


	void Initial(unsigned long len);
	//	double *Real;
	//	double *Image;
	double Random_Uniform(RandSeed & rs);
	double Random_Norm(double sigma, RandSeed & rs);
	void AWGNChannel(double * inSymbolSeqRe, double * inSymbolSeqIm, double sigma, int SymbolLen);
};




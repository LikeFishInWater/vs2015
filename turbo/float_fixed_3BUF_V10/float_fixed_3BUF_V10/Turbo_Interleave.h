/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void SubInterleave(int *inputBit, int *outputBit, int offset, int R, int C, int F0, const int *P);

void TurboInterleave(int *rateMatchOut, int rateMacthLen, int codeBlockSize, int codeBlockIndex, int f1, int f2, 
					int nullBitNum, int *systemBit, int *parityBit, int *iParityBit, int R, int k0);

void SubInterleave_double(double *inputBit, double *outputBit, int offset, int R, int C, int F0, const int *P);

void TurboInterleave_double(double *rateMatchOut, int rateMacthLen, int codeBlockSize, int codeBlockIndex, int f1, int f2, 
					int nullBitNum, double *systemBit, double *parityBit, double *iParityBit, int R, int k0);
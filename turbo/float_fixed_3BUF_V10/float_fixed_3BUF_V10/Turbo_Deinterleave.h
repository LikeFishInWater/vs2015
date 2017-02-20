/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void TurboDeinterleave(double *decodeIn, double *systemBit, double *parityBit, double *iParityBit,
					   int blockSizeAddTail, int k0, int R, int rateMatchLen, int nullBitNum, int codeBlockIndex);
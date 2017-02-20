/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void TurboDeinterleave_fix(int *decodeIn, int *systemBit, int *parityBit, int *iParityBit,
					   int blockSizeAddTail, int k0, int R, int rateMatchLen, int nullBitNum, int codeBlockIndex);
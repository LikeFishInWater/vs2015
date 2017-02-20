/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

int LTE_Turbo_Decode_fixed(int *decodeOut,int *La, int *decodeIn, int msgLen, int decodeInLen,
				int NL, int Qm, int NIR, int rvIndex, TurboCode *TC, LR *lr, OTHERS *others);
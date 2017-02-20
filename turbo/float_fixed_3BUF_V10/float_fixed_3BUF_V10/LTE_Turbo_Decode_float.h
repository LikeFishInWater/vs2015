/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

int CrcCheck(int *seqIn, int seqInLen, int crcLen, int crc24Type);

int ComputePara(int A, int G, int NIR, int NL, int Qm, int rvIndex, int *k, int *f1, int *f2, int Para[][7]);

int LTE_Turbo_Decode_float(int *decodeOut,double *La, double *decodeIn, int msgLen, int decodeInLen,
				int NL, int Qm, int NIR, int rvIndex, TurboCode *TC);
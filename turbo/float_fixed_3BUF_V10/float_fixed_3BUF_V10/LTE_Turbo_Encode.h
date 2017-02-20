/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void CRCEncode(int *seqOut, int *seqIn, int seqInLen, int crcLen, int crc24Type);

void LTE_Turbo_Encode(int *rateMatchOut, int *transBlockIn, int rateMatchLen, int transBlcokLen, int NL, int Qm, int NIR, int rvIndex);
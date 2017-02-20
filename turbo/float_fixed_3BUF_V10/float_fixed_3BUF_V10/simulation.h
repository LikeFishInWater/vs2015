/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void Test(double snr, TurboCode *TC, StatisStruct *SS, SPStruct *SP);

int statistics(TurboCode *TC, StatisStruct *SS, SPStruct *SP, int *decodeOut, int *originBit, int crcWrong);

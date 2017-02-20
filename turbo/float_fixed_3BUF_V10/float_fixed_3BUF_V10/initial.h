/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void GetParameter(TurboCode *TC, SPStruct *SP , LR *lr, OTHERS *others);

void WriteLogo(TurboCode *TC, StatisStruct *SS, SPStruct *SP, LR *lr, OTHERS *others);

void InitSimPara(StatisStruct *SS, SPStruct *SP);

void GetCfg(FILE *fp);

int GenPN(PNSeed *initPN);
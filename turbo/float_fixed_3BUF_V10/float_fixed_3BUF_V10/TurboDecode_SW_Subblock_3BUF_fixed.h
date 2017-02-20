/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void TurboDecode_SW_Subblock_3BUF_fixed(int *decodeOut, int *systemBit, int *parityBit, int *iParityBit, int decodeInLen, int f1, int f2, 
				 int decNum, int iterMax, int B, int normFactor, int decMethod,int *Le_sys,int *Le_par0,int *Le_par1,
				 LR *lr,OTHERS *others);

void SubDecode_SW_Subblock_3BUF_fixed(int *systemBit, int *parityBit0, int *parityBit1, int *Le, int blockSize,
				  int *sysTailBit, int *parTailBit,   int decNum, int outputEn, int *decodeOut, int decMode, 
				  int Req_A_init, int Req_Rf_init, int *Req_Qf_init, int Req_Rg_init, int *Req_Qg_init, int Rf2, int Qf2, 
				  int B, int normFactor, int decMethod, int initAlpha[][8], int initBeta_3BUF[][maxWinNum][8],
				  int *Le_sys,int *Le_par,LR *lr,OTHERS *others);

void FPU_3BUF_fix(int sysBit, int parBit, int La, int initFlag, int *initAlpha, int *alphaValueIn, int decMethod,OTHERS *others);

void BPU_3BUF_fix(int sysBit, int parBit, int La, int initFlag, int *initBeta, int *betaValueIn, 
		 int *alphaValueIn, int normalFactor, int *Le, int *LLR,int *Le_sys,int *Le_par, int decMethod, int Le_enable,OTHERS *others);

int MaxFunction_fix(int a, int b, int decMethod);

int Max8Function_fix(int data1,int data2,int data3,int data4,int data5,int data6,int data7,int data8, int decMethod);

void Butterfly_fix(int cValue0, int cValue1, int gamma, int *nValue0, int *nValue1, int decMethod);



void SaveBUF_3BUF_fix(int BUF_Write_Pos, int B, int decNum, int *sysBit, int *parBit, int *La,
			 int BUF0_SYS[][128],  int BUF0_PAR[][128],  int BUF0_LA[][128],  
			 int BUF1_SYS[][128],  int BUF1_PAR[][128],  int BUF1_LA[][128], 
			 int BUF2_SYS[][128],  int BUF2_PAR[][128],  int BUF2_LA[][128],
			 int BUF0_dir, int BUF1_dir, int BUF2_dir);

void SelectBUF_3BUF_fix(int id, int decNum, int *sysBit, int *parBit, int *La,
			   int *BUF0_sysBit,  int *BUF0_parBit,  int *BUF0_La,  
			   int *BUF1_sysBit,  int *BUF1_parBit,  int *BUF1_La,  
			   int *BUF2_sysBit,  int *BUF2_parBit,  int *BUF2_La);
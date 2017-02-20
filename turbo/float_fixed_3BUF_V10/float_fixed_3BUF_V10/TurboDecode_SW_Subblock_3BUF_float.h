/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void TurboDecode_SW_Subblock_3BUF(int *decodeOut, double *systemBit, double *parityBit, double *iParityBit, int decodeInLen, int f1, int f2, 
				 int decNum, int iterMax, int B, double normFactor, int decMethod,double *Le_sys,double *Le_par0,double *Le_par1);

void SubDecode_SW_Subblock_3BUF(double *systemBit, double *parityBit0, double *parityBit1, double *Le, int blockSize,
				  double *sysTailBit, double *parTailBit,   int decNum, int outputEn, int *decodeOut, int decMode, 
				  int Req_A_init, int Req_Rf_init, int *Req_Qf_init, int Req_Rg_init, int *Req_Qg_init, int Rf2, int Qf2, 
				  int B, double normFactor, int decMethod, double initAlpha[][8], double initBeta_3BUF[][maxWinNum][8],double *Le_sys,double *Le_par);

void CalcInitAddr(int x, int f1, int f2, int *A, int *Rf, int *Qf, int *Rg, int *Qg, int codeSize, int blockSize, int decNum, int dir);

void CalcReadAddr_3BUF(int Req_A_init, int Req_Rf_init, int *Req_Qf_init, int Req_Rg_init, int *Req_Qg_init, int CmpCounter,
				  int *Req_A, int *Req_Rf, int *Req_Qf, int *Req_Rg, int *Req_Qg, int decNum, int blockSize, int decMode,
				 int Rf2, int Qf2, int *AddrP, int *resortP, int *AddrSL, int *resortSL);

void SaveLeInitWriteAddr_3BUF(int *Le_Rf_init_1, int *Le_Rf_init_2, int *Le_Qf_init_1, int *Le_Qf_init_2, int *Le_Rg_init_1, int *Le_Rg_init_2,
							  int *Le_Qg_init_1, int *Le_Qg_init_2, int *Le_A_init_1, int *Le_A_init_2, int *Req_Rg, int *Req_Qg, int *Req_Rf,
							  int *Req_Qf, int Req_A,int CmpCounter, int B, int r, int q_up, int decNum, int blockSize);

void CalcLeWriteAddr_3BUF(int *Le_Rf_init_2, int *Le_Qf_init_2, int *Le_Rg_init_2, int *Le_Qg_init_2, int Le_A_init_2, int *resortP,
						int *Le_Rf, int *Le_Qf, int *Le_Rg, int *Le_Qg,  int *Le_A, int CmpCounter, int B, int decMode, int q_up, int r,
						int decNum, int blockSize, int Rf2, int Qf2, int *Le_w_ad, int *Le_w_sort);

void CalcBufAddr_BUFA(int initDir, int CmpCounter, int B, int *Ad, int *dir, int q_up, int r);
void CalcBufAddr_3BUF(int initDir,int initFlag, int CmpCounter, int B, int *Ad, int *dir, int *Flag, int final_BUF_flag, int q_up, int r);

void SaveBUF_3BUF(int BUF_Write_Pos, int B, int decNum, double *sysBit, double *parBit, double *La,
			 double BUF0_SYS[][128],  double BUF0_PAR[][128],  double BUF0_LA[][128],  
			 double BUF1_SYS[][128],  double BUF1_PAR[][128],  double BUF1_LA[][128], 
			 double BUF2_SYS[][128],  double BUF2_PAR[][128],  double BUF2_LA[][128],
			 int BUF0_dir, int BUF1_dir, int BUF2_dir);

void SelectBUF_3BUF(int id, int decNum, double *sysBit, double *parBit, double *La,
			   double *BUF0_sysBit,  double *BUF0_parBit,  double *BUF0_La,  
			   double *BUF1_sysBit,  double *BUF1_parBit,  double *BUF1_La,  
			   double *BUF2_sysBit,  double *BUF2_parBit,  double *BUF2_La);

void BPU_Warm(double sysBit, double parBit, double La, int initFlag, double *initBeta, double *betaValueIn, int decMethod);

void FPU(double sysBit, double parBit, double La, int initFlag, double *initAlpha, double *alphaValueIn, int decMethod);

void BPU(double sysBit, double parBit, double La, int initFlag, double *initBeta, double *betaValueIn, 
		 double *alphaValueIn, double normalFactor, double *Le, int *LLR,double *Le_sys,double *Le_par, int decMethod);

void AddrCal1Step(int *Rf, int *Qf, int *Rg, int *Qg, int Rf2, int Qf2, int CmpDirection, int blockSize, int decNum);

double MaxFunction(double a, double b, int decMethod);

double Max8Function(double data1,double data2,double data3,double data4,
					double data5,double data6,double data7,double data8, int decMethod);

void Butterfly(double cValue0, double cValue1, double gamma, double *nValue0, double *nValue1, int decMethod);

int BigMod(double d, int m);

void Reorder(double *data, int decNum, int *resortSE, int writeEn);

void ReorderI(int *data, int decNum, int *resortSE, int writeEn);
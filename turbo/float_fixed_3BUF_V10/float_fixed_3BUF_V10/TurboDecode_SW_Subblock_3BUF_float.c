/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "TurboDecode_SW_Subblock_3BUF_float.h"
#include "LTE_Turbo_Decode_float.h"

/*
********************************************************************************
* NAME:				TurboDecode_SW_Subblock_3BUF
* PURPOSE:			��һ��������Turbo���룬���ò��л���˼��.��TurboDecode_SW��TurboDecode_SW_Subblock��ͬ���ڣ��˴�ʡȥ��BPU_warm��һģ�飬��3·����
					�ļ��㻯Ϊ��·(BPU��FPU)���ڽ�Լ������Դ������¿�����������
*
* Input:
systemBit:			�⽻֯�������ϵͳ�����������λ�������0
parityBit:			�⽻֯�������У�����������Ӧ����������1�����У����أ����λ�������0
iParityBit:			�⽻֯�������У�����������Ӧ����������2�����У����أ����λ�������0
decodeInLen:		�������ĳ���
f1:					QPP��֯���Ĳ���
f2:					QPP��֯���Ĳ���
decNum:				�������������������
iterMax:			Ԥ�����������������
initBlockSize:		��齻�����ֵĳ���
normFactor:			����������Ϣ�ϵĳ���
betaValue:			�洢betaֵ������
decMethod:			�����㷨��0->Max-Log-MAP��1->Log-MAP
*
* Output:
decodeOut:			��ǰ�������Ľ�����
Le_sys:				�ṩ��demodulation��ϵͳ���ص�������Ϣ
Le_par0:			�ṩ��demodulation�ı�����1��У����ص�������Ϣ
Le_par1:			�ṩ��demodulation�ı�����2��У����ص�������Ϣ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void TurboDecode_SW_Subblock_3BUF(int *decodeOut, double *systemBit, double *parityBit, double *iParityBit, int decodeInLen, int f1, int f2, 
				 int decNum, int iterMax, int B, double normFactor, int decMethod,double *Le_sys,double *Le_par0,double *Le_par1)
{
	double Le[6144] = {0};
	double sysTailBit[6] = {0};
	double parTailBit[6] = {0};
	int i = 0;
	int iter = 0;
	int outputEn = 0;
	int blockSize = 0;
	int Req_RF_init = 0;
	int Req_RG_init = 0;
	int Req_QF_init[maxDecNum] = {0};
	int Req_QG_init[maxDecNum] = {0};
	int Req_A_init = 0;
	int Rf2 = 0;
	int Qf2 = 0;
	double initBeta0_3BUF[maxDecNum][maxWinNum][8] = {0};
	double initBeta1_3BUF[maxDecNum][maxWinNum][8] = {0};
	int widNum;
	int widNum_temp;
	double initAlpha0[maxDecNum][8] = {0};
	double initAlpha1[maxDecNum][8] = {0};
	int crcResult;

	//set blockSize (B) and initBlockSize (D)
	blockSize = decodeInLen/decNum;
	widNum_temp = blockSize/B;
	widNum = ((blockSize-B*widNum_temp) == 0) ? widNum_temp : (widNum_temp+1);

	//set initial alpha and beta value
	for (i=1; i<8; i++)
	{
		initBeta0_3BUF[decNum-1][widNum-1][i] = -32768;
		initBeta1_3BUF[decNum-1][widNum-1][i] = -32768;
		initAlpha0[0][i] = -32768;
		initAlpha1[0][i] = -32768;
	}

	//for each interleave size ,calculate some initial value
	Rf2 = (2*f2) % blockSize;
	Qf2 = BigMod(2*f2, decodeInLen) / blockSize;

	CalcInitAddr(0, f1, f2, &Req_A_init, &Req_RF_init, Req_QF_init, &Req_RG_init, Req_QG_init, decodeInLen, blockSize, decNum, 1);

	//save tail bits
	sysTailBit[0] = systemBit[decodeInLen];
	sysTailBit[1] = iParityBit[decodeInLen];
	sysTailBit[2] = parityBit[decodeInLen+1];
	sysTailBit[3] = systemBit[decodeInLen+2];
	sysTailBit[4] = iParityBit[decodeInLen+2];
	sysTailBit[5] = parityBit[decodeInLen+3];

	parTailBit[0] = parityBit[decodeInLen];
	parTailBit[1] = systemBit[decodeInLen+1];
	parTailBit[2] = iParityBit[decodeInLen+1];
	parTailBit[3] = parityBit[decodeInLen+2];
	parTailBit[4] = systemBit[decodeInLen+3];
	parTailBit[5] = iParityBit[decodeInLen+3];

	outputEn = 1;
	while (iter < iterMax)
	{
		iter++;

		//decoder 0
		SubDecode_SW_Subblock_3BUF(systemBit, parityBit, iParityBit, Le, blockSize, sysTailBit, parTailBit, decNum, outputEn, decodeOut, 0, 
			Req_A_init, Req_RF_init, Req_QF_init, Req_RG_init, Req_QG_init, Rf2, Qf2, B, normFactor, decMethod, initAlpha0, initBeta0_3BUF,Le_sys,Le_par0);

		crcResult = CrcCheck(decodeOut, decodeInLen, 24, 0);
		if(crcResult == 1)
		{
			break;
		}
		

		//decoder 1
		//outputEn = (iter == iterMax);
		SubDecode_SW_Subblock_3BUF(systemBit, parityBit, iParityBit, Le, blockSize, sysTailBit, parTailBit, decNum, outputEn, decodeOut, 1, 
			Req_A_init, Req_RF_init, Req_QF_init, Req_RG_init, Req_QG_init, Rf2, Qf2, B, normFactor, decMethod, initAlpha1, initBeta1_3BUF,Le_sys,Le_par1);
		
		crcResult = CrcCheck(decodeOut, decodeInLen, 24, 0);
		if(crcResult == 1)
		{
			break;
		}

	}
}

/*
********************************************************************************
* NAME:				SubDecode_SW_Subblock_3BUF
* PURPOSE:			���������㷨�������������������빦�ܡ���SubDecode_SW��TurboDecode_SW_Subblock��ͬ���ڣ��˴�ʡȥ��BPU_warm��һģ�飬��3·���еļ���
					��Ϊ��·(BPU��FPU)���ڽ�Լ������Դ������¿�����������
*
* Input:
systemBit:			�⽻֯��õ���ϵͳ����
parityBit0:			�⽻֯��õ���У����أ���Ӧ����������1�����У�����
parityBit1:			�⽻֯��õ���У����أ���Ӧ����������2�����У�����
Le:					�洢�⸽��Ϣ������
sysTailBit:			�洢ϵͳβ���ص�����
parTailBit:			�洢У��β���ص�����
decNum:				�������������������
outputEn:			�����������ָʾ�ź�
decMode:			����ģʽ��0->DEC 0���룬1->DEC 1����
initBlockSize:		��齻�����ֵĳ���D��D=0��ʾ���䲻��������Ҫ�ӿ�䴫����Ϣ��D!=0��ʾ���佻����ÿ����Ԥ���ֵ
normFactor:			����������Ϣ�ϵĳ���
betaValue:			�洢betaֵ������
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
blockSize:			��ǰ�������Ĵ�С
forwardRF��forwardQF��forwardRG��forwardQG��backwardRF��backwardQF��
backwardRG��backwardQG��forwardA��backwardA��Rf2��Qf2����ַ�������õ��ĳ�ֵ
initBeta:			�洢��ʼbetaֵ������
initAlpha:			�洢��ʼalphaֵ������
*
* Output:
decodeOut:			������
Le_sys:				�ṩ��demodulation��ϵͳ���ص�������Ϣ
Le_par:				�ṩ��demodulation��У����ص�������Ϣ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void SubDecode_SW_Subblock_3BUF(double *systemBit, double *parityBit0, double *parityBit1, double *Le, int blockSize,
				  double *sysTailBit, double *parTailBit,   int decNum, int outputEn, int *decodeOut, int decMode, 
				  int Req_A_init, int Req_Rf_init, int *Req_Qf_init, int Req_Rg_init, int *Req_Qg_init, int Rf2, int Qf2, 
				  int B, double normFactor, int decMethod, double initAlpha[][8], double initBeta_3BUF[][maxWinNum][8],double *Le_sys,double *Le_par)
{//double initBeta_3BUF[][maxWinNum][8]
	double sysBit[maxDecNum] = {0};	
	double parBit[maxDecNum] = {0};	
	double La[maxDecNum] = {0};	
	double betaValueIn[maxDecNum][8] = {0};
	double alphaValueIn[maxDecNum][8] = {0};
	double BPU_alphaValueIn[maxDecNum][8] = {0};
	int Req_Rf[maxDecNum] = {0};	//ǰ��ȡ����Ԫ��ַ
	int Req_Qf[maxDecNum] = {0};
	int Req_Rg[maxDecNum] = {0};
	int Req_Qg[maxDecNum] = {0};
	int Req_A = 0;
	int Le_Rf_init_1[maxDecNum] = {0}, Le_Rf_init_2[maxDecNum] = {0}, Le_Rf_init_3[maxDecNum] = {0};	//Leд�ĳ�ʼ��ַ
	int Le_Qf_init_1[maxDecNum] = {0}, Le_Qf_init_2[maxDecNum] = {0}, Le_Qf_init_3[maxDecNum] = {0};
	int Le_Rg_init_1[maxDecNum] = {0}, Le_Rg_init_2[maxDecNum] = {0}, Le_Rg_init_3[maxDecNum] = {0};
	int Le_Qg_init_1[maxDecNum] = {0}, Le_Qg_init_2[maxDecNum] = {0}, Le_Qg_init_3[maxDecNum] = {0};
	int Le_A_init_1 = 0, Le_A_init_2=0, Le_A_init_3=0;
	int Le_Rf[maxDecNum] = {0};	//Leд��ַ
	int Le_Qf[maxDecNum] = {0};
	int Le_Rg[maxDecNum] = {0};
	int Le_Qg[maxDecNum] = {0};
	int Le_A=0;
	int Le_w_ad=0;
	int Le_w_sort[maxDecNum]= {0};
	double BUF0_SYS[maxDecNum][128]= {0}, BUF0_PAR[maxDecNum][128]= {0}, BUF0_LA[maxDecNum][128]= {0};
	double BUF1_SYS[maxDecNum][128]= {0}, BUF1_PAR[maxDecNum][128]= {0}, BUF1_LA[maxDecNum][128]= {0};
	double BUF2_SYS[maxDecNum][128]= {0}, BUF2_PAR[maxDecNum][128]= {0}, BUF2_LA[maxDecNum][128]= {0};
	double BUF3_SYS[maxDecNum][128]= {0}, BUF3_PAR[maxDecNum][128]= {0}, BUF3_LA[maxDecNum][128]= {0};
	double BUF_A[maxDecNum][128*8]= {0};
	int BPU_Warm_Ad=0, FPU_Ad=0, BPU_Ad=0;
	int BUF_A_Ad, BUF_A_dir;
	int BUF0_Ad, BUF0_dir, BUF0_Flag;
	int BUF1_Ad, BUF1_dir, BUF1_Flag;
	int BUF2_Ad, BUF2_dir, BUF2_Flag;
	int BUF3_Ad, BUF3_dir;
	double initBeta_3BUF_temp[maxDecNum][maxWinNum][8] = {0};
	double initAlpha_temp[maxDecNum][8] = {0};
	int AddrSL = 0;
	int AddrP = 0;
	int resortP[maxDecNum] = {0};
	int resortSL[maxDecNum] = {0};
	double LeOut[maxDecNum] = {0};
	double Le_sysOut[maxDecNum] = {0};
	double Le_parOut[maxDecNum] = {0};

	int LLR[maxDecNum] = {0};
	int blockIndex = 0;
	int BPU_Warm_initFlag = 0;
	int BPU_initFlag = 0;
	int FPU_initFlag = 0;
	int Tail_initFlag = 0;
	int CmpCounter = 0;
	int CmpLen = 0;
	double sysTail = 0;
	double parTail = 0;
	double *parityBit = NULL;
	int i = 0, r=0, q=0, q_up=0;

	int q_temp=0;
	int B_temp=0;
	int BUF_Write_Pos = 0;
	double BUF0_sysBit[maxDecNum]= {0}, BUF0_parBit[maxDecNum]= {0}, BUF0_La[maxDecNum]= {0};
	double BUF1_sysBit[maxDecNum]= {0}, BUF1_parBit[maxDecNum]= {0}, BUF1_La[maxDecNum]= {0};
	double BUF2_sysBit[maxDecNum]= {0}, BUF2_parBit[maxDecNum]= {0}, BUF2_La[maxDecNum]= {0};
	double BUF3_sysBit[maxDecNum]= {0}, BUF3_parBit[maxDecNum]= {0}, BUF3_La[maxDecNum]= {0};
	double BPU_Warm_sysBit[maxDecNum]= {0}, BPU_Warm_parBit[maxDecNum]= {0}, BPU_Warm_La[maxDecNum]= {0};	
	double BPU_sysBit[maxDecNum]= {0}, BPU_parBit[maxDecNum]= {0}, BPU_La[maxDecNum]= {0};	
	double FPU_sysBit[maxDecNum]= {0}, FPU_parBit[maxDecNum]= {0}, FPU_La[maxDecNum]= {0};	
	double BPU_Warm_initBeta[maxDecNum][8] = {0}, BPU_Warm_betaValueIn[maxDecNum][8] = {0};
	double BPU_initBeta[maxDecNum][8] = {0}, BPU_betaValueIn[maxDecNum][8] = {0};
	double BPU_Warm_tempBeta[maxDecNum][8] = {0};

	////////add for various code length/////// 
	int B_value;//for the last window, the computing length will either be B or r
	int final_BUF_flag[3];//for the last window's BPU,if final_BUF_flag[i] == 1,then BUFi is be read for the last window's BPU

	//select parity bit
	//decMode = 0, DEC 0 is working, need parityBit0
	//decMode = 1, DEC 1 is working, need parityBit1
	if (0 == decMode)
	{
		parityBit = parityBit0;
	}
	else
	{
		parityBit = parityBit1;
	}

	q = blockSize/B;
	r = blockSize - q*B;
	q_up = (r == 0) ? q : (q+1);
	CmpLen = (q_up+2)*B;
	/**********************************************************************************************************
																				Iterate Compute
	**********************************************************************************************************/
	//compute tail bits
	for (CmpCounter=2; CmpCounter>=0; CmpCounter--)
	{
		sysTail = sysTailBit[CmpCounter+3*decMode];
		parTail = parTailBit[CmpCounter+3*decMode];
		Tail_initFlag = 0;
		if (CmpCounter == 2)
		{
			Tail_initFlag = 1;
		}
		BPU_Warm(sysTail, parTail, 0, Tail_initFlag, initBeta_3BUF[decNum-1][q_up-1], betaValueIn[decNum-1], decMethod);
	}
	for (i=0; i<8; i++)
	{
		initBeta_3BUF[decNum-1][q_up-1][i] = betaValueIn[decNum-1][i];//��3��βbits�õ����һ�������������������һ��������beta��ֵ
	}


	//perform slide window map decoding, widows size is B, without overlap
	for(q_temp=0;q_temp<(q_up+2);q_temp++){

		B_value = ((q_temp == q_up+1) && (r != 0)) ? r : B; //if r!=0, r is the length of the last slide window

		for(B_temp=0;B_temp<B_value;B_temp++){

			CmpCounter = q_temp * B +B_temp;

			/***********enable the address_control unit*************************/
			//set request address.Req_A��Req_Rf��Req_Qf��Req_Rg��Req_Qg ��CmpCounter+1ʱ�����Ӧ��AddrP��resortP��AddrSL��resortSL ��CmpCounter��Ӧ
			CalcReadAddr_3BUF(Req_A_init, Req_Rf_init, Req_Qf_init, Req_Rg_init, Req_Qg_init, CmpCounter, &Req_A, Req_Rf, Req_Qf,
				Req_Rg, Req_Qg, decNum, blockSize, decMode, Rf2, Qf2, &AddrP, resortP, &AddrSL, resortSL);//��'CmpCounter=0~(q_up*B-1)����Ҫ����
				/*printf("%d\n",AddrSL);
				for(i=0;i<maxDecNum;i++){
					printf("%d\t",resortSL[i]);
				}
				printf("\n");*/

			//save initial le write address.ֻ��xB-2��xB-3ʱ����/////////////////////////////////////////////////////////////////////////////3����Ҫ�޸�/////
			SaveLeInitWriteAddr_3BUF(Le_Rf_init_1,Le_Rf_init_2, Le_Qf_init_1,Le_Qf_init_2, Le_Rg_init_1,Le_Rg_init_2, 
				Le_Qg_init_1,Le_Qg_init_2, &Le_A_init_1,&Le_A_init_2,
				Req_Rg, Req_Qg, Req_Rf, Req_Qf, Req_A, CmpCounter, B, r, q_up, decNum, blockSize);

			//set le write address.��'CmpCounter=3B~(CmpLen-1)����Ҫ����/////////////////////////////////////////////////////////////////////////////4����Ҫ�޸�/////
			CalcLeWriteAddr_3BUF(Le_Rf_init_2, Le_Qf_init_2, Le_Rg_init_2, Le_Qg_init_2, Le_A_init_2, resortP, Le_Rf, Le_Qf, Le_Rg, Le_Qg,
				&Le_A, CmpCounter, B, decMode, q_up, r, decNum, blockSize, Rf2, Qf2, &Le_w_ad, Le_w_sort);
				/*printf("%d\n",Le_w_ad);
				for(i=0;i<maxDecNum;i++){
					printf("%d\t",Le_w_sort[i]);
				}
				printf("\n");*/

			/***********the address_control unit has finished its work***********/

			/***********set the directions of 5 data buffers*********************//////////////////////////////////////////////////////////////////////////////1����Ҫ�޸�/////
			//set BUF  and dir.��'CmpCounter=0~(CmpLen-1)'����Ҫ����
			//CalcBufAddr(0, CmpCounter, B, &BUF_A_Ad, &BUF_A_dir);//BUF_A�ķ���Ϊ010101...
			//CalcBufAddr_3BUF(1, 1, CmpCounter, B, &BUF0_Ad, &BUF0_dir, &BUF0_Flag);//BUF_0�ķ���Ϊ11000111000...
			//CalcBufAddr_3BUF(0, 0, CmpCounter, B, &BUF1_Ad, &BUF1_dir, &BUF1_Flag);//BUF_1�ķ���Ϊ00011100011...
			//CalcBufAddr_3BUF(0, 2, CmpCounter, B, &BUF2_Ad, &BUF2_dir, &BUF2_Flag);//BUF_2�ķ���Ϊ01110001110...
			for(i = 0;i < 3;i++)
			{
				final_BUF_flag[i] = ((q_up+2)%3 == i) ? 1 : 0;
			}
			CalcBufAddr_BUFA(0, CmpCounter, B, &BUF_A_Ad, &BUF_A_dir, q_up, r);//BUF_A�ķ���Ϊ010101...
			CalcBufAddr_3BUF(1, 1, CmpCounter, B, &BUF0_Ad, &BUF0_dir, &BUF0_Flag, final_BUF_flag[0], q_up, r);//BUF_0�ķ���Ϊ11000111000...
			CalcBufAddr_3BUF(0, 0, CmpCounter, B, &BUF1_Ad, &BUF1_dir, &BUF1_Flag, final_BUF_flag[1], q_up, r);//BUF_1�ķ���Ϊ00011100011...
			CalcBufAddr_3BUF(0, 2, CmpCounter, B, &BUF2_Ad, &BUF2_dir, &BUF2_Flag, final_BUF_flag[2], q_up, r);//BUF_2�ķ���Ϊ01110001110...
			/***********finished*************************************************//////////////////////////////////////////////////////////////////////////////1����Ҫ�޸�/////
			/*printf("%d\t",BUF_A_dir);printf("%d\n",BUF_A_Ad);
			printf("%d\t",BUF0_dir);printf("%d\n",BUF0_Ad);
			printf("%d\t",BUF1_dir);printf("%d\n",BUF1_Ad);
			printf("%d\t",BUF2_dir);printf("%d\n",BUF2_Ad);printf("\n");*/

			/************set initFlags for BPU��FPU*****************************/
			BPU_initFlag = 0;
			FPU_initFlag = 0;

			if (CmpCounter == B)
			{
				FPU_initFlag = 1;
			}

			//if (CmpCounter % B == 0 || ((CmpCounter+r) == (q_up+2)*B && r != 0))/////////////////////////////////////////////////////////////////////////////2����Ҫ�޸�/////
			if (CmpCounter % B == 0)
			{
				BPU_initFlag = 1;
			}																	/////////////////////////////////////////////////////////////////////////////2����Ҫ�޸�/////
			/***********finished*************************************************/

			/***********save system,parity and La data to the proper buffer******/
			//read system,parity and La data from 8 rams
			for (blockIndex=0; blockIndex<decNum; blockIndex++)
			{
				sysBit[blockIndex] = systemBit[blockIndex*blockSize + AddrSL];
				parBit[blockIndex] = parityBit[blockIndex*blockSize + AddrP];
				La[blockIndex] = Le[blockIndex*blockSize + AddrSL];
			}

			//reorder all read bits
			Reorder(sysBit, decNum, resortSL, 0);
			Reorder(La, decNum, resortSL, 0);
			Reorder(parBit, decNum, resortP, 0);

			//save to BUF0~BUF3, the 3 buffers are shared by all the parallel decoders
			if (BUF_Write_Pos == 3*B)
			{
				BUF_Write_Pos = 0;
			}
			SaveBUF_3BUF(BUF_Write_Pos, B, decNum, sysBit, parBit, La, BUF0_SYS, BUF0_PAR, BUF0_LA,
				BUF1_SYS, BUF1_PAR, BUF1_LA,BUF2_SYS, BUF2_PAR, BUF2_LA,BUF0_dir, BUF1_dir, BUF2_dir);
			//for(blockIndex=0; blockIndex<decNum; blockIndex++){
				//printf("%f\t",BUF0_SYS[blockIndex][BUF_Write_Pos]);
			//}
			BUF_Write_Pos++;

			//Read BUF0~BUF3 to the buf0~buf3 of parallel decoders
			for (blockIndex=0; blockIndex<decNum; blockIndex++)
			{
				BUF0_sysBit[blockIndex] = BUF0_SYS[blockIndex][BUF0_Ad];//printf("%f\t",BUF0_sysBit[blockIndex]);
				BUF0_parBit[blockIndex] = BUF0_PAR[blockIndex][BUF0_Ad];
				BUF0_La[blockIndex] = BUF0_LA[blockIndex][BUF0_Ad];

				BUF1_sysBit[blockIndex] = BUF1_SYS[blockIndex][BUF1_Ad];
				BUF1_parBit[blockIndex] = BUF1_PAR[blockIndex][BUF1_Ad];
				BUF1_La[blockIndex] = BUF1_LA[blockIndex][BUF1_Ad];

				BUF2_sysBit[blockIndex] = BUF2_SYS[blockIndex][BUF2_Ad];
				BUF2_parBit[blockIndex] = BUF2_PAR[blockIndex][BUF2_Ad];
				BUF2_La[blockIndex] = BUF2_LA[blockIndex][BUF2_Ad];
			}
			/***********finished*************************************************/

			/*******read the alpha_value of to BPU,and write the new alpha_value to BUF_A*****/
			//read alpha value in forward computation
			for (blockIndex=0; blockIndex<decNum; blockIndex++)
			{
				BPU_alphaValueIn[blockIndex][0] = BUF_A[blockIndex][BUF_A_Ad*8+0];
				BPU_alphaValueIn[blockIndex][1] = BUF_A[blockIndex][BUF_A_Ad*8+1];
				BPU_alphaValueIn[blockIndex][2] = BUF_A[blockIndex][BUF_A_Ad*8+2];
				BPU_alphaValueIn[blockIndex][3] = BUF_A[blockIndex][BUF_A_Ad*8+3];
				BPU_alphaValueIn[blockIndex][4] = BUF_A[blockIndex][BUF_A_Ad*8+4];
				BPU_alphaValueIn[blockIndex][5] = BUF_A[blockIndex][BUF_A_Ad*8+5];
				BPU_alphaValueIn[blockIndex][6] = BUF_A[blockIndex][BUF_A_Ad*8+6];
				BPU_alphaValueIn[blockIndex][7] = BUF_A[blockIndex][BUF_A_Ad*8+7];
			}

			//save alpha value in backward computation
			for (blockIndex=0; blockIndex<decNum; blockIndex++)
			{
				BUF_A[blockIndex][BUF_A_Ad*8+0] = FPU_initFlag ? initAlpha[blockIndex][0] : alphaValueIn[blockIndex][0];
				BUF_A[blockIndex][BUF_A_Ad*8+1] = FPU_initFlag ? initAlpha[blockIndex][1] : alphaValueIn[blockIndex][1];
				BUF_A[blockIndex][BUF_A_Ad*8+2] = FPU_initFlag ? initAlpha[blockIndex][2] : alphaValueIn[blockIndex][2];
				BUF_A[blockIndex][BUF_A_Ad*8+3] = FPU_initFlag ? initAlpha[blockIndex][3] : alphaValueIn[blockIndex][3];
				BUF_A[blockIndex][BUF_A_Ad*8+4] = FPU_initFlag ? initAlpha[blockIndex][4] : alphaValueIn[blockIndex][4];
				BUF_A[blockIndex][BUF_A_Ad*8+5] = FPU_initFlag ? initAlpha[blockIndex][5] : alphaValueIn[blockIndex][5];
				BUF_A[blockIndex][BUF_A_Ad*8+6] = FPU_initFlag ? initAlpha[blockIndex][6] : alphaValueIn[blockIndex][6];
				BUF_A[blockIndex][BUF_A_Ad*8+7] = FPU_initFlag ? initAlpha[blockIndex][7] : alphaValueIn[blockIndex][7];
			}
			/***********finished*************************************************/

			/***********set the control signals of BPU_warm��BPU��FPU************/
			//select the proper buffer for BPU_warm��BPU��FPU units
			if (CmpCounter % B == 0)
			{
				if (0 == CmpCounter)
				{
					FPU_Ad = 2;
				}
				else if (2 == FPU_Ad)
				{
					FPU_Ad = 0;
				}
				else
				{
					FPU_Ad++;
				}
			}

			if (CmpCounter % B == 0)
			{
				if (0 == CmpCounter)
				{
					BPU_Ad = 1;
				}
				else if (2 == BPU_Ad)
				{
					BPU_Ad = 0;
				}
				else
				{
					BPU_Ad++;
				}
			}

			SelectBUF_3BUF(FPU_Ad, decNum, FPU_sysBit, FPU_parBit, FPU_La, BUF0_sysBit, BUF0_parBit, BUF0_La,
				BUF1_sysBit, BUF1_parBit, BUF1_La, BUF2_sysBit, BUF2_parBit, BUF2_La);

			SelectBUF_3BUF(BPU_Ad, decNum, BPU_sysBit, BPU_parBit, BPU_La, BUF0_sysBit, BUF0_parBit, BUF0_La,
				BUF1_sysBit, BUF1_parBit, BUF1_La, BUF2_sysBit, BUF2_parBit, BUF2_La);
			/***********finished*************************************************/

			/***********set init_value of BPU_warm��BPU��FPU************//////////////////////////////////////////////////////////////////////////////5����Ҫ�޸�/////
			//for BPU
			//if ((CmpCounter % B == 0 || ((CmpCounter+r) == (q_up+2)*B && r != 0)) && q_temp>= 2)
			if (CmpCounter % B == 0 && q_temp>= 2)
			{
				for(blockIndex=0; blockIndex<decNum; blockIndex++){
					for(i=0; i<8; i++){
						BPU_initBeta[blockIndex][i] = initBeta_3BUF[blockIndex][q_temp-2][i];
					}
				}
			}
			/***********finished*************************************************//////////////////////////////////////////////////////////////////////////////5����Ҫ�޸�/////

			/***********enable BPU_warm��BPU��FPU********************************/
			for (blockIndex=0; blockIndex<decNum; blockIndex++)
			{
				FPU(FPU_sysBit[blockIndex], FPU_parBit[blockIndex], FPU_La[blockIndex], FPU_initFlag, 
					initAlpha[blockIndex],	alphaValueIn[blockIndex], decMethod);

				BPU(BPU_sysBit[blockIndex], BPU_parBit[blockIndex], BPU_La[blockIndex], BPU_initFlag, BPU_initBeta[blockIndex],	
					betaValueIn[blockIndex], BPU_alphaValueIn[blockIndex], normFactor, &LeOut[blockIndex], &LLR[blockIndex],
					&Le_sysOut[blockIndex],&Le_parOut[blockIndex], decMethod);
			}
			//if (B_temp == B-1 && q_temp>= 2)/////////////////////////////////////////////////////////////////////////////6����Ҫ�޸�/////
			if (q_temp>= 2 && ((r == 0 && B_temp == B-1) ||(r != 0 &&((B_temp == B-1 && q_temp <=q_up ) || (B_temp == r-1 && q_temp == q_up+1)))))
			{
				if(q_temp == 2){//Ϊǰһ�����������������һ����������ֵ
					for(blockIndex=0; blockIndex<decNum-1; blockIndex++){
						for(i=0; i<8; i++){
							initBeta_3BUF[blockIndex][q_up-1][i] = betaValueIn[blockIndex+1][i];
						}
					}
				}
				else{//Ϊ��������������ǰһ����������ֵ
					for(blockIndex=0; blockIndex<decNum; blockIndex++){
						for(i=0; i<8; i++){
							initBeta_3BUF[blockIndex][q_temp-3][i] = betaValueIn[blockIndex][i];
						}
					}
				}
			}/////////////////////////////////////////////////////////////////////////////6����Ҫ�޸�/////
			/***********finished*************************************************/

			/***********write the decoded results to ram*************************/
			//reorder Le and LLR
			Reorder(LeOut, decNum, Le_w_sort, 1);
			ReorderI(LLR, decNum, Le_w_sort, 1);
			Reorder(Le_sysOut, decNum, Le_w_sort, 1);
			Reorder(Le_parOut, decNum, resortP, 1);
			/*�˴���Ҫע�⣬��ΪУ��λʼ���ǰ�˳�����������������ģ����Բ���Ҫ���ţ�Ҫ����Ҳ����resortP*/

			//save Le
			if (CmpCounter >= 2*B)
			{
				for (blockIndex=0; blockIndex<decNum; blockIndex++)
				{
					Le[blockIndex*blockSize+Le_w_ad] = LeOut[blockIndex];
					Le_sys[blockIndex*blockSize+Le_w_ad] = Le_sysOut[blockIndex];
					Le_par[blockIndex*blockSize+((q_temp-2)*B)+(B-B_temp-1)] = Le_parOut[blockIndex];
					/*�˴���Ҫע�⣬��ΪУ��λʼ���ǰ�˳��Ӵ洢���ж�ȡ�ģ����ҲҪ��˳��д�룬����д��ʱҪ����BPU�ļ��㷽����ͬһ���������ǴӺ���ǰ��.�ⲿ����Ҫ�ٽ�֯*/
				}
			}

			//output decode result
			if (outputEn == 1 && CmpCounter >= 2*B)
			{
				for (blockIndex=0; blockIndex<decNum; blockIndex++)
				{
					decodeOut[blockIndex*blockSize+Le_w_ad] = LLR[blockIndex];
				}
			}
			/***********finished*************************************************/

			/***********save the alpha and beta values for the next innerIT******/
			//reset initial beta value
/*			if (CmpCounter == (4*B-1))
			{
				for (blockIndex=0; blockIndex<decNum-1; blockIndex++)
				{
					for (i=0; i<8; i++)
					{
						initBeta_temp[blockIndex][i] = betaValueIn[blockIndex+1][i];
					}
				}
			}
*/
			//reset initial alpha value
//			if (CmpCounter == (blockSize+2*B-1))
			//if (CmpCounter == (blockSize+B-1))/////////////////////////////////////////////////////////////////////////////7����Ҫ�޸�/////
			if (CmpCounter == (blockSize+B-1))
			{
				for (blockIndex=1; blockIndex<decNum; blockIndex++)
				{
					for (i=0; i<8; i++)
					{
						initAlpha_temp[blockIndex][i] = alphaValueIn[blockIndex-1][i];
					}
				}
			}
			/***********finished*************************************************/


		}
	}

/*	initBeta[decNum-1][0] = 0;
	for (i=1; i<8; i++)
	{
		initBeta[decNum-1][i] = -32768;
	}
	//reset initial beta value for the next innerIT of this subdecoder
	for (blockIndex=0; blockIndex<decNum-1; blockIndex++)
	{
		for (i=0; i<8; i++)
		{
			initBeta[blockIndex][i] = initBeta_temp[blockIndex][i];
		}
	}
*/
	initBeta_3BUF[decNum-1][q_up-1][0] = 0;
	for (i=1; i<8; i++)
	{
		initBeta_3BUF[decNum-1][q_up-1][i] = -32768;
	}
	//reset initial alpha value for the next innerIT of this subdecoder
	for (blockIndex=1; blockIndex<decNum; blockIndex++)
	{
		for (i=0; i<8; i++)
		{
			initAlpha[blockIndex][i] = initAlpha_temp[blockIndex][i];
		}
	}
}

/*
********************************************************************************
* NAME:				CalcInitAddr
* PURPOSE:			����dir���ݹ����ǰ��ͺ����Rf��Qf��Rg��Qg

* Input:
systemBit:			�⽻֯�������ϵͳ�����������λ�������0
parityBit:			�⽻֯�������У�����������Ӧ����������1�����У����أ����λ�������0
iParityBit:			�⽻֯�������У�����������Ӧ����������2�����У����أ����λ�������0
decodeInLen:		�������ĳ���
f1:					QPP��֯���Ĳ���
f2:					QPP��֯���Ĳ���
decNum:				�������������������
iterMax:			Ԥ�����������������
initBlockSize:		��齻�����ֵĳ���
normFactor:			����������Ϣ�ϵĳ���
betaValue:			�洢betaֵ������
decMethod:			�����㷨��0->Max-Log-MAP��1->Log-MAP

* Output:
decodeOut:			��ǰ�������Ľ�����
********************************************************************************
*/
void CalcInitAddr(int x, int f1, int f2, int *A, int *Rf, int *Qf, int *Rg, int *Qg, int codeSize, int blockSize, int decNum, int dir)
{
	int blockIndex = 0;
	int normalAddr = 0;
	int iTemp1, iTemp2 = 0;

	for (blockIndex=0; blockIndex<decNum; blockIndex++)
	{
		normalAddr = blockIndex*blockSize+x;
		iTemp1 = BigMod(1.0*f1*normalAddr + 1.0*f2*normalAddr*normalAddr, codeSize);
		iTemp2 = (dir == 0) ? BigMod(f1 + f2 + 2.0*f2*(normalAddr-1), codeSize) :  BigMod(f1 + f2 + 2.0*f2*normalAddr, codeSize);
		*Rf = iTemp1 % blockSize;
		Qf[blockIndex] = iTemp1 / blockSize;
		*Rg = iTemp2 % blockSize;
		Qg[blockIndex] = iTemp2 / blockSize;
	}
	*A = x;
}

/*
********************************************************************************
* NAME:				CalcReadAddr_3BUF
* PURPOSE:			����dir���ݹ����ǰ��ͺ����Rf��Qf��Rg��Qg

* Input:
Req_A_init:			����ÿ�����е���������������ǰʱ���������bit��������Ҫ�����С����еľ���ƫ��λ�õĳ�ʼֵ��Ϊ0
Req_Rf_init:		ÿ�������������ĵݹ����QPP������Rf�ĳ�ʼֵ�����飩
Req_Qf_init:		ÿ�������������ĵݹ����QPP������Qf�ĳ�ʼֵ�����飩
Req_Rg_init:		ÿ�������������ĵݹ����QPP������Rg�ĳ�ʼֵ�����飩
Req_Qg_init:		ÿ�������������ĵݹ����QPP������Qg�ĳ�ʼֵ�����飩
CmpCounter:			��¼�ѽ��������bits����0~((q_up+3)*B)-1
decNum:				�������������������
BlockSize:			ÿ�����ĳ���
decMode:			������ĸ���������DEC0��DEC1��
Rf2:				Rf2��ֵ
Qf2:				Qf2��ֵ

* Output:
Req_A:				����ÿ�����е���������������һʱ�����õ�bit��������Ҫ�����С����еľ���ƫ��λ��
Req_Rf:				��һʱ�������Rf
Req_Qf:				��һʱ�������Qf[decNum]
Req_Rg:				��һʱ�������Rg
Req_Qg:				��һʱ�������Qg[decNum]
AddrP:				˳������ĵ�ַƫ������������DEC0����ʱ��ϵͳ��У��λ���Լ�DEC1����ʱ��У��λ
resortP:			��AddrP���׵�decNum��ֵ��Ӧ��decNum������������������ţ�˳��
AddrSL:				��������ĵ�ַƫ������������DEC1����ʱ��ϵͳλ
resortSL:			��AddrSL���׵�decNum��ֵ��Ӧ��decNum������������������ţ�����
*
* AUTHOR: 			Duck
********************************************************************************
*/
void CalcReadAddr_3BUF(int Req_A_init, int Req_Rf_init, int *Req_Qf_init, int Req_Rg_init, int *Req_Qg_init, int CmpCounter,
				  int *Req_A, int *Req_Rf, int *Req_Qf, int *Req_Rg, int *Req_Qg, int decNum, int blockSize, int decMode,
				 int Rf2, int Qf2, int *AddrP, int *resortP, int *AddrSL, int *resortSL)
{
	int blockIndex = 0;
	int i = 0;

	if (0 == CmpCounter)
	{
		for (i=0; i<decNum; i++)
		{
			Req_Rf[i] = Req_Rf_init;
			Req_Qf[i] = Req_Qf_init[i];
			Req_Rg[i] = Req_Rg_init;
			Req_Qg[i] = Req_Qg_init[i];
		}
		*Req_A = Req_A_init;
	}

	//compute parity bit resort array in every clock
	for (i=0; i<decNum; i++)
	{
		resortP[i] = i;
	}

	//generate address and set resort array
	if (!decMode)
	{
		*AddrSL = *Req_A;
		memcpy(resortSL, resortP, sizeof(int)*decNum);
	}
	else
	{
		*AddrSL = Req_Rf[1];
		if (1 == decNum)
		{
			*AddrSL = Req_Rf[0];
		}
		memcpy(resortSL, Req_Qf, sizeof(int)*decNum);
	}
	*AddrP = *Req_A;

	//compute next Req address
	*Req_A = (*Req_A + 1) % blockSize;
	for (blockIndex=0; blockIndex<decNum; blockIndex++)
	{
		AddrCal1Step(&Req_Rf[blockIndex], &Req_Qf[blockIndex], &Req_Rg[blockIndex], &Req_Qg[blockIndex], Rf2, Qf2, 1,
			blockSize, decNum);
	}
}

/*
********************************************************************************
* NAME:				SaveLeInitWriteAddr_3BUF
* PURPOSE:			���������������Ϣ�ĵ�ַ�ĳ�ʼֵ

* Input:
Req_Rg:				��ʱ�������Rg
Req_Qg:				��ʱ�������Qg[decNum]
Req_Rf:				��ʱ�������Rf
Req_Qf:				��ʱ�������Qf[decNum]
Req_A:				��ǰʱ�����õ�bit��������Ҫ�����С����еľ���ƫ��λ�ã�decNum����
CmpCounter:			����ֵ
B:					������С
r:					r = blockSize - q*B
q_up:				��¼���ڵĻ������
decNum:				�����������ĸ���
blockSize:			ÿ��������������������������ĳ���

* Output:
Le_Rf_init_1~2:		�洢������Ϣ�ĵ�ַ��Rf��ֵ
Le_Qf_init_1~2:		�洢������Ϣ�ĵ�ַ��Qf[decNum]��ֵ
Le_Rg_init_1~2:		�洢������Ϣ�ĵ�ַ��Rg��ֵ
Le_Qg_init_1~2:		�洢������Ϣ�ĵ�ַ��Qg[decNum]��ֵ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void SaveLeInitWriteAddr_3BUF(int *Le_Rf_init_1, int *Le_Rf_init_2, int *Le_Qf_init_1, int *Le_Qf_init_2, int *Le_Rg_init_1, int *Le_Rg_init_2,
							  int *Le_Qg_init_1, int *Le_Qg_init_2, int *Le_A_init_1, int *Le_A_init_2, int *Req_Rg, int *Req_Qg, int *Req_Rf,
							  int *Req_Qf, int Req_A,int CmpCounter, int B, int r, int q_up, int decNum, int blockSize)
{
	/*int blockIndex = 0;
	int i = 0;

	//set le write address
	if ((CmpCounter+3)%B == 0 || CmpCounter == (blockSize-3))
	{
		if (r == 0 || (CmpCounter+3) != (q_up)*B)
		{
			for (i=0; i<decNum; i++)
			{
				Le_Rg_init_2[i] = Le_Rg_init_1[i];
				Le_Rg_init_1[i] = Req_Rg[i];
				Le_Qg_init_2[i] = Le_Qg_init_1[i];
				Le_Qg_init_1[i] = Req_Qg[i];
			}
		}
	}
	if ((CmpCounter+2)%B == 0 || CmpCounter == (blockSize-2))
	{
		if (r == 0 || (CmpCounter+2) != (q_up)*B)
		{
			*Le_A_init_2 = *Le_A_init_1;
			*Le_A_init_1 = Req_A;
			for (i=0; i<decNum; i++)
			{
				Le_Rf_init_2[i] = Le_Rf_init_1[i];
				Le_Rf_init_1[i] = Req_Rf[i];
				Le_Qf_init_2[i] = Le_Qf_init_1[i];
				Le_Qf_init_1[i] = Req_Qf[i];
			}
		}
	}*/

	//change for various code length
	int blockIndex = 0;
	int i = 0;

	//set le write address
	if ((r == 0 && (CmpCounter+3)%B == 0) || (r != 0 && (CmpCounter == (blockSize-3) || ((CmpCounter+3)%B == 0) && (CmpCounter+3 != q_up*B))))
	{
		for (i=0; i<decNum; i++)
		{
			Le_Rg_init_2[i] = Le_Rg_init_1[i];
			Le_Rg_init_1[i] = Req_Rg[i];
			Le_Qg_init_2[i] = Le_Qg_init_1[i];
			Le_Qg_init_1[i] = Req_Qg[i];
		}
	}
	if ((r == 0 && (CmpCounter+2)%B == 0) || (r != 0 && (CmpCounter == (blockSize-2) || ((CmpCounter+2)%B == 0) && (CmpCounter+2 != q_up*B))))
	{
		*Le_A_init_2 = *Le_A_init_1;
		*Le_A_init_1 = Req_A;
		for (i=0; i<decNum; i++)
		{
			Le_Rf_init_2[i] = Le_Rf_init_1[i];
			Le_Rf_init_1[i] = Req_Rf[i];
			Le_Qf_init_2[i] = Le_Qf_init_1[i];
			Le_Qf_init_1[i] = Req_Qf[i];
		}
	}
}


/*
********************************************************************************
* NAME:				CalcLeWriteAddr_3BUF
* PURPOSE:			���Ƽ��������������Ϣ�ĵ�ֵַ

* Input:
Le_Rf_init_2:		�洢������Ϣ�ĵ�ַ��Rf��ֵ
Le_Qf_init_2:		�洢������Ϣ�ĵ�ַ��Qf[decNum]��ֵ
Le_Rg_init_2:		�洢������Ϣ�ĵ�ַ��Rg��ֵ
Le_Qg_init_2:		�洢������Ϣ�ĵ�ַ��Qg[decNum]��ֵ
Le_A_init_2:		�洢������Ϣ�ĵ�ַ��ƫ�Ƴ�ֵ
resortP:			��AddrP���׵�decNum��ֵ��Ӧ��decNum������������������ţ�˳��
CmpCounter:			����ֵ
B:					������С
r:					r = blockSize - q*B
q_up:				��¼���ڵĻ������
decNum:				�����������ĸ���
blockSize:			ÿ��������������������������ĳ���
Rf2:				((2*f2)%L)%W
Qf2:				((2*f2)%L)/W

* Output:
Le_Rf:				�洢������Ϣ�ĵ�ַ��Rfֵ
Le_Qf:				�洢������Ϣ�ĵ�ַ��Qf[decNum]ֵ
Le_Rg:				�洢������Ϣ�ĵ�ַ��Rgֵ
Le_Qg:				�洢������Ϣ�ĵ�ַ��Qg[decNum]ֵ
Le_A:				��ַ����
Le_w_ad:			�洢������Ϣ�ĵ�ַ��ƫ��ֵ
Le_w_sort:			��Le_w_ad���׵�decNum������Ӧ��decNum����������������ֵ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void CalcLeWriteAddr_3BUF(int *Le_Rf_init_2, int *Le_Qf_init_2, int *Le_Rg_init_2, int *Le_Qg_init_2, int Le_A_init_2, int *resortP,
						int *Le_Rf, int *Le_Qf, int *Le_Rg, int *Le_Qg,  int *Le_A, int CmpCounter, int B, int decMode, int q_up, int r,
						int decNum, int blockSize, int Rf2, int Qf2, int *Le_w_ad, int *Le_w_sort)
{
/*	int blockIndex = 0;
	int i = 0;

	//init
	if (CmpCounter % B == 0 || ((CmpCounter+r) == (q_up+2)*B && r != 0))
	{
		*Le_A = Le_A_init_2;
		for (i=0; i<decNum; i++)
		{
			Le_Rf[i] = Le_Rf_init_2[i];
			Le_Qf[i] = Le_Qf_init_2[i];
			Le_Rg[i] = Le_Rg_init_2[i];
			Le_Qg[i] = Le_Qg_init_2[i];
		}
	}
	if (!decMode)
	{
		*Le_w_ad = *Le_A;
		memcpy(Le_w_sort, resortP, sizeof(int)*decNum);
	}
	else
	{
		*Le_w_ad = Le_Rf[1];
		if (1 == decNum)
		{
			*Le_w_ad = Le_Rf[0];
		}
		memcpy(Le_w_sort, Le_Qf, sizeof(int)*decNum);
	}

	//compute next Le address
	*Le_A = (*Le_A - 1 + blockSize) % blockSize;
	for (blockIndex=0; blockIndex<decNum; blockIndex++)
	{
		AddrCal1Step(&Le_Rf[blockIndex], &Le_Qf[blockIndex], &Le_Rg[blockIndex], &Le_Qg[blockIndex], Rf2, Qf2, 0,
			blockSize, decNum);
	}*/

	//change for various code length
	int blockIndex = 0;
	int i = 0;

	//init
	if (CmpCounter % B == 0)
	{
		*Le_A = Le_A_init_2;
		for (i=0; i<decNum; i++)
		{
			Le_Rf[i] = Le_Rf_init_2[i];
			Le_Qf[i] = Le_Qf_init_2[i];
			Le_Rg[i] = Le_Rg_init_2[i];
			Le_Qg[i] = Le_Qg_init_2[i];
		}
	}
	if (!decMode)
	{
		*Le_w_ad = *Le_A;
		memcpy(Le_w_sort, resortP, sizeof(int)*decNum);
	}
	else
	{
		*Le_w_ad = Le_Rf[1];
		if (1 == decNum)
		{
			*Le_w_ad = Le_Rf[0];
		}
		memcpy(Le_w_sort, Le_Qf, sizeof(int)*decNum);
	}

	//compute next Le address
	*Le_A = (*Le_A - 1 + blockSize) % blockSize;
	for (blockIndex=0; blockIndex<decNum; blockIndex++)
	{
		AddrCal1Step(&Le_Rf[blockIndex], &Le_Qf[blockIndex], &Le_Rg[blockIndex], &Le_Qg[blockIndex], Rf2, Qf2, 0,
			blockSize, decNum);
	}

}

/*
********************************************************************************
* NAME:				CalcBufAddr_BUFA
* PURPOSE:			�����alpha��BUFA�Ķ�/д�ķ����ƫ�Ƶ�ַ
*
* Input:
initDir:			��ʼ���Ķ�д����
CmpCounter:			ÿ�������������Ѿ�������ô��bits
B:					������С
q_up:				��������
r:					�����Էֳ���������������r=0������r = blockSize - (q_up-1)*B;
*
* Output:
Ad:					BUFA��ƫ�Ƶ�ַ
dir:				BUFA�Ķ�/д����
*
* AUTHOR: 			Duck
********************************************************************************
*/
void CalcBufAddr_BUFA(int initDir, int CmpCounter, int B, int *Ad, int *dir, int q_up, int r)
{

/*	if (0 == CmpCounter)
	{
		*dir = initDir;
	}
	else if (CmpCounter % B == 0)
	{
		*dir = 1 - (*dir);
	}

	if (CmpCounter % B == 0)
	{
		*Ad = (*dir) ? 0 : (B-1);
	}
	else
	{
		*Ad = (*Ad) + (2*(*dir)-1);
	}*/

	//change for various code length
	if (0 == CmpCounter)
	{
		*dir = initDir;
	}
	else if (CmpCounter % B == 0)
	{
		*dir = 1 - (*dir);
	}

	if (CmpCounter % B == 0)
	{
		if(CmpCounter == (q_up+1)*B && r != 0)
		{
			*Ad = (*dir) ? (B-r) : (r-1);
		}
		else
		{
			*Ad = (*dir) ? 0 : (B-1);
		}
	}
	else
	{
		*Ad = (*Ad) + (2*(*dir)-1);
	}

}

/*
********************************************************************************
* NAME:				CalcBufAddr_3BUF
* PURPOSE:			���������BUF�Ķ�/д�ķ����ƫ�Ƶ�ַ
*
* Input:
initDir:			��ʼ���Ķ�д�����������õ�BUF0~BUF3�Ķ�д��������Ϊ����������/��������������˳�ʼ��Ϊ1010.���ǵ���BUF0����ǡ�����
CmpCounter:			ÿ�������������Ѿ�������ô��bits
Flag:				buffer��д����ת�ı�־����3Ϊһ������
final_BUF_flag:		�ж��Ƿ�ΪBPU�����ȡ�����һ������
q_up:				��������
r:					�����Էֳ���������������r=0������r = blockSize - (q_up-1)*B;
*
* Output:
Ad:					BUF��ƫ�Ƶ�ַ
dir:				BUF�Ķ�/д����
Flag:				buffer��д����ת�ı�־����3Ϊһ������
*
* AUTHOR: 			Duck
********************************************************************************
*/
//void CalcBufAddr_3BUF(int initDir,int initFlag, int CmpCounter, int B, int *Ad, int *dir, int *Flag)
void CalcBufAddr_3BUF(int initDir,int initFlag, int CmpCounter, int B, int *Ad, int *dir, int *Flag, int final_BUF_flag, int q_up, int r)
{

	/*if (0 == CmpCounter)
	{
		*dir = initDir;
		*Flag = initFlag;
	}
	else if (CmpCounter % B == 0)
	{
		//*dir = 1 - (*dir);
		*Flag = (*Flag + 1)%3;
		if(*Flag == 0){
			*dir = 1 - (*dir);
		}
	}

	if (CmpCounter % B == 0)
	{
		*Ad = (*dir) ? 0 : (B-1);
	}
	else
	{
		*Ad = (*Ad) + (2*(*dir)-1);
	}*/

	//change for the various code length
	if (0 == CmpCounter)
	{
		*dir = initDir;
		*Flag = initFlag;
	}
	else if (CmpCounter % B == 0)
	{
		//*dir = 1 - (*dir);
		*Flag = (*Flag + 1)%3;
		if(*Flag == 0){
			*dir = 1 - (*dir);
		}
	}

	if (CmpCounter % B == 0)
	{
		if(r != 0 && final_BUF_flag == 1 && CmpCounter == (q_up+1)*B)
		{
			*Ad = (*dir) ? (B-r) : (r-1);
		}
		else
		{
			*Ad = (*dir) ? 0 : (B-1);
		}
	}
	else
	{
		*Ad = (*Ad) + (2*(*dir)-1);
	}

}

/*
********************************************************************************
* NAME:				SaveBUF_3BUF
* PURPOSE:			����������ǰ�˵�4��buffer�ж�ȡ�����ݷֱ����decNum������������3��buffer�У���3��buffer�Ǵ�12����ѡ�������
*
* Input:
BUF_Write_Pos:		ѡ��buffer�Ĳ���
D:					�����Ľ�������
decNum:				�����������ĸ���
sysBit:				��ȡ��ϵͳ����
parBit:				��ȡ��У�����
La:					��ȡ��������Ϣ
*
* Output:
BUF0~2_SYS:			����������ϵͳ���ص�4��buffer
BUF0~2_PAR:			����������У����ص�4��buffer
BUF0~2_LA:			����������������Ϣ��4��buffer
*
* AUTHOR: 			Duck
********************************************************************************
*/
void SaveBUF_3BUF(int BUF_Write_Pos, int B, int decNum, double *sysBit, double *parBit, double *La,
			 double BUF0_SYS[][128],  double BUF0_PAR[][128],  double BUF0_LA[][128],  
			 double BUF1_SYS[][128],  double BUF1_PAR[][128],  double BUF1_LA[][128], 
			 double BUF2_SYS[][128],  double BUF2_PAR[][128],  double BUF2_LA[][128],
			 int BUF0_dir, int BUF1_dir, int BUF2_dir)
{
	int blockIndex;

	if (BUF_Write_Pos < B)
	{
		for (blockIndex=0; blockIndex<decNum; blockIndex++)
		{
			if(BUF0_dir == 1){//˳���
				BUF0_SYS[blockIndex][BUF_Write_Pos] = sysBit[blockIndex];
				BUF0_PAR[blockIndex][BUF_Write_Pos] = parBit[blockIndex];
				BUF0_LA[blockIndex][BUF_Write_Pos] = La[blockIndex];
			}
			else{//�����
				BUF0_SYS[blockIndex][(B-1)-BUF_Write_Pos] = sysBit[blockIndex];
				BUF0_PAR[blockIndex][(B-1)-BUF_Write_Pos] = parBit[blockIndex];
				BUF0_LA[blockIndex][(B-1)-BUF_Write_Pos] = La[blockIndex];
			}
		}
	}
	else if (BUF_Write_Pos < 2*B)
	{
		for (blockIndex=0; blockIndex<decNum; blockIndex++)
		{
			if(BUF1_dir == 1){
				BUF1_SYS[blockIndex][BUF_Write_Pos-B] = sysBit[blockIndex];
				BUF1_PAR[blockIndex][BUF_Write_Pos-B] = parBit[blockIndex];
				BUF1_LA[blockIndex][BUF_Write_Pos-B] = La[blockIndex];
			}
			else{
				BUF1_SYS[blockIndex][(B-1)-(BUF_Write_Pos-B)] = sysBit[blockIndex];
				BUF1_PAR[blockIndex][(B-1)-(BUF_Write_Pos-B)] = parBit[blockIndex];
				BUF1_LA[blockIndex][(B-1)-(BUF_Write_Pos-B)] = La[blockIndex];
			}
		}
	}
	else
	{
		for (blockIndex=0; blockIndex<decNum; blockIndex++)
		{
			if(BUF2_dir == 1){
				BUF2_SYS[blockIndex][BUF_Write_Pos-2*B] = sysBit[blockIndex];
				BUF2_PAR[blockIndex][BUF_Write_Pos-2*B] = parBit[blockIndex];
				BUF2_LA[blockIndex][BUF_Write_Pos-2*B] = La[blockIndex];
			}
			else{
				BUF2_SYS[blockIndex][(B-1)-(BUF_Write_Pos-2*B)] = sysBit[blockIndex];
				BUF2_PAR[blockIndex][(B-1)-(BUF_Write_Pos-2*B)] = parBit[blockIndex];
				BUF2_LA[blockIndex][(B-1)-(BUF_Write_Pos-2*B)] = La[blockIndex];
			}
		}
	}
}


/*
********************************************************************************
* NAME:				SelectBUF_3BUF
* PURPOSE:			��3��buffer��ѡ���ȡ���ݵ�buffer�����Ҷ�ȡ����
*
* Input:
BUF0~2_sysBit:		��ϵͳ���ص�3��buffer
BUF0~2_parBit:		��У����ص�3��buffer
BUF0~2_La:			��������Ϣ��3��buffer
decNum:				�����������ĸ���
id:					ѡbuffer�Ĳ���
*
* Output:
sysBit:				��ȡ��ϵͳ����
parBit:				��ȡ��У�����
La:					��ȡ��������Ϣ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void SelectBUF_3BUF(int id, int decNum, double *sysBit, double *parBit, double *La,
			   double *BUF0_sysBit,  double *BUF0_parBit,  double *BUF0_La,  
			   double *BUF1_sysBit,  double *BUF1_parBit,  double *BUF1_La,  
			   double *BUF2_sysBit,  double *BUF2_parBit,  double *BUF2_La)
{
	int blockIndex;

	for (blockIndex=0; blockIndex<decNum; blockIndex++)
	{
		if (0 == id)
		{
			sysBit[blockIndex] = BUF0_sysBit[blockIndex];
			parBit[blockIndex] = BUF0_parBit[blockIndex];
			La[blockIndex] = BUF0_La[blockIndex];
		}
		else if (1 == id)
		{
			sysBit[blockIndex] = BUF1_sysBit[blockIndex];
			parBit[blockIndex] = BUF1_parBit[blockIndex];
			La[blockIndex] = BUF1_La[blockIndex];
		}
		else
		{
			sysBit[blockIndex] = BUF2_sysBit[blockIndex];
			parBit[blockIndex] = BUF2_parBit[blockIndex];
			La[blockIndex] = BUF2_La[blockIndex];
		}
	}
}

/*
********************************************************************************
* NAME:				BPU_Warm
* PURPOSE:			����������betaֵ��ģ��
*
* Input:
sysBit:				�����ϵͳ����
parBit:				�����У�����
La:					�����������Ϣ
normFactor:			����������Ϣ�ϵĳ���
initFlag:			betaֵ�ĳ�ʼ���ź�
initBeta:			beta��ʼֵ
betaValueIn:		�ݴ��betaֵ
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
*
* Output:
betaValueIn:		�ݴ��betaֵ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void BPU_Warm(double sysBit, double parBit, double La, int initFlag, double *initBeta, double *betaValueIn, int decMethod)		
{
	double sysAddLePar[2] = {0};
	double sysAddLe = 0;
	double temp = 0;
	double nextTimebetaValue[8] = {0};
	int index = 0;

	sysAddLe = sysBit + La;
	sysAddLePar[0] = sysAddLe + parBit;
	sysAddLePar[1] = sysAddLe - parBit;	

	if (1 == initFlag)                      
	{									
		for(index=0; index<8; index++)
		{							
			betaValueIn[index] = initBeta[index];
		}
	}

	/* butterfly 0 */
	Butterfly(betaValueIn[0], betaValueIn[1], sysAddLePar[0], &nextTimebetaValue[0], &nextTimebetaValue[4], decMethod);

	/* butterfly 1 */
	Butterfly(betaValueIn[2], betaValueIn[3], sysAddLePar[1], &nextTimebetaValue[1], &nextTimebetaValue[5], decMethod);

	/* butterfly 2 */
	Butterfly(betaValueIn[4], betaValueIn[5], -sysAddLePar[1], &nextTimebetaValue[2], &nextTimebetaValue[6], decMethod);

	/* butterfly 3 */
	Butterfly(betaValueIn[6], betaValueIn[7], -sysAddLePar[0], &nextTimebetaValue[3], &nextTimebetaValue[7], decMethod);

	/* save beta value */
	temp = betaValueIn[1];
	for(index=0;index<8;index++)                            
	{
		betaValueIn[index] = nextTimebetaValue[index] - temp;
	}

}

/*
********************************************************************************
* NAME:				FPU
* PURPOSE:			ǰ��������alpha��ģ��
*
* Input:
sysBit:				�����ϵͳ����
parBit:				�����У�����
La:					�����������Ϣ
normFactor:			����������Ϣ�ϵĳ���
initFlag:			alphaֵ�ĳ�ʼ���ź�
initAlpha:			alpha��ʼֵ
betaValueIn:		�����betaֵ
alphaValueIn:		�����alphaֵ
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
*
* Output:
alphaValueIn:		�ݴ��alphaֵ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void FPU(double sysBit, double parBit, double La, int initFlag, double *initAlpha, double *alphaValueIn, int decMethod)		
{
	double sysAddLePar[2] = {0};   
	double nextTimeAlphaValue[8] = {0};
	double sysAddLe = 0;
	double temp = 0;
	int index = 0;

	/*initialize the alpha*/
	if (1 == initFlag)
	{
		for(index=0;index<8;index++)
		{							
			alphaValueIn[index] = initAlpha[index];
		}
	}

	sysAddLe = sysBit + La;
	sysAddLePar[0] = sysAddLe + parBit;
	sysAddLePar[1] = sysAddLe - parBit;	

	/*compute the next time's alpha*/
	/* butterfly 0 */
	Butterfly(alphaValueIn[0], alphaValueIn[4], sysAddLePar[0], &nextTimeAlphaValue[0], &nextTimeAlphaValue[1], decMethod);

	/* butterfly 1 */
	Butterfly(alphaValueIn[1], alphaValueIn[5], sysAddLePar[1], &nextTimeAlphaValue[2], &nextTimeAlphaValue[3], decMethod);

	/* butterfly 2 */
	Butterfly(alphaValueIn[2], alphaValueIn[6], -sysAddLePar[1], &nextTimeAlphaValue[4], &nextTimeAlphaValue[5], decMethod);

	/* butterfly 3 */
	Butterfly(alphaValueIn[3], alphaValueIn[7], -sysAddLePar[0], &nextTimeAlphaValue[6], &nextTimeAlphaValue[7], decMethod);

	temp = alphaValueIn[1];
	for(index=0; index<8; index++)                           /* save alpha value */
	{
		alphaValueIn[index] = nextTimeAlphaValue[index] - temp;
	}

}
/*
********************************************************************************
* NAME:				BPU
* PURPOSE:			����������betaֵ��Le��ģ��
*
* Input:
sysBit:				�����ϵͳ����
parBit:				�����У�����
La:					�����������Ϣ
normFactor:			����������Ϣ�ϵĳ���
initFlag:			betaֵ�ĳ�ʼ���ź�
initBeta:			beta��ʼֵ
betaValueIn:		�ݴ��betaֵ
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
*
* Output:
betaValueIn:		�ݴ��betaֵ
Le_sys:				�͸�demodulation��ϵͳ���ص�������Ϣ
Le_par:				�͸�demodulation��У����ص�������Ϣ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void BPU(double sysBit, double parBit, double La, int initFlag, double *initBeta, double *betaValueIn, 
		 double *alphaValueIn, double normalFactor, double *Le, int *LLR,double *Le_sys,double *Le_par, int decMethod)		
{
	double sysAddLePar[2] = {0};
	double sysAddLe = 0;
	double temp = 0;
	double nextTimebetaValue[8] = {0};
	int index = 0;

	sysAddLe = sysBit + La;
	sysAddLePar[0] = sysAddLe + parBit;
	sysAddLePar[1] = sysAddLe - parBit;	

	if (1 == initFlag)                      
	{									
		for(index=0; index<8; index++)
		{							
			betaValueIn[index] = initBeta[index];
		}
	}

	/* butterfly 0 */
	Butterfly(betaValueIn[0], betaValueIn[1], sysAddLePar[0], &nextTimebetaValue[0], &nextTimebetaValue[4], decMethod);

	/* butterfly 1 */
	Butterfly(betaValueIn[2], betaValueIn[3], sysAddLePar[1], &nextTimebetaValue[1], &nextTimebetaValue[5], decMethod);

	/* butterfly 2 */
	Butterfly(betaValueIn[4], betaValueIn[5], -sysAddLePar[1], &nextTimebetaValue[2], &nextTimebetaValue[6], decMethod);

	/* butterfly 3 */
	Butterfly(betaValueIn[6], betaValueIn[7], -sysAddLePar[0], &nextTimebetaValue[3], &nextTimebetaValue[7], decMethod);

	/*compute the current Le*/
	*Le = Max8Function( parBit + alphaValueIn[0] + betaValueIn[0], 
		parBit + alphaValueIn[4] + betaValueIn[1],
		-parBit + alphaValueIn[1] + betaValueIn[2],
		-parBit + alphaValueIn[5] + betaValueIn[3],
		-parBit + alphaValueIn[6] + betaValueIn[4],
		-parBit + alphaValueIn[2] + betaValueIn[5],
		parBit + alphaValueIn[7] + betaValueIn[6], 
		parBit + alphaValueIn[3] + betaValueIn[7],decMethod)
		-Max8Function(-parBit + alphaValueIn[4] + betaValueIn[0],
		-parBit + alphaValueIn[0] + betaValueIn[1],
		parBit + alphaValueIn[5] + betaValueIn[2], 
		parBit + alphaValueIn[1] + betaValueIn[3],
		parBit + alphaValueIn[2] + betaValueIn[4], 
		parBit + alphaValueIn[6] + betaValueIn[5],
		-parBit + alphaValueIn[3] + betaValueIn[6],
		-parBit + alphaValueIn[7] + betaValueIn[7],decMethod);
	*Le_par = Max8Function( sysAddLe + alphaValueIn[0] + betaValueIn[0], 
		sysAddLe + alphaValueIn[4] + betaValueIn[1],
		-sysAddLe + alphaValueIn[5] + betaValueIn[2],
		-sysAddLe + alphaValueIn[1] + betaValueIn[3],
		-sysAddLe + alphaValueIn[2] + betaValueIn[4],
		-sysAddLe + alphaValueIn[6] + betaValueIn[5],
		sysAddLe + alphaValueIn[7] + betaValueIn[6], 
		sysAddLe + alphaValueIn[3] + betaValueIn[7], decMethod)
		-Max8Function(-sysAddLe + alphaValueIn[4] + betaValueIn[0],
		-sysAddLe + alphaValueIn[0] + betaValueIn[1],
		sysAddLe + alphaValueIn[1] + betaValueIn[2], 
		sysAddLe + alphaValueIn[5] + betaValueIn[3],
		sysAddLe + alphaValueIn[6] + betaValueIn[4], 
		sysAddLe + alphaValueIn[2] + betaValueIn[5],
		-sysAddLe + alphaValueIn[3] + betaValueIn[6],
		-sysAddLe + alphaValueIn[7] + betaValueIn[7], decMethod);

	*LLR = ((*Le + 2*sysAddLe) >= 0) ? 0 : 1;			//hard decision
	*Le = (*Le) * normalFactor;
	*Le_sys = *Le;
	*Le_par = (*Le_par) * normalFactor;

	/* save beta value */
	temp = betaValueIn[1];
	for(index=0;index<8;index++)                            
	{
		betaValueIn[index] = nextTimebetaValue[index] - temp;
	}

}

/*
********************************************************************************
* NAME:				AddrCal1Step
* PURPOSE:			���Ƽ����ֵַ

* Input:
Rf:					��ʱ�̵�Rf��ֵ
Qf:					��ʱ�̵�Qf[decNum]��ֵ
Rg:					��ʱ�̵�Rg��ֵ
Qg:					��ʱ�̵�Qg[decNum]��ֵ
CmpDirection:		���Ʒ���;0->����;1->����
decNum:				�����������ĸ���
blockSize:			ÿ��������������������������ĳ���
Rf2:				((2*f2)%L)%W
Qf2:				((2*f2)%L)/W

* Output:
Rf:					��һʱ�̵�Rf��ֵ
Qf:					��һʱ�̵�Qf[decNum]��ֵ
Rg:					��һʱ�̵�Rg��ֵ
Qg:					��һʱ�̵�Qg[decNum]��ֵ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void AddrCal1Step(int *Rf, int *Qf, int *Rg, int *Qg, int Rf2, int Qf2, int CmpDirection, int blockSize, int decNum)
{
	int cRf = *Rf;
	int cQf = *Qf;
	int cRg = *Rg;
	int cQg = *Qg;
	int nextRf = 0;
	int nextQf = 0;
	int nextRg = 0;
	int nextQg = 0;
	int KRf = 0;
	int KQf = 0;
	int KRg = 0;
	int KQg = 0;

	if (1 == CmpDirection)
	{
		KRf = ((cRf + cRg - blockSize) >= 0) ? 1 : 0;
		KRg =((cRg + Rf2 - blockSize) >= 0) ? 1 : 0;
		KQf = ((cQf + cQg + KRf - decNum) >= 0) ? 1 : 0;
		KQg = ((cQg + Qf2 + KRg - decNum) >= 0) ? 1 : 0;

		nextRf = cRf + cRg - KRf*blockSize;
		nextRg = cRg + Rf2 - KRg*blockSize;
		nextQf = cQf + cQg + KRf - KQf*decNum;
		nextQg = cQg + Qf2 + KRg - KQg*decNum;
	}
	else
	{
		KRf = ((cRf - cRg) < 0) ? 1 : 0;
		KRg =((cRg - Rf2) < 0) ? 1 : 0;
		KQf = ((cQf - cQg - KRf) < 0) ? 1 : 0;
		KQg = ((cQg - Qf2 - KRg) < 0) ? 1 : 0;

		nextRf = cRf - cRg + KRf*blockSize;
		nextRg = cRg - Rf2 + KRg*blockSize;
		nextQf = cQf - cQg - KRf + KQf*decNum;
		nextQg = cQg - Qf2 - KRg + KQg*decNum;
	}

	*Rf = nextRf;
	*Rg = nextRg;
	*Qf = nextQf;
	*Qg = nextQg;

}
/*
********************************************************************************
* NAME:				MaxFunction
* PURPOSE:			�����ֵ
*
* Input:
a:					����ֵ1
b:					����ֵ2
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
*
* Return Value:
ln(exp(a)+exp(b))
*
* AUTHOR: 			Duck
********************************************************************************
*/
double MaxFunction(double a, double b, int decMethod)
{
	double tempMax = 0;
	double tempMin = 0;
	double temp = 0;

	if (a > b)
	{
		tempMax = a;
		tempMin = b;
	}
	else
	{
		tempMax = b;
		tempMin = a;
	}

	if (0 == decMethod)//Max-Log-MAP
	{
		return tempMax;
	}
	else if (1 == decMethod)//Log-MAP
	{
		temp = tempMin-tempMax;
		temp = log(1+exp(temp));
		return (tempMax+temp);
	}
	else//Max-Log-MAP+
	{
		temp = tempMax - tempMin;
		// 		return (tempMax+Log2/pow(2.0,temp));
		if (temp > 3.1174)
		{
			return tempMax;
		}
		else if (temp > 1.6649)
		{
			return (tempMax+0.0886);
		}
		else if (temp > 0.8814)
		{
			return (tempMax+0.2599);
		}
		else if (temp > 0.3830)
		{
			return (tempMax+0.4332);
		}
		else
		{
			return (tempMax+0.6065);
		}
	}

}

/*
********************************************************************************
* NAME:				Max8Function
* PURPOSE:			��8���������ֵ
*
* Input:
data1 - data8:		����ֵ
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
*
* Return Value:
ln(exp(data1)+...+exp(data8))
*
* AUTHOR: 			Duck
********************************************************************************
*/
double Max8Function(double data1,double data2,double data3,double data4,
					double data5,double data6,double data7,double data8, int decMethod)
{
	double tempLevel1[4] = {0};     
	double tempLevel2[2] = {0};      

	tempLevel1[0] = MaxFunction(data1, data2, decMethod);
	tempLevel1[1] = MaxFunction(data3, data4, decMethod);
	tempLevel1[2] = MaxFunction(data5, data6, decMethod);
	tempLevel1[3] = MaxFunction(data7, data8, decMethod);

	tempLevel2[0] = MaxFunction(tempLevel1[0], tempLevel1[1], decMethod);
	tempLevel2[1] = MaxFunction(tempLevel1[2], tempLevel1[3], decMethod);

	return MaxFunction(tempLevel2[0], tempLevel2[1], decMethod);
}
/*
********************************************************************************
* NAME:				Butterfly
* PURPOSE:			����ͼ�еĵ�������
*
* Input:
cValue0��cValue1:	��ǰʱ�̵�����ֵ
gamma:				��֧����
decMethod:			�����㷨��0->Max-Log-MAP��1->Max-Log-MAP+��2->Log-MAP
*
* Output:
nValue0��nValue1:	��һʱ�̵�����ֵ
*
* AUTHOR: 			Duck
********************************************************************************
*/
void Butterfly(double cValue0, double cValue1, double gamma, double *nValue0, double *nValue1, int decMethod)
{
	double temp1 = 0;     
	double temp2 = 0;      
	double temp3 = 0;     
	double temp4 = 0;    

	temp1 = cValue0 + gamma;
	temp2 = cValue1 - gamma;
	temp3 = cValue0 - gamma;
	temp4 = cValue1 + gamma;

	*nValue0 = MaxFunction(temp1, temp2, decMethod);
	*nValue1 = MaxFunction(temp3, temp4, decMethod);
}
/*
********************************************************************************
* NAME:				BigMod
* PURPOSE:			��ģ����
*
* Input:	
d:					����ģ����
m:					��ģ�Ķ���
*
* Return Value:
remainder:			ģ
*
* AUTHOR: 			Duck
********************************************************************************
*/
int BigMod(double d, int m)
{
	int quotient = 0;
	int remainder = 0;

	while (d<0)
	{
		d += m;
	}
	quotient = (int)(d/m);
	remainder = (int)(d - 1.0*m*quotient);

	return remainder;
}

/*
********************************************************************************
* NAME:				Reorder
* PURPOSE:			��Ram��������ݽ��н�֯����������
*	
* Input:
data:				����֯������
decNum:				��������������
resortSE:			��֯��
writeEn:			�ж��ǽ�֯���ǽ⽻֯
*
* Output:
data:				���潻֯�������
*
* AUTHOR: 			Duck
********************************************************************************
*/
void Reorder(double *data, int decNum, int *resortSE, int writeEn)
{
	double temp[maxDecNum] = {0};
	int i = 0;

	if (!writeEn)
	{
		for (i=0; i<decNum; i++)
		{
			temp[i] = data[resortSE[i]];
		}
	}
	else
	{
		for (i=0; i<decNum; i++)
		{
			temp[resortSE[i]] = data[i];
		}
	}

	memcpy(data, temp, sizeof(double)*decNum);

}

/*
********************************************************************************
* NAME:				ReorderI
* PURPOSE:			��Ram��������ݽ��н�֯���������������������
*
* Input:
data:				����֯������
decNum:				��������������
resortSE:			��֯��
writeEn:			�ж��ǽ�֯���ǽ⽻֯
*
* Output:
data:				���潻֯�������
*
* AUTHOR: 			Duck
********************************************************************************
*/
void ReorderI(int *data, int decNum, int *resortSE, int writeEn)
{
	int temp[maxDecNum] = {0};
	int i = 0;

	if (!writeEn)
	{
		for (i=0; i<decNum; i++)
		{
			temp[i] = data[resortSE[i]];
		}
	}
	else
	{
		for (i=0; i<decNum; i++)
		{
			temp[resortSE[i]] = data[i];
		}
	}

	memcpy(data, temp, sizeof(int)*decNum);

}
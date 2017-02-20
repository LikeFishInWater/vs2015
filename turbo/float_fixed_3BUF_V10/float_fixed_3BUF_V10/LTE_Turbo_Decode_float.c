/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "LTE_Turbo_Decode_float.h"

/*
********************************************************************************
* NAME:		CrcCheck
* PURPOSE:	crcУ����򣬿�ƥ��4�в�ͬ����crc������ȷ���1���������0

* Input:
seqIn:		��У��CRC����������
seqInLen:	�������г���
crcLen:		���������е�CRCУ��λ�ĳ���
crc24Type:	crcLen=24ʱ������ѭ�����ɶ���ʽ����crc24TypeΪ0��1���ֱ��Ӧ��׼�е�gcrc24A��gcrc24B

* Return Value:
CrcResult:	CRCУ����������ȷΪ1��������Ϊ0
*
* AUTHOR:	Duck
********************************************************************************
*/
int CrcCheck(int *seqIn, int seqInLen, int crcLen, int crc24Type)
{
	int i = 0;
	int j = 0;
	int temp = 0;
	int sum = 0;
	int reg[24] = {0};
	int G[25] = {0};
	int sumZero = 0;		//used to find out whether input sequence is a all 0 sequence or not

	//-	gCRC24A(D) = [D24 + D23 + D18 + D17 + D14 + D11 + D10 + D7 + D6 + D5 + D4 + D3 + D + 1] and;
	//-	gCRC24B(D) = [D24 + D23 + D6 + D5 + D + 1] for a CRC length L = 24 and;
	//-	gCRC16(D) = [D16 + D12 + D5 + 1] for a CRC length L = 16.
	//-	gCRC8(D) = [D8 + D7 + D4 + D3 + D + 1] for a CRC length of L = 8.
	switch(crcLen)
	{
	case 8: G[0]=G[1]=G[3]=G[4]=G[7]=G[8]=1; break;
	case 16:G[0]=G[5]=G[12]=G[16]=1; break;
	case 24: 
		if (!crc24Type)
		{
			G[0]=G[1]=G[3]=G[4]=G[5]=G[6]=G[7]=G[10]=G[11]=G[14]=G[17]=G[18]=G[23]=G[24]=1;
		}
		else
		{
			G[0]=G[1]=G[5]=G[6]=G[23]=G[24]=1;
		}
		break;
	default: break;
	}


	if (crcLen != 0)
	{
		for(i = 0; i < seqInLen ; i++)
		{
			temp = reg[crcLen-1];
			for(j = crcLen-1; j >0; j--)
			{
				reg[j] = reg[j-1] ^ (G[j] && temp);
			}
			reg[0] = temp ^ seqIn[i];
			sumZero += seqIn[i];
		}

		sum = 0;
		for(i = 0; i < crcLen; i++)
		{
			sum += reg[i];
		}

		if(sum != 0)
		{
			return 0;
		}
		else
		{
			if (0 != sumZero)// can't be all zero sqence
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		return 1;
	}

}


/*
********************************************************************************
* NAME:		ComputePara
* PURPOSE:	�����������鳤�ȡ�����ƥ�䳤�Ⱥ�����һЩ��������������ָ�ͷָ������ƥ���е�һЩ��Ҫ����

* Input:
A:			�����TB�鳤��
G:			�ֿ鼰����ƥ�������ĳ���
NL:			��������ӳ�䵽1�����Ͳ㣬NL=1�����ӳ�䵽2����4����NL=2
Qm:			���ݵ��Ʒ�ʽѡ��Qm=1->BPSK��Qm=2->QPSK��Qm=4->16QAM��Qm=6->64QAM
NIR:		��Ӧ������������soft buffer�Ĵ�С
rvIndex:	����汾�ţ�0��1��2��3��4�֣�����ȷ��cycle buf�д������ʼ��
k:			�洢��֯���ȵ�����
f1:			�洢��֯����f1������
f2:			�洢��֯����f2������
*
* Return Value:
C:			TB��ָ��CB����Ŀ
*
* Output:
Para:		�������飬��һάΪCB�����ţ��ڶ�άΪ7������������Ϊ
Para[i][0]:	��i��CB��ͷ����ӵĿձ�����Ŀ����i=0���з���ֵ
Para[i][1]:	��i��CB��ĳ���
Para[i][2]:	��i��CB�����õĽ�֯����f1
Para[i][3]:	��i��CB�����õĽ�֯����f2
Para[i][4]:	��i��CB���ڱ��������ӿ齻֯ʱ�õ��Ľ�֯���������
Para[i][5]:	��i��CB��������ƥ��ʱ�õ���ѭ������������ʼ���λ��k0
Para[i][6]:	��i��CB��������ƥ��ʱ�õ���ѭ�����������������E
*
* AUTHOR:	Duck
********************************************************************************
*/
int ComputePara(int A, int G, int NIR, int NL, int Qm, int rvIndex, int *k, int *f1, int *f2, int Para[][7])
{
	int C = 0;
	int L = 24;
	int B = A+L;
	int B_hat = 0;
	int Z = 6144;
	int K_Pos = 0;
	int K_Neg = 0;
	int C_Pos = 0;
	int C_Neg = 0;
	int f1_Pos = 0;
	int f1_Neg = 0;
	int f2_Pos = 0;
	int f2_Neg = 0;
	int F = 0;
	int i = 0;
	int G_hat = 0;
	int gamma = 0;
	int Ncb = 0;

	/*	code block segmentation*/
	//C & B'
	if (B <= Z)
	{
		C = 1;
		B_hat = B;
	}
	else
	{
		C = (int)((B+Z-L-1)/(Z-L));
		B_hat = B + C*L;
	}
	//K+ & K-
	for (i=0; i<188; i++)
	{
		if (C*k[i] >= B_hat)
		{
			K_Pos = k[i];
			K_Neg = k[i-1];
			f1_Pos = f1[i];
			f2_Pos = f2[i];
			f1_Neg = f1[i-1];
			f2_Neg = f2[i-1];
			break;
		}
	} 
	//C+ & C-
	if (1 == C)
	{
		C_Neg = 0;
		C_Pos = 1;
		K_Neg = 0;
	}
	else
	{
		C_Neg = (int)((C*K_Pos-B_hat)/(K_Pos-K_Neg));
		C_Pos = C - C_Neg;
	}
	//F
	F = C_Neg*K_Neg + C_Pos*K_Pos - B_hat;

	/*	rate match*/
	G_hat = (int)(G/(NL*Qm));
	gamma = G_hat%C;
	for (i=0; i<C; i++)
	{
		if (0 == i)
		{
			Para[i][0] = F;
		}
		else
		{
			Para[i][0] = 0;
		}
		if (i < C_Neg)
		{
			Para[i][1] = K_Neg;
			Para[i][2] = f1_Neg;
			Para[i][3] = f2_Neg;
		}
		else
		{
			Para[i][1] = K_Pos;
			Para[i][2] = f1_Pos;
			Para[i][3] = f2_Pos;
		}
		Para[i][4] = (int)((Para[i][1]+4+31)/32); //R

		//		Ncb = ((int)(NIR/C) < (3*R*32)) ? (int)(NIR/C) : (3*R*32);		//for down link turbo coded transport channels
		Ncb = 3*Para[i][4]*32;
		Para[i][5] = Para[i][4] * (2*rvIndex*((int)(Ncb+8*Para[i][4]-1)/(8*Para[i][4]))+2);//k0

		if (i <= C - gamma - 1)
		{
			Para[i][6] = NL*Qm*(int)(G_hat/C);
		}
		else
		{
			Para[i][6] = NL*Qm*(int)((G_hat+C-1)/C);
		}
	}

	return C;
}

/*
********************************************************************************
* NAME:		LTE_Turbo_Decode_float
* PURPOSE:	turbo����������

* Input:
decodeIn:	��������������
decodeInLen:�������ݳ���
msgLen:		�����������������
NL:			��������ӳ�䵽1�����Ͳ㣬NL=1�����ӳ�䵽2����4����NL=2
Qm:			���ݵ��Ʒ�ʽѡ��Qm=1->BPSK��Qm=2->QPSK��Qm=4->16QAM��Qm=6->64QAM
NIR:		��Ӧ������������soft buffer�Ĵ�С
rvIndex:	����汾�ţ�0��1��2��3��4�֣�����ȷ��cycle buf�д������ʼ��
TC:			�涨Turbo�뱾���һЩ�����Ľṹ�壬�������롢����ƥ�䡢���ơ������

*
* Output:
decodeOut:	�������������
La:			���������������,Ϊ�⸽��Ϣ
*
* EXIT:		
crcResult:	crcУ������1��ʾ��ȷ��0��ʾ����
*
* AUTHOR:	Duck
********************************************************************************
*/
int LTE_Turbo_Decode_float(int *decodeOut,double *La, double *decodeIn, int msgLen, int decodeInLen,
				int NL, int Qm, int NIR, int rvIndex, TurboCode *TC)
{
	double *systemBit;			//system bit
	double *parityBit;			//parity bit, corresponding to Component encoder 1
	double *iParityBit;			//parity bit, corresponding to Component encoder 2
	double *Le_sys;
	double *Le_par0;
	double *Le_par1;
	double *Le_sys_temp;
	double *Le_par0_temp;
	double *Le_par1_temp;
	double *Le_par1_temp2;
	double *cDecodeIn;
	int *cDecodeOut;
	int *decodeOutAddCrc;
	double *La_temp;//save the exInfor of the severial subblocks

	int codeBlockIndex = 0;
	int saveBitAddr = 0;
	int getBitAddr = 0;
	int crcResult[16] = {0};			//suppose 16 code block at most
	int decodeOutSaveAddr = 0;
	int leOutSaveAddr=0;
	//double betaValue[6144*8] = {0};
	int i = 0;
	int C = 0;																				//number of sub-block
	int Para[16][7] = {0};																//parameters array, suppose 16 sub-block at most
	int crcLen = 24;																	//crc length
	int k[188]={40,48,56,64,72,80,88,96,104,112,120,128,
		136,144,152,160,168,176,184,192,200,208,216,224,
		232,240,248,256,264,272,280,288,296,304,312,320,
		328,336,344,352,360,368,376,384,392,400,408,416,
		424,432,440,448,456,464,472,480,488,496,504,512,
		528,544,560,576,592,608,624,640,656,672,688,704,
		720,736,752,768,784,800,816,832,848,864,880,896,
		912,928,944,960,976,992,1008,1024,1056,1088,1120,
		1152,1184,1216,1248,1280,1312,1344,1376,1408,1440,
		1472,1504,1536,1568,1600,1632,1664,1696,1728,1760,
		1792,1824,1856,1888,1920,1952,1984,2016,2048,2112,
		2176,2240,2304,2368,2432,2496,2560,2624,2688,2752,
		2816,2880,2944,3008,3072,3136,3200,3264,3328,3392,
		3456,3520,3584,3648,3712,3776,3840,3904,3968,4032,
		4096,4160,4224,4288,4352,4416,4480,4544,4608,4672,
		4736,4800,4864,4928,4992,5056,5120,5184,5248,5312,
		5376,5440,5504,5568,5632,5696,5760,5824,5888,5952,
		6016,6080,6144};
	int f1[188]={3,7,19,7,7,11,5,11,7,41,103,15,
		9,17,9,21,101,21,57,23,13,27,11,27,85,29,33,15,17,33,103,
		19,19,37,19,21,21,115,193,21,133,81,45,23,243,151,155,25,
		51,47,91,29,29,247,29,89,91,157,55,31,17,35,227,65,19,37,
		41,39,185,43,21,155,79,139,23,217,25,17,127,25,239,17,137,
		215,29,15,147,29,59,65,55,31,17,171,67,35,19,39,19,199,21,
		211,21,43,149,45,49,71,13,17,25,183,55,127,27,29,29,57,45,
		31,59,185,113,31,17,171,209,253,367,265,181,39,27,127,143,
		43,29,45,157,47,13,111,443,51,51,451,257,57,313,271,179,
		331,363,375,127,31,33,43,33,477,35,233,357,337,37,71,71,
		37,39,127,39,39,31,113,41,251,43,21,43,45,45,161,89,323,
		47,23,47,263};
	int f2[188]={10,12,42,16,18,20,22,24,26,84,90,32,
		34,108,38,120,84,44,46,48,50,52,36,56,58,60,62,32,198,68,
		210,36,74,76,78,120,82,84,86,44,90,46,94,48,98,40,102,52,
		106,72,110,168,114,58,118,180,122,62,84,64,66,68,420,96,
		74,76,234,80,82,252,86,44,120,92,94,48,98,80,102,52,106,
		48,110,112,114,58,118,60,122,124,84,64,66,204,140,72,74,
		76,78,240,82,252,86,88,60,92,846,48,28,80,102,104,954,96,
		110,112,114,116,354,120,610,124,420,64,66,136,420,216,444,
		456,468,80,164,504,172,88,300,92,188,96,28,240,204,104,212,
		192,220,336,228,232,236,120,244,248,168,64,130,264,134,408,
		138,280,142,480,146,444,120,152,462,234,158,80,96,902,166,
		336,170,86,174,176,178,120,182,184,186,94,190,480,};// k, f1, f2,  all k,f1,f2 value in QPP


	//test mode
	int B = TC->SW_Size;
	double normFactor = TC->normFactor_float;
	int iterMax = TC->maxIterNum;
	int decNum = TC->decNum;
	int decMethod = TC->decMethod;
	double sigma2 = TC->sigma2;

	int GG=0;
	int FF=0;

	systemBit = (double *)malloc((maxInfoLength+4 +1) * sizeof(double));
	if (systemBit == NULL)
	{
		printf("Can not malloc systemBit!\n");
		getch();
		exit(0);
	}
	parityBit = (double *)malloc((maxInfoLength+4 +1) * sizeof(double));
	if (parityBit == NULL)
	{
		printf("Can not malloc parityBit!\n");
		getch();
		exit(0);
	}
	iParityBit = (double *)malloc((maxInfoLength+4 +1) * sizeof(double));
	if (iParityBit == NULL)
	{
		printf("Can not malloc iParityBit!\n");
		getch();
		exit(0);
	}
	Le_sys = (double *)malloc((maxInfoLength+4 +1) * sizeof(double));
	if (Le_sys == NULL)
	{
		printf("Can not malloc Le_sys!\n");
		getch();
		exit(0);
	}
	Le_par0 = (double *)malloc((maxInfoLength+4 +1) * sizeof(double));
	if (Le_par0 == NULL)
	{
		printf("Can not malloc Le_par0!\n");
		getch();
		exit(0);
	}
	Le_par1 = (double *)malloc((maxInfoLength+4 +1) * sizeof(double));
	if (Le_par1 == NULL)
	{
		printf("Can not malloc Le_par1!\n");
		getch();
		exit(0);
	}
	Le_sys_temp = (double *)malloc((maxInfoLength +1) * sizeof(double));
	if (Le_sys_temp == NULL)
	{
		printf("Can not malloc Le_sys_temp!\n");
		getch();
		exit(0);
	}
	Le_par0_temp = (double *)malloc((maxInfoLength +1) * sizeof(double));
	if (Le_par0_temp == NULL)
	{
		printf("Can not malloc Le_par0_temp!\n");
		getch();
		exit(0);
	}
	Le_par1_temp = (double *)malloc((maxInfoLength +1) * sizeof(double));
	if (Le_par1_temp == NULL)
	{
		printf("Can not malloc Le_par1_temp!\n");
		getch();
		exit(0);
	}
	Le_par1_temp2 = (double *)malloc((maxInfoLength +1) * sizeof(double));
	if (Le_par1_temp2 == NULL)
	{
		printf("Can not malloc Le_par1_temp2!\n");
		getch();
		exit(0);
	}
	cDecodeIn = (double *)malloc((3*maxInfoLength+12 +1) * sizeof(double));
	if (cDecodeIn == NULL)
	{
		printf("Can not malloc cDecodeIn!\n");
		getch();
		exit(0);
	}
	cDecodeOut = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (cDecodeOut == NULL)
	{
		printf("Can not malloc cDecodeOut!\n");
		getch();
		exit(0);
	}
	decodeOutAddCrc = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (decodeOutAddCrc == NULL)
	{
		printf("Can not malloc decodeOutAddCrc!\n");
		getch();
		exit(0);
	}
	La_temp = (double *)malloc((3*maxInfoLength+12 +1) * sizeof(double));
	if (La_temp == NULL)
	{
		printf("Can not malloc La_temp!\n");
		getch();
		exit(0);
	}

	memset(systemBit,0,(maxInfoLength+4 +1)*sizeof(double));
	memset(parityBit,0,(maxInfoLength+4 +1)*sizeof(double));
	memset(iParityBit,0,(maxInfoLength+4 +1)*sizeof(double));
	memset(Le_sys,0,(maxInfoLength+4 +1)*sizeof(double));
	memset(Le_par0,0,(maxInfoLength+4 +1)*sizeof(double));
	memset(Le_par1,0,(maxInfoLength+4 +1)*sizeof(double));
	memset(Le_sys_temp,0,(maxInfoLength +1)*sizeof(double));
	memset(Le_par0_temp,0,(maxInfoLength +1)*sizeof(double));
	memset(Le_par1_temp,0,(maxInfoLength +1)*sizeof(double));
	memset(Le_par1_temp2,0,(maxInfoLength +1)*sizeof(double));
	memset(cDecodeIn,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(cDecodeOut,0,(maxInfoLength +1)*sizeof(int));
	memset(decodeOutAddCrc,0,(maxInfoLength +1)*sizeof(int));
	memset(La_temp,0,(3*maxInfoLength+12 +1)*sizeof(double));

	//compute parameter
	C = ComputePara(msgLen, decodeInLen, NIR, NL, Qm, rvIndex, k, f1, f2, Para);

	/***************************************************************************************************
	for each code block: split, TurboDeinterleave, decode, check crc, combine and check crc again 
	***************************************************************************************************/
	getBitAddr = 0;
	for (codeBlockIndex=0; codeBlockIndex<C; codeBlockIndex++)
	{
		saveBitAddr = 0;

		//split code block concatenation
		while (saveBitAddr < Para[codeBlockIndex][6])
		{
			cDecodeIn[saveBitAddr] = decodeIn[getBitAddr];
			if (2 == decMethod)
			{
				cDecodeIn[saveBitAddr] /= sigma2;//for log map
			}
			getBitAddr++;
			saveBitAddr++;
		}

		//TurboDeinterleave
		TurboDeinterleave(cDecodeIn, systemBit, parityBit, iParityBit, Para[codeBlockIndex][1]+4, 
			Para[codeBlockIndex][5], Para[codeBlockIndex][4], Para[codeBlockIndex][6], Para[codeBlockIndex][0], codeBlockIndex);


		//float turbo decode
		TurboDecode_SW_Subblock_3BUF(cDecodeOut, systemBit, parityBit, iParityBit, Para[codeBlockIndex][1], Para[codeBlockIndex][2], 
			Para[codeBlockIndex][3], decNum, iterMax, B, normFactor, decMethod,Le_sys_temp,Le_par0_temp,Le_par1_temp2);
		
		/*prepare the Le information for the demodulation*/
		FF = 0;
		GG = (Para[codeBlockIndex][2] + Para[codeBlockIndex][3])%( Para[codeBlockIndex][1]);

		/*for(i=0;i<Para[codeBlockIndex][1];i++){
			Le_par1_temp[i] = Le_par1_temp2[FF];
			FF=(FF+GG)%(Para[codeBlockIndex][1]);
			GG=(GG+2*Para[codeBlockIndex][3])%(Para[codeBlockIndex][1]);
		}*/
		memcpy(Le_par1_temp,Le_par1_temp2,Para[codeBlockIndex][1]*sizeof(double));

		if(codeBlockIndex == C-1){//the final subblock ,add tail bits
			for(i=0;i<4;i++){
				Le_sys[Para[codeBlockIndex][1]+i] = 0;
				Le_par0[Para[codeBlockIndex][1]+i] = 0;
				Le_par1[Para[codeBlockIndex][1]+i] = 0;
			}
		}
		memcpy(Le_sys,Le_sys_temp,Para[codeBlockIndex][1]*sizeof(double));
		memcpy(Le_par0,Le_par0_temp,Para[codeBlockIndex][1]*sizeof(double));
		memcpy(Le_par1,Le_par1_temp,Para[codeBlockIndex][1]*sizeof(double));

		TurboInterleave_double(La_temp, Para[codeBlockIndex][6], Para[codeBlockIndex][1], codeBlockIndex
			, Para[codeBlockIndex][2], Para[codeBlockIndex][3], Para[codeBlockIndex][0], Le_sys,Le_par0,Le_par1, 
			Para[codeBlockIndex][4], Para[codeBlockIndex][5]);

		//construct original data
		for (i=0; i<Para[codeBlockIndex][1]-crcLen; i++)
		{
			if (0 != codeBlockIndex || i >= Para[codeBlockIndex][0])
			{
				decodeOutAddCrc[decodeOutSaveAddr] = cDecodeOut[i];
				decodeOutSaveAddr++;
			}
		}
		//construct exInfor of demodulation
		for (i=0; i<Para[codeBlockIndex][6]; i++)
		{
			La[leOutSaveAddr] = La_temp[i];
			leOutSaveAddr++;
		}

		if (1 == C)
		{
			crcResult[codeBlockIndex] = CrcCheck(cDecodeOut, Para[codeBlockIndex][1], 24, 0);
		}
		else
		{
			crcResult[codeBlockIndex] = CrcCheck(cDecodeOut, Para[codeBlockIndex][1], 24, 1);
		}
	}

	free(systemBit);
	systemBit = NULL;
	free(parityBit);
	parityBit = NULL;
	free(iParityBit);
	iParityBit = NULL;
	free(Le_sys);
	Le_sys = NULL;
	free(Le_par0);
	Le_par0 = NULL;
	free(Le_par1);
	Le_par1 = NULL;
	free(Le_sys_temp);
	Le_sys_temp = NULL;
	free(Le_par0_temp);
	Le_par0_temp = NULL;
	free(Le_par1_temp);
	Le_par1_temp = NULL;
	free(Le_par1_temp2);
	Le_par1_temp2 = NULL;
	free(cDecodeIn);
	cDecodeIn = NULL;
	free(cDecodeOut);
	cDecodeOut = NULL;
//	free(decodeOutAddCrc);
//	decodeOutAddCrc = NULL;
	free(La_temp);
	La_temp = NULL;

	//check crc
	memcpy(decodeOut, decodeOutAddCrc, sizeof(int)*msgLen);
	if (1 == C)
	{
		free(decodeOutAddCrc);
		decodeOutAddCrc = NULL;

		return crcResult[0];
	}
	else
	{
		crcResult[0] = CrcCheck(decodeOutAddCrc, msgLen+crcLen, crcLen, 0);

		free(decodeOutAddCrc);
		decodeOutAddCrc = NULL;

		return crcResult[0];
	}

}

/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "LTE_Turbo_Decode_fixed.h"
#include "LTE_Turbo_Decode_float.h"
#include "Turbo_Deinterleave_fixed.h"


/*
********************************************************************************
* NAME:		LTE_Turbo_Decode_fix
* PURPOSE:	turbo译码主函数

* Input:
decodeIn:	译码器输入数据
decodeInLen:输入数据长度
msgLen:		译码器解码输出长度
NL:			如果传输块映射到1个发送层，NL=1；如果映射到2个或4个，NL=2
Qm:			根据调制方式选择：Qm=1->BPSK；Qm=2->QPSK；Qm=4->16QAM；Qm=6->64QAM
NIR:		对应于整个传输块的soft buffer的大小
rvIndex:	冗余版本号，0，1，2，3共4种，用于确定cycle buf中传输的起始点
TC:			规定Turbo码本身的一些参数的结构体，包括编码、速率匹配、调制、译码等
lr:			规定输入译码器的LLR值定点化参数的结构体
others:		规定译码器内部alpha、beta、Le_sys、Le_par定点化字长的结构体

*
* Output:
decodeOut:	译码器解码输出
La:			译码器输出的软量,为外附信息
*
* EXIT:		
crcResult:	crc校验结果，1表示正确，0表示错误
*
* AUTHOR:	Duck
********************************************************************************
*/
int LTE_Turbo_Decode_fixed(int *decodeOut,int *La, int *decodeIn, int msgLen, int decodeInLen,
				int NL, int Qm, int NIR, int rvIndex, TurboCode *TC, LR *lr, OTHERS *others)
{
	int *systemBit;			//system bit
	int *parityBit;			//parity bit, corresponding to Component encoder 1
	int *iParityBit;			//parity bit, corresponding to Component encoder 2
	int *Le_sys;
	int *Le_par0;
	int *Le_par1;
	int *Le_sys_temp;
	int *Le_par0_temp;
	int *Le_par1_temp;
	int *Le_par1_temp2;
	int *cDecodeIn;
	int *cDecodeOut;
	int *decodeOutAddCrc;
	int *La_temp;//save the exInfor of the severial subblocks

	int codeBlockIndex = 0;
	int saveBitAddr = 0;
	int getBitAddr = 0;
	int crcResult[16] = {0};			//suppose 16 code block at most
	int decodeOutSaveAddr = 0;
	int leOutSaveAddr=0;
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
	int normFactor = TC->normFactor_fixed;
	//int normFactor = 2306;

	int iterMax = TC->maxIterNum;
	int decNum = TC->decNum;
	int decMethod = TC->decMethod;
	double sigma2 = TC->sigma2;

	int GG=0;
	int FF=0;

	systemBit = (int *)malloc((maxInfoLength+4 +1) * sizeof(int));
	if (systemBit == NULL)
	{
		printf("Can not malloc systemBit!\n");
		getch();
		exit(0);
	}
	parityBit = (int *)malloc((maxInfoLength+4 +1) * sizeof(int));
	if (parityBit == NULL)
	{
		printf("Can not malloc parityBit!\n");
		getch();
		exit(0);
	}
	iParityBit = (int *)malloc((maxInfoLength+4 +1) * sizeof(int));
	if (iParityBit == NULL)
	{
		printf("Can not malloc iParityBit!\n");
		getch();
		exit(0);
	}
	Le_sys = (int *)malloc((maxInfoLength+4 +1) * sizeof(int));
	if (Le_sys == NULL)
	{
		printf("Can not malloc Le_sys!\n");
		getch();
		exit(0);
	}
	Le_par0 = (int *)malloc((maxInfoLength+4 +1) * sizeof(int));
	if (Le_par0 == NULL)
	{
		printf("Can not malloc Le_par0!\n");
		getch();
		exit(0);
	}
	Le_par1 = (int *)malloc((maxInfoLength+4 +1) * sizeof(int));
	if (Le_par1 == NULL)
	{
		printf("Can not malloc Le_par1!\n");
		getch();
		exit(0);
	}
	Le_sys_temp = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (Le_sys_temp == NULL)
	{
		printf("Can not malloc Le_sys_temp!\n");
		getch();
		exit(0);
	}
	Le_par0_temp = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (Le_par0_temp == NULL)
	{
		printf("Can not malloc Le_par0_temp!\n");
		getch();
		exit(0);
	}
	Le_par1_temp = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (Le_par1_temp == NULL)
	{
		printf("Can not malloc Le_par1_temp!\n");
		getch();
		exit(0);
	}
	Le_par1_temp2 = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (Le_par1_temp2 == NULL)
	{
		printf("Can not malloc Le_par1_temp2!\n");
		getch();
		exit(0);
	}
	cDecodeIn = (int *)malloc((3*maxInfoLength+12 +1) * sizeof(int));
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
	La_temp = (int *)malloc((3*maxInfoLength+12 +1) * sizeof(int));
	if (La_temp == NULL)
	{
		printf("Can not malloc La_temp!\n");
		getch();
		exit(0);
	}

	memset(systemBit,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(parityBit,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(iParityBit,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(Le_sys,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(Le_par0,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(Le_par1,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(Le_sys_temp,0,(maxInfoLength +1)*sizeof(int));
	memset(Le_par0_temp,0,(maxInfoLength +1)*sizeof(int));
	memset(Le_par1_temp,0,(maxInfoLength +1)*sizeof(int));
	memset(Le_par1_temp2,0,(maxInfoLength +1)*sizeof(int));
	memset(cDecodeIn,0,(3*maxInfoLength+12 +1)*sizeof(int));
	memset(cDecodeOut,0,(maxInfoLength +1)*sizeof(int));
	memset(decodeOutAddCrc,0,(maxInfoLength +1)*sizeof(int));
	memset(La_temp,0,(3*maxInfoLength+12 +1)*sizeof(int));

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
		TurboDeinterleave_fix(cDecodeIn, systemBit, parityBit, iParityBit, Para[codeBlockIndex][1]+4, 
			Para[codeBlockIndex][5], Para[codeBlockIndex][4], Para[codeBlockIndex][6], Para[codeBlockIndex][0], codeBlockIndex);


		//turbo decode
			TurboDecode_SW_Subblock_3BUF_fixed(cDecodeOut, systemBit, parityBit, iParityBit, Para[codeBlockIndex][1], Para[codeBlockIndex][2], 
				Para[codeBlockIndex][3], decNum, iterMax, B, normFactor, decMethod,Le_sys_temp,Le_par0_temp,Le_par1_temp2,lr,others);
		
		/*prepare the Le information for the demodulation*/
		FF = 0;
		GG = (Para[codeBlockIndex][2] + Para[codeBlockIndex][3])%( Para[codeBlockIndex][1]);

		/*for(i=0;i<Para[codeBlockIndex][1];i++){
			Le_par1_temp[i] = Le_par1_temp2[FF];
			FF=(FF+GG)%(Para[codeBlockIndex][1]);
			GG=(GG+2*Para[codeBlockIndex][3])%(Para[codeBlockIndex][1]);
		}*/
		memcpy(Le_par1_temp,Le_par1_temp2,Para[codeBlockIndex][1]*sizeof(int));

		if(codeBlockIndex == C-1){//the final subblock ,add tail bits
			for(i=0;i<4;i++){
				Le_sys[Para[codeBlockIndex][1]+i] = 0;
				Le_par0[Para[codeBlockIndex][1]+i] = 0;
				Le_par1[Para[codeBlockIndex][1]+i] = 0;
			}
		}
		memcpy(Le_sys,Le_sys_temp,Para[codeBlockIndex][1]*sizeof(int));
		memcpy(Le_par0,Le_par0_temp,Para[codeBlockIndex][1]*sizeof(int));
		memcpy(Le_par1,Le_par1_temp,Para[codeBlockIndex][1]*sizeof(int));

		TurboInterleave(La_temp, Para[codeBlockIndex][6], Para[codeBlockIndex][1], codeBlockIndex
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

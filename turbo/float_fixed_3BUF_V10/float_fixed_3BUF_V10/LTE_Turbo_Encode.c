/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "LTE_Turbo_Encode.h"


/*
********************************************************************************
* NAME:		CRCEncode
* PURPOSE:	crc添加程序，匹配4种不同长度的crc
*
* Input:
seqIn:		输入的序列
seqInLen:	输入序列的长度
crcLen:		要添加的CRC校验位的长度
crc24Type:	crcLen=24时有两种循环生成多项式，用crc24Type为0、1来分别对应标准中的gcrc24A和gcrc24B
*
* Output:
seqOut:		加校验位后输出的序列
*
* AUTHOR: 	Duck
********************************************************************************
*/
void CRCEncode(int *seqOut, int *seqIn, int seqInLen, int crcLen, int crc24Type)
{
	int i,j;
	int temp;
	int reg[24] = {0};
	int G[25] = {0};

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
		for(i = 0; i < seqInLen; i++)
		{
			temp = reg[crcLen-1] ^ seqIn[i];
			for(j = crcLen-1 ;j > 0; j--)
			{
				reg[j] = reg[j -1] ^ (G[j] && temp);
			}
			reg[0] = temp;
		}
		if (seqIn != seqOut)
		{
			memcpy(seqOut, seqIn, seqInLen * sizeof(int));
		}
		for(i = 0; i < crcLen; i++)
		{
			seqOut[i+seqInLen] = reg[crcLen-1-i]; 
		}
	}
	else//if CRC length is 0, just copy input to output
	{
		if (seqIn != seqOut)
		{
			memcpy(seqOut, seqIn, seqInLen * sizeof(int));
		}
	}

}

/*
********************************************************************************
* NAME:			LTE_Turbo_Encode
* PURPOSE:		Turbo编码主函数
*
* Input:
transBlockIn:	待编码的传输块
transBlcokLen:	待编码的传输块长度
NL:				如果传输块映射到1个发送层，NL=1；如果映射到2个或4个，NL=2
Qm:				根据调制方式选择：Qm=1->BPSK；Qm=2->QPSK；Qm=4->16QAM；Qm=6->64QAM
NIR:			对应于整个传输块的soft buffer的大小
rvIndex:		冗余版本号，0，1，2，3共4种，用于确定cycle buf中传输的起始点
rateMatchLen:	turbo编码及速率匹配后的输出长度
*
* Output:
rateMatchOut:	turbo编码及速率匹配后的输出
*
* AUTHOR: 		Duck
********************************************************************************
*/
void LTE_Turbo_Encode(int *rateMatchOut, int *transBlockIn, int rateMatchLen, int transBlcokLen, int NL, int Qm, int NIR, int rvIndex)
{
	int *transBlockAddCrc;													//transport block after add crc
	int *cRateMatchOut;														//ram to save rate match out of one sub-block 
	int *systemBit;															//ram to save system bit of one sub-blockc
	int *parityBit;															//ram to save parity bit of one sub-block
	int *iParityBit;														//ram to save Turbo_Interleave parity bit of one sub-block
	int *cCodeBlock;														//ram to save current sub-block

	int rateMatchSaveAddr = 0;												//save address of rate match out data
	int codeBlockIndex = 0;													//index of sub-block
	int saveBitAddr = 0;													//save bit address
	int getBitAddr = 0;														//get bit address
	int C = 0;																//number of sub-block
	int Para[16][7] = {0};													//parameters array, suppose 16 sub-blocks at most
	int crcLen = 24;														//crc length
	int i = 0;
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

	transBlockAddCrc = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (transBlockAddCrc == NULL)
	{
		printf("Can not malloc transBlockAddCrc!\n");
		getch();
		exit(0);
	}
	cRateMatchOut = (int *)malloc((3*maxInfoLength+12 +1) * sizeof(int));
	if (cRateMatchOut == NULL)
	{
		printf("Can not malloc cRateMatchOut!\n");
		getch();
		exit(0);
	}
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
	cCodeBlock = (int *)malloc((maxInfoLength +1) * sizeof(int));
	if (cCodeBlock == NULL)
	{
		printf("Can not malloc cCodeBlock!\n");
		getch();
		exit(0);
	}

	memset(transBlockAddCrc,0,(maxInfoLength +1)*sizeof(int));
	memset(cRateMatchOut,0,(3*maxInfoLength+12 +1)*sizeof(int));
	memset(systemBit,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(parityBit,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(iParityBit,0,(maxInfoLength+4 +1)*sizeof(int));
	memset(cCodeBlock,0,(maxInfoLength +1)*sizeof(int));

	//compute parameter
	C = ComputePara(transBlcokLen, rateMatchLen, NIR, NL, Qm, rvIndex, k, f1, f2, Para);

	/*add crc to transport block*/
	CRCEncode(transBlockAddCrc, transBlockIn, transBlcokLen, crcLen, 0);

	/***************************************************************************************************
	for each code block, add crc , encode, rate match
	***************************************************************************************************/
	getBitAddr = 0;
	for (codeBlockIndex=0; codeBlockIndex<C; codeBlockIndex++)
	{
		saveBitAddr = 0;
		if (0 == codeBlockIndex)		//first sub-block, add F null bits
		{
			for (i=0; i<Para[codeBlockIndex][0]; i++)
			{
				cCodeBlock[saveBitAddr] = 0;
				saveBitAddr++;
			}
		}

		//add crc
		if (C == 1)
		{
			while (saveBitAddr < Para[codeBlockIndex][1])
			{
				cCodeBlock[saveBitAddr] = transBlockAddCrc[getBitAddr];
				getBitAddr++;
				saveBitAddr++;
			}
		}
		else
		{
			while (saveBitAddr < Para[codeBlockIndex][1]-crcLen)
			{
				cCodeBlock[saveBitAddr] = transBlockAddCrc[getBitAddr];
				getBitAddr++;
				saveBitAddr++;
			}
			CRCEncode(cCodeBlock, cCodeBlock, Para[codeBlockIndex][1]-crcLen, crcLen, 1);
		}

		//turbo encode for each sub-block
		TurboEncode(Para[codeBlockIndex][1], cCodeBlock, systemBit, parityBit, iParityBit,
			Para[codeBlockIndex][2], Para[codeBlockIndex][3]);

		//sub-block interleave and rate match
		TurboInterleave(cRateMatchOut, Para[codeBlockIndex][6], Para[codeBlockIndex][1], codeBlockIndex
			, Para[codeBlockIndex][2], Para[codeBlockIndex][3], Para[codeBlockIndex][0], systemBit, parityBit, iParityBit, 
			Para[codeBlockIndex][4], Para[codeBlockIndex][5]);

		//code block concatenation
		for (i=0; i<Para[codeBlockIndex][6]; i++)
		{
			rateMatchOut[rateMatchSaveAddr] = cRateMatchOut[i];
			rateMatchSaveAddr++;
		}
	}

	if (rateMatchSaveAddr != rateMatchLen)
	{
		printf("rate match error in TurboEncode. quit. \n");
		_getch();
		exit(0);
	}

	free(transBlockAddCrc);
	transBlockAddCrc = NULL;
	free(cRateMatchOut);
	cRateMatchOut = NULL;
	free(systemBit);
	systemBit = NULL;
	free(parityBit);
	parityBit = NULL;
	free(iParityBit);
	iParityBit = NULL;
	free(cCodeBlock);
	cCodeBlock = NULL;

}


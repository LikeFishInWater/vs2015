/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "Turbo_Deinterleave_fixed.h"

/*
********************************************************************************
* NAME:				TurboDeinterleave_fix
* PURPOSE:			解交织，用于解Turbo速率匹配中的子块交织,针对整型数
*
* Input:
decodeIn:			当前输入的译码块，其组成对应编码输出时的cycle buffer中一段长为rateMatchLen、起始为k0的输出，不含NULL比特
blockSizeAddTail:	解交织输出的3路比特流的长度
R:					交织矩阵的行数
k0:					速率匹配时从循环缓冲器中读数据的起始点
rateMacthLen:		速率匹配后输出的数据总长度
codeBlockIndex:		当前码块序号，若当前码块是传输块分割后的第一个码块，需要将另外单独填充的nullBitNum考虑在内
nullBitNum:			码块分割中在第一个码块中添加的空比特的个数
*
* Output:
systemBit:			解交织后输出的系统比特流，打孔位处已填充0
parityBit:			解交织后输出的校验比特流，对应分量编码器1输出的校验比特，打孔位处已填充0
iParityBit:			解交织后输出的校验比特流，对应分量编码器2输出的校验比特，打孔位处已填充0
*
* AUTHOR:			Duck
********************************************************************************
*/
void TurboDeinterleave_fix(int *decodeIn, int *systemBit, int *parityBit, int *iParityBit,
					   int blockSizeAddTail, int k0, int R, int rateMatchLen, int nullBitNum, int codeBlockIndex)
{
/*	int C = 32;				//row number of interleave matrix
	int interMatrixSize = 0;//interleave matrix size
	int F0 = 0;						//number of null bits added in sub-block interleave
	int getAddr = 0;
	int getBitLen = 0;
	int bitType = 0;
	int bitPosition = 0;
	int originBitPosition = 0;
	int totalNullBitNum = 0;
	int offset = 0;
	int sysLen = 0;
	int parLen = 0;
	int iParLen = 0;
	//inter-column permutation pattern
	const int P[32] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};

	//compute null bits number
	interMatrixSize = R * C;
	F0 = interMatrixSize - blockSizeAddTail;

	memset(systemBit, 0, sizeof(int)*6148);
	memset(parityBit, 0, sizeof(int)*6148);
	memset(iParityBit, 0, sizeof(int)*6148);
	getAddr = k0;
	getBitLen = 0;
	while (getBitLen < rateMatchLen)
	{
		//decide which bit stream it is , 0->system bit, 1->parity bit, 2-> interleave parity bit
		if (getAddr < interMatrixSize)
		{
			bitType = 0;
			bitPosition = getAddr;//系统bits
		}
		else
		{
			bitType = ((getAddr-interMatrixSize)%2 == 0) ? 1 : 2;
			bitPosition = (int)((getAddr-interMatrixSize)/2);//校验bits
		}

		offset = (2 == bitType) ? 1 : 0;
		originBitPosition = (P[(int)(bitPosition/R)] + C*(bitPosition%R) + offset)%interMatrixSize;
		if (0 == codeBlockIndex && 2 != bitType)
		{
			totalNullBitNum = F0 + nullBitNum;//for sub-block 0's system and parity bit stream, add additional nullBitNum NULL bits.
		}
		else
		{
			totalNullBitNum = F0;
		}

		if (originBitPosition >= F0)
		{
			switch(bitType)
			{
			case 0: 
				if (originBitPosition >= totalNullBitNum || 0 != codeBlockIndex)
				{
					systemBit[originBitPosition-F0] += decodeIn[getBitLen];
					getBitLen++;
					sysLen++;
				}
				else
				{
					systemBit[originBitPosition-F0] = 0;//null bit added to first code block
				}
				break;
			case 1: 
				if (originBitPosition >= totalNullBitNum || 0 != codeBlockIndex)
				{
					parityBit[originBitPosition-F0] += decodeIn[getBitLen];
					getBitLen++;
					parLen++;
				}
				else
				{
					parityBit[originBitPosition-F0] = 0;//null bit added to first code block
				}
				break;
			case 2: 
				iParityBit[originBitPosition-F0] += decodeIn[getBitLen];
				getBitLen++;
				iParLen++;
				break;
			default: 
				printf("error bit type. \n");
				_getch();
				break;
			}
		}
		getAddr = (getAddr+1)%(3*interMatrixSize);
	}*/
	int i;
	int lenCB;
	lenCB = (int)(rateMatchLen/3);
	for(i=0;i<lenCB;i++)
	{
		systemBit[i] = decodeIn[i];
		parityBit[i] = decodeIn[i+lenCB];
		iParityBit[i] = decodeIn[i+2*lenCB];
	}

}



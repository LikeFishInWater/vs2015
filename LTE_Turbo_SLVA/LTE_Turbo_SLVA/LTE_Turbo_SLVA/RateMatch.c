#include "define.h"
#include "RateMatch.h"

/*
********************************************************************************
* NAME:				SubInterleave
* PURPOSE:			子块交织函数
*
* Input:
inputBit:			输入的待交织的序列
R:					交织矩阵的行数
C:					交织矩阵的列数，LTE标准固定为32
F0:					交织矩阵中填充的空比特的个数
offset:				交织后输出的偏移量，取值为0或1
P:					交织矩阵的列间置换模式，LTE标准中规定为P[32] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};
*
* Output:
outputBit:			交织后输出的序列
*
* AUTHOR: 			Duck
********************************************************************************
*/
void SubInterleave(int *inputBit, int *outputBit, int offset, int R, int C, int F0, const int *P)
{
	int getAddr = 0;			//get data address
	int saveAddr = 0;		//save data address
	int rowIndex = 0;		//row index
	int columnIndex = 0;		//column index

	saveAddr=0; 
	while(saveAddr < R*C)
	{
		//get address begin from column p[0]
		getAddr = (C*rowIndex + P[columnIndex] + offset)%(R*C);
		//decide if it is a null bit
		if (getAddr >= F0)
		{
			outputBit[saveAddr] = inputBit[getAddr-F0];
		}
		else
		{
			outputBit[saveAddr] = 0;//fill null bit
		}
		saveAddr++;

		if (rowIndex < R-1)
		{
			rowIndex++;
		}
		else
		{
			rowIndex = 0;
			columnIndex++;
		}
	}

}


/*
********************************************************************************
* NAME:				TurboInterleave
* PURPOSE:			对一个码块进行Turbo速率匹配
*
* Input:
codeBlockSize:		码块的长度
f1:					QPP交织器的参数
f2:					QPP交织器的参数
nullBitNum:			码块分割中在第一个码块中添加的空比特的个数
systemBit:			分量编码器1输出的系统比特
parityBit:			分量编码器1输出的校验比特
iParityBit:			分量编码器2输出的校验比特
R:					交织矩阵的行数
k0:					速率匹配时从循环缓冲器中读数据的起始点
rateMacthLen:		速率匹配后输出的数据总长度
codeBlockIndex:		当前码块序号，若当前码块是传输块分割后的第一个码块，需要将另外单独填充的nullBitNum考虑在内
*
* Output:
rateMatchOut:		速率匹配后输出的数据
*
* AUTHOR: 			Yijia Xu
********************************************************************************
*/
void TurboInterleave(int *rateMatchOut, int rateMacthLen, int codeBlockSize, int codeBlockIndex, int f1, int f2, 
					int nullBitNum, int *systemBit, int *parityBit, int *iParityBit, int R, int k0)
{
	int cycleBuf[3*6176] = {0};								//cycle buffer
	int cycleBufSize = 0;															//cycle buffer size
	int codeBlockSizeAddTail = codeBlockSize + 4;			//
	int C = 32;																				//column number of Turbo_Interleave matrix
	int interMatrixSize = 0;														//Turbo_Interleave matrix size
	int F0;																						//number of null bits added
	int rateMatchSaveAddr = 0;												//rate match out save address
	int cycleBufGetAddr = 0;													//address of getting data from cycle buffer
	int bitType = 0;																		//bit stream type,0->sys,1->par,2->ipar
	int bitPosition = 0;
	int originBitPosition = 0;
	int totalNullBitNum = 0;
	int offset = 0;
	const int P[32] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};
	int i = 0;
	int temp[6176] = {0};
	int cycleBufSaveAddr = 0;

	//compute cycle buffer size & null bits number
	interMatrixSize = R * C;
	F0 = interMatrixSize - codeBlockSizeAddTail;
	cycleBufSize = 3*interMatrixSize;

	//interleave system bit and save it to cycle buf
	memset(temp, 0, sizeof(int)*R*C);
	SubInterleave(systemBit, temp, 0, R, C, F0, P);//offset is 0
	for (i=0; i<R*C; i++)
	{
		cycleBuf[cycleBufSaveAddr] = temp[i];
		cycleBufSaveAddr++;
	}

	//interleave parity bit and save it to cycle buf
	memset(temp, 0, sizeof(int)*R*C);
	SubInterleave(parityBit, temp, 0, R, C, F0, P);//offset is 0
	for (i=0; i<R*C; i++)
	{
		cycleBuf[cycleBufSaveAddr] = temp[i];
		cycleBufSaveAddr += 2;
	}

	//interleave Turbo_Interleave parity bit and save it to cycle buf
	cycleBufSaveAddr = R*C + 1;
	memset(temp, 0, sizeof(int)*R*C);
	SubInterleave(iParityBit, temp, 1, R, C, F0, P);//offset is 1
	for (i=0; i<R*C; i++)
	{
		cycleBuf[cycleBufSaveAddr] = temp[i];
		cycleBufSaveAddr += 2;
	}

	//rate match
	cycleBufGetAddr = k0;//set initial get address
	while (rateMatchSaveAddr < rateMacthLen)
	{
		//decide which bit stream it is , 0->system bit, 1->parity bit, 2-> Turbo_Interleave parity bit
		if (cycleBufGetAddr < interMatrixSize)
		{
			bitType = 0;
			bitPosition = cycleBufGetAddr;
		}
		else
		{
			bitType = ((cycleBufGetAddr-interMatrixSize)%2 == 0) ? 1 : 2;
			bitPosition = (int)((cycleBufGetAddr-interMatrixSize)/2);
		}
		//add another null bit
		if (0 == codeBlockIndex && 2 != bitType)
		{
			totalNullBitNum = F0 + nullBitNum;
		}
		else
		{
			totalNullBitNum = F0;
		}
		offset = (2 == bitType) ? 1 : 0;//compute offset
		originBitPosition = (P[(int)(bitPosition/R)] + C*(bitPosition%R) + offset)%interMatrixSize;
		if (originBitPosition >= totalNullBitNum)
		{
			rateMatchOut[rateMatchSaveAddr] = cycleBuf[cycleBufGetAddr];
			rateMatchSaveAddr++;
		}
		cycleBufGetAddr = (cycleBufGetAddr+1)%cycleBufSize;
	}

}
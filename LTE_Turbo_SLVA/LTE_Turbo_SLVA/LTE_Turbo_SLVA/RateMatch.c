#include "define.h"
#include "RateMatch.h"

/*
********************************************************************************
* NAME:				SubInterleave
* PURPOSE:			�ӿ齻֯����
*
* Input:
inputBit:			����Ĵ���֯������
R:					��֯���������
C:					��֯�����������LTE��׼�̶�Ϊ32
F0:					��֯���������Ŀձ��صĸ���
offset:				��֯�������ƫ������ȡֵΪ0��1
P:					��֯������м��û�ģʽ��LTE��׼�й涨ΪP[32] = {0,16,8,24,4,20,12,28,2,18,10,26,6,22,14,30,1,17,9,25,5,21,13,29,3,19,11,27,7,23,15,31};
*
* Output:
outputBit:			��֯�����������
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
* PURPOSE:			��һ��������Turbo����ƥ��
*
* Input:
codeBlockSize:		���ĳ���
f1:					QPP��֯���Ĳ���
f2:					QPP��֯���Ĳ���
nullBitNum:			���ָ����ڵ�һ���������ӵĿձ��صĸ���
systemBit:			����������1�����ϵͳ����
parityBit:			����������1�����У�����
iParityBit:			����������2�����У�����
R:					��֯���������
k0:					����ƥ��ʱ��ѭ���������ж����ݵ���ʼ��
rateMacthLen:		����ƥ�������������ܳ���
codeBlockIndex:		��ǰ�����ţ�����ǰ����Ǵ����ָ��ĵ�һ����飬��Ҫ�����ⵥ������nullBitNum��������
*
* Output:
rateMatchOut:		����ƥ������������
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
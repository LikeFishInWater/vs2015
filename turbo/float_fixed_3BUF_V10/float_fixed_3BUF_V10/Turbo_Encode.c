/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "Turbo_Encode.h"

/*
********************************************************************************
* NAME:			TurboEncode
* PURPOSE:		��һ��������Turbo���룬������LTE�е�QPP��֯��
*
* Input:
infoBit:		����Ĵ�������������
frameLen:		���ĳ���
f1:				QPP��֯���Ĳ���
f2:				QPP��֯���Ĳ���
*
* Output:
systemBit:		����������1�����ϵͳ����
parityBit:		����������1�����У�����
iParityBit:		����������2�����У�����
*
* AUTHOR: 		Duck
********************************************************************************
*/
void TurboEncode(int frameLen, int *infoBit, int *systemBit, int *parityBit, int *iParityBit, int f1, int f2)
{
	int i = 0;
	int tailBitAddr = 0;			//tail bit read address
	int tailBit[12] = {0};			//tail bits
	int regIn = 0;						//register input value
	int interleaveAddr = 0;	//Turbo_Interleave address
	int g = 0;							//initial value for Turbo_Interleave function
	static int dOne[3] = {0};	//register value of first component encoder
	static int dTwo[3] = {0};	//register value of second component encoder

	/******************encoder 1********************/
	for (i=0; i<frameLen; i++)
	{
		regIn = (infoBit[i] + dOne[1] + dOne[2])%2;
		systemBit[i] = infoBit[i];													//save system bit
		parityBit[i] = (infoBit[i] + dOne[0] + dOne[1])%2;		//save parity bit
		dOne[2] = dOne[1];
		dOne[1] = dOne[0];
		dOne[0] = regIn;
	}

	//tail bits
	for (i=0; i<3; i++)
	{
		tailBit[2*i] = (dOne[1] + dOne[2]) % 2;
		tailBit[2*i+1] = (dOne[0] + dOne[2]) % 2;
		dOne[2] = dOne[1];
		dOne[1] = dOne[0];
		dOne[0] = 0;
	}

	/******************encoder 2********************/
	g = f1 + f2;
	interleaveAddr = 0;
	for (i=0; i<frameLen; i++)
	{
		regIn = (infoBit[interleaveAddr] + dTwo[1] + dTwo[2])%2;
		iParityBit[i] = (infoBit[interleaveAddr] + dTwo[0] + dTwo[1])%2;	//save Turbo_Interleave parity bit
		dTwo[2] = dTwo[1];
		dTwo[1] = dTwo[0];
		dTwo[0] = regIn;
		//compute next Turbo_Interleave address
		interleaveAddr = (g + interleaveAddr)%frameLen;
		g = (g + 2*f2)%frameLen;
	}

	//tail bits
	for (i=0; i<3; i++)
	{
		tailBit[6+2*i] = (dTwo[1] + dTwo[2]) % 2;
		tailBit[6+2*i+1] = (dTwo[0] + dTwo[2]) % 2;
		dTwo[2] = dTwo[1];
		dTwo[1] = dTwo[0];
		dTwo[0] = 0;
	}

	/*attach tail bits to 3 encoded bit stream*/
	tailBitAddr = 0;
	for (i=0; i<4; i++)
	{
		systemBit[frameLen+i] = tailBit[tailBitAddr];
		tailBitAddr++;
		parityBit[frameLen+i] = tailBit[tailBitAddr];
		tailBitAddr++;
		iParityBit[frameLen+i] = tailBit[tailBitAddr];
		tailBitAddr++;
	}

}


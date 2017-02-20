#include "define.h"
#include "CRC_Encode.h"

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
* AUTHOR: 	Yijia Xu
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
	//	-	gCRC8(D) = [D8 + D7 + D4 + D3 + D + 1] for a CRC length of L = 8.
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
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
* PURPOSE:		对一个码块进行Turbo编码，适用于LTE中的QPP交织器
*
* Input:
infoBit:		输入的待编码的码块数据
infoBitLen:		码块的长度
f1:				QPP交织器的参数
f2:				QPP交织器的参数
*
* Output:
systemBit:		分量编码器1输出的系统比特
parityBit:		分量编码器1输出的校验比特
iParityBit:		分量编码器2输出的校验比特
*
* AUTHOR: 		Yijia Xu
********************************************************************************
*/
void TurboEncode(int infoBitLen, int *infoBit, int *systemBit, int *parityBit, int *iParityBit, int f1, int f2)
{
	int i = 0;
	int tailBitAddr = 0;			//tail bit read address
	int tailBit[12] = {0};                                                                                                                                                                                                                                                                                                                               			//tail bits
	int regIn = 0;						//register input value
	int interleaveAddr = 0;	//Turbo_Interleave address
	int g = 0;							//initial value for Turbo_Interleave function
	static int dOne[3] = {0};	//register value of first component encoder
	static int dTwo[3] = {0};	//register value of second component encoder

	/******************encoder 1********************/
	for (i=0; i<infoBitLen; i++)
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
	for (i=0; i<infoBitLen; i++)
	{
		regIn = (infoBit[interleaveAddr] + dTwo[1] + dTwo[2])%2;
		iParityBit[i] = (infoBit[interleaveAddr] + dTwo[0] + dTwo[1])%2;	//save Turbo_Interleave parity bit
		dTwo[2] = dTwo[1];
		dTwo[1] = dTwo[0];
		dTwo[0] = regIn;
		//compute next Turbo_Interleave address
		interleaveAddr = (g + interleaveAddr)%infoBitLen;
		g = (g + 2*f2)%infoBitLen;
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
		systemBit[infoBitLen+i] = tailBit[tailBitAddr];
		tailBitAddr++;
		parityBit[infoBitLen+i] = tailBit[tailBitAddr];
		tailBitAddr++;
		iParityBit[infoBitLen+i] = tailBit[tailBitAddr];
		tailBitAddr++;
	}

}

void set_Matrix (int *h , int *g , int n , int m , int k)
{
	int	   i, j;
	int	   h_sequence[7]={1,1,1,0,0,1,0};
	int	   pp, gg;


	for(i=0;i<k;i++)	//indexes of information column
	{
		h[i*n+i] = 1;
		for(j=i+1; j<k; j++)	//fill the i-th column
		{
			h[j*n+i] = h_sequence[((j-i-1)%7)];
		}
		h[i*n+k+i] = 1;	//set the parity bits for 1st cc
		
		index_2nd_constituent_encoder(k, &pp, &gg, i);

		h[(k+i)*n+pp] = 1;
		for(j=i+1; j<k; j++)	//fill the i-th column
		{
			h[(k+j)*n+pp] = h_sequence[((j-i-1)%7)];
		}
		h[(k+i)*n+2*k+i] = 1;	//set the parity bits for 2nd cc
	}

	for(i=0;i<k;i++)
	{
		g[i*n+i] = 1;
	}

	for(i=0;i<(m);i++)
	{
		for(j=0;j<k;j++)
		{
			g[j*n+k+i] = h[i*n+j];
		}
	}

}

void index_2nd_constituent_encoder(int length, int *PP, int *GG, int col_ID)
{
	int	index;
	int	f1, f2;

	if (length <= 512)
	{
		index = length/8 - 5;
	}
	else
	{
		if (length <= 1024)
		{
			index = length/16 + 27;
		}
		else
		{
			if (length <= 2048)
			{
				index = length/32 + 59;
			}
			else
			{
				index = length/64 + 91;
			}
		}
	}
	
	f1 = f1vec[index];
	f2 = f2vec[index];
	
	if(col_ID==0)
	{
		(*PP) = 0;
		(*GG) = f1 + f2;
	}
	else
	{
		// compute next interleave address
		// pi(x+1) = (pi(x) + g(x)) mod length
		(*PP) = ((*PP) + (*GG)) % length;		
		// g(x+1) = (g(x) + 2 * f2) mod length
		(*GG) = ((*GG) + 2 * f2) % length;
	}
}

void set_Matrix_CRC(int *g, int m, int n){
	int i,j;
	int g_sequence[25]={1,1,0,1,1,1,1,1,0,0,1,1,0,0,1,0,0,1,1,0,0,0,0,1,1};
	for(i=0;i<m;i++){
		for(j=i;j<n;j++){
			g[i*n+j]=g_sequence[(j-i)%25];
		}
	}

}
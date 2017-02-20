// LdpcScma.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "complex"
using namespace std;
const int M = 4;
const int N = 2;
const int K = 4;
const int J = 6;




int main()
{
	const int EsNoLen = 3;
	const int dataLen = 2000;
	const int LdpcLen = 4000;
	
	int LdpcData[J][LdpcLen];
	float EsNo[EsNoLen] = { 5.0,5.1,5.2 };
	for (int ii = 0; ii < EsNoLen; ii++)
	{
		int SNR = EsNo[ii];
		float sigma2 = 1 / 2 * 10 ^ (-SNR / 10);
		// data generation
		for (int i = 0; i < J; i++)
		{
			for (int jj = 0; jj < dataLen; jj++)
			{
				data[i][jj] = GenPN(&PN);
			}
		}
		//LDPC coding
		//modulation
		//SCMA mapping
		//transmitting
		//SCMA decoding
		//LDPC decoding
		

	}
	
	int a = 0;
	F[0][0]=a;
    return 0;
}


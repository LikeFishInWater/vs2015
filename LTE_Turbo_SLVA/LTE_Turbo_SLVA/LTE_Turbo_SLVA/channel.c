#include "channel.h"
#include "struct.h"
#include <string.h>
#include "define.h"

/*
********************************************************************************
* NAME:					awgn_channel_double
* PURPOSE:				�׸�˹�ŵ�

* Input:
RandomSeed *initNoise:	�������е������漴������ֵ�����������׸�˹����
channelIn:				�����ŵ�����Ϣ
length:					����������Ϣ�ĳ���
snr:					���������
rate:					����
*
*Output:
channelOut:������Ϣ�����ϰ׸�˹���������
*
* AUTHOR:				Duck
********************************************************************************
*/

void AWGNChannel(double snr, int length, double rate, double *channelIn, double *channelOut, RandomSeed *initNoise)
{
	long i;
	double noise;
	double sigma;

	sigma = (double) (1.0 / sqrt( 2.0 * rate * pow(10.0,0.1*snr) ));
	
	for (i=0; i<length; i++)
	{
		noise = random_g(sigma, initNoise);
		channelOut[i] = channelIn[i] + noise;
	//	channelOut[i] = channelIn[i];
	}

}

/*
********************************************************************************
* NAME:					random_g
* PURPOSE:				��������������

* Input:
initNoise:				�������е������漴������ֵ�����������׸�˹����
*
* AUTHOR:				Duck
********************************************************************************
*/

double random_g(double sigma, RandomSeed *initNoise)
{
	double u1,u2;
	double noise;

	u1 = random_u(initNoise);
	u2 = random_u(initNoise);
	noise = (double)sqrt((double)(-2)*log(1.0-u1))*(double)cos(2.0*PI*u2);
	noise = sigma * noise; 

	return noise;
}
/*
********************************************************************************
* NAME:					random_u
* PURPOSE:				�������������

* Input:
initNoise:				�������е������漴������ֵ
*
*Return:	
u:						�����
*
* AUTHOR:				Duck
********************************************************************************
*/

double random_u(RandomSeed *initNoise)
{
	double u = 0;

	initNoise->ix = (initNoise->ix*249) % 61967;
	initNoise->iy = (initNoise->iy*251) % 63443;
	initNoise->iz = (initNoise->iz*252) % 63599;

	u = (double)((initNoise->ix/61967.0+initNoise->iy/63443.0+initNoise->iz/63599.0)
		- (int)(initNoise->ix/61967.0+initNoise->iy/63443.0+initNoise->iz/63599.0));

	return u;
}



int GenPN(PNSeed *initPN)
{
	int result = 0;
	int i = 0;

	/*shift the shift register*/
	for(i=10; i>=1; i--)
	{
		initPN->reg[i] = initPN->reg[i-1];
	}

	/*calculate the output*/
	initPN->reg[0] = initPN->reg[10] ^ initPN->reg[3];
	result = initPN->reg[10];

	/*output the result*/
	return result;

}
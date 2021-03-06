/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "awgn_channel.h"

/*
********************************************************************************
* NAME:					awgn_channel_double
* PURPOSE:				白高斯信道

* Input:
RandomSeed *initNoise:	利用其中的种子随即产生数值，进而产生白高斯噪声
channelIn:				输入信道的信息
length:					输入和输出信息的长度
snr:					信噪比
rate:					码率
*
*Output:
channelOut:				输入信息叠加上白高斯噪声的输出
initNoise：				初始化时间种子
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
	}

}

/*
********************************************************************************
* NAME:					random_g
* PURPOSE:				白噪声产生函数

* Input:
sigma:					信道噪声方差N0的倒数
initNoise:				利用其中的种子随即产生数值，进而产生白高斯噪声
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
* PURPOSE:				随机数产生函数

* Input:
initNoise:				利用其中的种子随即产生数值
*
*Return:	
u:						随机的时间种子
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

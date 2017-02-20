//每次统计是对一个帧进行的。即每次统计出来的max等是一个帧的。统计完即可进行定点化，每个帧的定点化要求是一致的

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <direct.h>
#include "define.h"
#include "Struct.h"
#include "fixed_function.h"
//READ ME:通过试验可以证明，当将入口的LLR值统一变为原来的N倍时，只要将送给demodulation的先验信息同样除一个N，则可以保证性能保持不变
/*
********************************************************************************
* NAME:			Fixed_point
* PURPOSE:		为输入译码器的LLR值定点化。从程序的具体执行可知，只要将LLR定点化，由于采用的是max-Log-MAP算法，中间值和最后的Le_sys、Le_par
				均为整型，因此针对这些数可以省去统计再量化这一步骤，直接根据计算结果定出bit数。

* Input:
length:			需要定点化的数据流的长度
input:			输入的double类型的数据
lr:				用来存储输入数据流的max、mean、variance、power等信息，之后的由int->double的计算需要
method:			量化的方式选择：0->用最大值量化；1->用平均能量量化
*
* Output:
output:			定点化后的输出

* AUTHOR: 		Duck
********************************************************************************
*/
void Fixed_point(int length, double *input, int *output, LR *lr, int method){
	int		index_f=0;
	double	max0=0;
	double	max_temp=0;
	double	mean0=0;
	double	mean_temp=0;
	double	power0=0;
	double	power_temp=0;
	double	variance0=0;
	double	variance_temp=0;
	int		l=0;
	int		up_max_temp=0;//number of overflow data at the entrance of fixed decoder
	
	/*****record the data with maximum fabs and the mean and variance of input data******/
	for(index_f = 0;index_f < length;index_f++)
	{
		max_temp = fabs(input[index_f]);
		if(max_temp > max0)
		{
			max0 = max_temp;
		}
		mean0 += input[index_f];
		power0 += input[index_f]*input[index_f];
	}

	mean0 /= length;
	power0 /= length;
	lr->mean = mean0;		//mean of input data
	lr->power = power0;		//average power of input data
	lr->max = max0;			//maximum fabs of input data

	for(index_f = 0;index_f < length;index_f++)
	{
		variance0 += (input[index_f]-(lr->mean))*(input[index_f]-(lr->mean));
	}

	variance0 /= length;
	lr->variance = variance0;//variance of input data

	/*****record the data with maximum fabs and the mean and variance of input data******/

	/*********************Fixed for input data********************************/
	l = 1<<(lr->width-1);

	//fixed with maximum fabs of input data. Since there are little data's fabs near to lr->max, here use lr->temp_max(0~1) to smaller the input
	if(method == 0)
	{
		max0 = (lr->temp_max)*(lr->max);
		for(index_f = 0;index_f < length;index_f++)
		{
			if(input[index_f] >= (max0))		//topping
			{
				output[index_f] = l - 1;
				up_max_temp++;					//record the data overflow
			}
			else if(input[index_f] <= -max0)	//topping
			{
				output[index_f] = -l;
				up_max_temp++;					//record the data overflow
			}
			else{
				output[index_f] = (int)((input[index_f]/max0)*l);
			}
		}
	}

	//fixed with average power of input data. Since most data's power is larger than sqrt(lr->power), here use lr->temp_power(>1) to smaller the number of overflow data
	else if(method == 1)
	{
		power0 = (lr->temp_power) * sqrt(lr->power);
		for(index_f = 0;index_f < length;index_f++)
		{
			if(input[index_f] >= (power0))		//topping
			{
				output[index_f] = l - 1;
				up_max_temp++;					//record the data overflow
			}
			else if(input[index_f] <= (-power0))//topping
			{
				output[index_f] = -l;
				up_max_temp++;					//record the data overflow
			}
			else{
				output[index_f] = (int)((input[index_f]/power0) * l);
			}
		}
	}
	else{
		printf("Error at quantization method!");
		getch();
		exit(0);
	}
	/*********************Fixed for input data********************************/
	lr->up_max = up_max_temp;					//number of data that overflow
//	printf("%d\t",up_max_temp);
//	printf("%d\n",length);
}

/*
********************************************************************************
* NAME:			De_fixed_point
* PURPOSE:		将经过定点化计算得到的demodulation的先验信息进行int->double的计算

* Input:
length:			需要浮点化的数据流的长度
input:			输入的int类型的数据，需要浮点化
lr:				存储了输入数据流的max、mean、variance、power等信息
method:			浮点化的方式选择，与定点化方式对应：0->用最大值量化；1->用平均能量量化
*
* Output:
output:			浮点化后的输出

* AUTHOR: 		Duck
********************************************************************************
*/
void De_fixed_point(int length, int *input, double *output, LR *lr, int method){
	int index_df = 0;
	int l=0;
	double max0 = 0;
	double power0 = 0;

	l = 1 << (lr->width-1);
	max0 = (lr->temp_max) * (lr->max);
	power0 = (lr->temp_power) * sqrt(lr->power);

	//Defixed with maximum fabs of input data since fixed with maximum fabs of input data.
	if(method == 0)
	{
		for(index_df = 0;index_df < length;index_df++)
		{
			output[index_df] = ((input[index_df]*1.0)/l)*max0;
		}
	}

	//Defixed with average power of input data since fixed with average power of input data.
	else if(method == 1)
	{
		for(index_df = 0;index_df < length;index_df++)
		{
			output[index_df] = ((input[index_df]*1.0)/l)*power0;
		}
	}
	else{
		printf("Error at de_quantization method!");
		getch();
		exit(0);
	}
}

/*
********************************************************************************
* NAME:			topping
* PURPOSE:		按照程序中所给参数对内部计算所得值进行削顶操作
*
* Input:
data:			输入的要进行削顶的数
*
* Output:
width_data:		程序中设定的这一数的比特数
data:			削顶后的数
*
* AUTHOR: 		Duck
********************************************************************************
*/
int topping(int data, int width_data)
{
	int l = 1 << (width_data-1);
	if(data >= l)
	{
		data = l-1;
	}
	else if(data < -l)
	{
		data = -l;
	}
	else{
		data = data;
	}
	return data;
}
/*
********************************************************************************
* NAME:			MultiplyCoefficient
* PURPOSE:		在利用max-Log-MAP算法时，需要乘一个修正系数。本函数利用移位实现这一乘法
*
* Input:
opt:			将修正系数用整数表示的结果
x:				要乘opt的数
*
* Output:
x:				乘opt的结果
*
* AUTHOR: 		Duck
********************************************************************************
*/
int MultiplyCoefficient(int x, int opt)
{
	int result;
	int abs_x;
	int w0=0;
	int w1=0;
	int w2=0;
	int w3=0;
	w0 = opt/1000;
	w1 = opt/100 - w0*10;
	w2 = opt/10 - w0*100 - w1*10;
	w3 = opt - w0*1000 - w1*100 - w2*10;
	
	result = 0;

	abs_x = abs(x);
	if (w0 != 0)
	{
		result += (abs_x >> w0);
	}

	abs_x = abs(x);
	if (w1 != 0)
	{
		result += (abs_x >> w1);
	}

	abs_x = abs(x);
	if (w2 != 0)
	{
		result += (abs_x >> w2);
	}

	abs_x = abs(x);
	if (w3 != 0)
	{
		result -= (abs_x >> w3);
	}

	result = (x >= 0) ? result : (0-result);
	return result;

}
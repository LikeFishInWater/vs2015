//ÿ��ͳ���Ƕ�һ��֡���еġ���ÿ��ͳ�Ƴ�����max����һ��֡�ġ�ͳ���꼴�ɽ��ж��㻯��ÿ��֡�Ķ��㻯Ҫ����һ�µ�

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
//READ ME:ͨ���������֤����������ڵ�LLRֵͳһ��Ϊԭ����N��ʱ��ֻҪ���͸�demodulation��������Ϣͬ����һ��N������Ա�֤���ܱ��ֲ���
/*
********************************************************************************
* NAME:			Fixed_point
* PURPOSE:		Ϊ������������LLRֵ���㻯���ӳ���ľ���ִ�п�֪��ֻҪ��LLR���㻯�����ڲ��õ���max-Log-MAP�㷨���м�ֵ������Le_sys��Le_par
				��Ϊ���ͣ���������Щ������ʡȥͳ����������һ���裬ֱ�Ӹ��ݼ���������bit����

* Input:
length:			��Ҫ���㻯���������ĳ���
input:			�����double���͵�����
lr:				�����洢������������max��mean��variance��power����Ϣ��֮�����int->double�ļ�����Ҫ
method:			�����ķ�ʽѡ��0->�����ֵ������1->��ƽ����������
*
* Output:
output:			���㻯������

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
* PURPOSE:		���������㻯����õ���demodulation��������Ϣ����int->double�ļ���

* Input:
length:			��Ҫ���㻯���������ĳ���
input:			�����int���͵����ݣ���Ҫ���㻯
lr:				�洢��������������max��mean��variance��power����Ϣ
method:			���㻯�ķ�ʽѡ���붨�㻯��ʽ��Ӧ��0->�����ֵ������1->��ƽ����������
*
* Output:
output:			���㻯������

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
* PURPOSE:		���ճ����������������ڲ���������ֵ������������
*
* Input:
data:			�����Ҫ������������
*
* Output:
width_data:		�������趨����һ���ı�����
data:			���������
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
* PURPOSE:		������max-Log-MAP�㷨ʱ����Ҫ��һ������ϵ����������������λʵ����һ�˷�
*
* Input:
opt:			������ϵ����������ʾ�Ľ��
x:				Ҫ��opt����
*
* Output:
x:				��opt�Ľ��
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
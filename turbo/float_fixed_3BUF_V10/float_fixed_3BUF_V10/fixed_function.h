/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void Fixed_point(int length, double *input, int *output, LR *lr, int method);

void De_fixed_point(int length, int *input, double *output, LR *lr, int method);

int topping(int data, int width_data);

int MultiplyCoefficient(int x, int opt);
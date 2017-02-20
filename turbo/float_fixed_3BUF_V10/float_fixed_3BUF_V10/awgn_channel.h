/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"

void AWGNChannel(double snr, int length, double rate, double *channelIn, double *channelOut, RandomSeed *initNoise);

double random_g(double sigma, RandomSeed *initNoise);

double random_u(RandomSeed *initNoise);
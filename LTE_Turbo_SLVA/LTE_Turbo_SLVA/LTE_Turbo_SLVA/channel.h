#include <math.h>
#include "define.h"

//void GenPNSequence(int *seq, int *reg, int length);
/*
double random_u(struct AWGN *gauss);
void gauss_g(struct AWGN *gauss);
void awgn_channel_double (int input[] , double output[] , struct AWGN *gauss , int len);
*/
void AWGNChannel(double snr, int length, double rate, double *channelIn, double *channelOut, RandomSeed *initNoise);

double random_g(double sigma, RandomSeed *initNoise);

double random_u(RandomSeed *initNoise);

int GenPN(PNSeed *initPN);
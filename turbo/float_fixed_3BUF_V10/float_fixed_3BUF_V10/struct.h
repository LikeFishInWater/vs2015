/*
**********************************************************************************
* Copyright (c)	2009, NCRL@SEU.
* All rights reserved.
*
* ANSI C source	code
*
* MODULE	NAME ：struct.h
*
* ABSTRACT：struct definition
*	
* VERSION：	V7
* AUTHOR:	Duck		2011-09-05 
********************************************************************************
*/

#ifndef struct_h
#define struct_h

typedef struct
{
	long ix;
	long iy;
	long iz;
}RandomSeed;

typedef struct
{
	int reg[11];
}PNSeed;

typedef struct  
{
	int		crcLen;
	int		msgLen;
	int		infoLen;
	int		rateMatchLen;
	double	rate;
	int		decMethod;
	int		maxIterNum;
	int		maxbigIT;
	int		NL;
	int		Qm;
	int		NIR;
	int		rvIndex;
	int		decNum;
	int		decoder_Type;
	double	normFactor_float;
	int		normFactor_fixed;
	int		SW_Size;
	int		debugMode;
	double sigma2;

	//for debug
	int num_error[1000];
	FILE *fp;

}TurboCode;

typedef struct 
{
	double	startSnr;								/*start Eb/N0*/
	double	stepSnr;								/*step  Eb/N0*/
	double	stopSnr;								/*stop  Eb/N0*/
	int		leastErrorFrame;					/*least error frames needed*/
	int		leastTestFrame;					/*least test frames needed*/
	int		dispStep;								/*display step*/
	int		writeFreq;								/*write frequency*/
	int		noiseFul;								/*add noise,1 -> add,0 -> no*/
	int		decMode;								/*decode mode,0->new,1->continue*/
	int		recordOn;								/*record error frame,0->off,1->on*/
	int		testOn;									/*test error frame,0->off,1->on*/
	char		performance[100];
}SPStruct;

typedef struct 
{
	double					snr;										/*current snr*/
	int							simIndex;							/*index of simulation frames*/
	int							bitError;								/*total error bits*/
	double					BER;										/*bit error rate*/
	int							curError;								/*error bits in current frame*/
	int							frameError;						/*total error frames*/
	double					FER;										/*frame error rate*/
	int							initBitError;
	int							initFrameError;
	double					initStartSnr;
	int							initSimIndex;
	RandomSeed		initNoise;							/*random seed for awgn channel*/
	PNSeed					initPN;								/*initial PN Sequence*/
	RandomSeed		lastNoise;							/*random seed for awgn channel*/
	PNSeed					lastPN;								/*initial PN Sequence*/
}StatisStruct;

/*parameters for fixed decoder*/
typedef struct 
{
	int fixed_method;
	int width;
	double max;
	double mean;
	double variance;
	double power;
	double temp_power;//用来提高平均power的值，以减少饱和(>1)
	double temp_max;//用来降低最大绝对值，以提高饱和度(<1)
	int up_max;
}LR;

typedef struct //译码器内部alpha、beta、Le和给予demodulation的Le值的量化长度
{
	int width_alpha;
	int width_beta;
	int width_le;
	int width_le_demo;
}OTHERS;
/*parameters for fixed decoder*/

#endif

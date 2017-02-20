/*
**********************************************************************************
* Copyright (c)	2011, NCRL@SEU.
* All rights reserved.
*
* ANSI C source	code
*
* MODULE	NAME ：struct.h
*
* ABSTRACT：struct definition
*	
* VERSION：	V7
* AUTHOR:	Yijia Xu		2011-11-20 
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


struct state{
	int last_state1,last_state2;
	int trans_in1,trans_in2;
	int trans_out1[2],trans_out2[2];
	double gamma1,gamma2;
	double weight1,weight2;
	double alpha;
	double beta;
	int memory_last_state;
	int memory_trans_in;
	double addMetric;
};
struct SerialListVA1{
	
	int BestPathOut;//最优路径的信息序列(L X infoBitLen)
	
};

struct SerialListVA2{
	double BestPathAddMetric;//最优路径input_length时刻的累加度量
	double PathAddMetric;//待选路径input_length时刻的累加度量
	int MergeTime;//次优路径与最优路径的汇合时刻
	int CandiMergeTime;
};


/*
struct TurboCode 
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
	int		radixType;
	double normFactor;
	int		D;
	int		SW_Size;
	int		debugMode;
	int		normalizeOn;
	int       updateAllD;
	double sigma2;

	//for debug
	int num_error[1000];
	FILE *fp;

};
*/

struct SPStruct
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
};

struct StatisStruct
{
	double					    snr;										/*current snr*/
	double                     sigma2;
	double						total_ErrorBit_num ;								/*total error bits*/
	double					    BER;										/*bit error rate*/
	int							cur_ErrorBit_num;								/*error bits in current frame*/
	int							frameError;						/*total error frames*/
	double					    FER;										/*frame error rate*/
	int                            frame_num;

	RandomSeed		initNoise;							/*random seed for awgn channel*/
	PNSeed					initPN;								/*initial PN Sequence*/
	RandomSeed		lastNoise;							/*random seed for awgn channel*/
	PNSeed					lastPN;								/*initial PN Sequence*/
};


#endif

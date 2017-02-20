#pragma once
#include "LDPC.h"
#include "SCMA.h"
#include "AwgnChannel.h"
typedef struct
{
	int reg[11];
} PNSeed;
PNSeed PN = { { 1,0,1,0,0,0,1,1,0,0,1 } };
class CSimulate
{
public:
	float startSNR, stepSNR, stopSNR, RandomSeed, ModulationType;
	double MS_alpha;
	int maxIteration, DecodeAlgorithm;

	string CodeFileName;
	unsigned char * MsgSeq;
	unsigned long MsgLen;
	int LdpcLen;

	int J,K;

	int ** data;
	int ** ldpcData;
	double ** ScmaDataRe;
	double ** ScmaDataIm;
	double ** RecDataRe;
	double ** RecDataIm;


	
	CLDPC ldpc;
	CSCMA scma£»
	CChannel AwgnChannel;

	CSimulate();
	~CSimulate();
	void ReadProfile();
	void Initial();
	void Simulate();

};

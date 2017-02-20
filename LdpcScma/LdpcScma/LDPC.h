#pragma once

#include <string>
using namespace std;

class CNode
{
public:
	CNode();
	~CNode();
	unsigned int Degree;
	//	int maxDegree;
	unsigned long *Index;
	unsigned long *EdgeNo;
};

class CLDPC
{
public:
	CLDPC();
	~CLDPC();
	unsigned long CodeLen;
	unsigned long MsgLen;
	unsigned long ChkLen;
	double Rate;
	int maxIteration;
	CNode *checkNode;
	CNode *varaiableNode;
	unsigned int maxVarDegree;
	unsigned int maxChkDegree;
	double *Q;
	double *R;
	unsigned long EdgeSum;
	int DecodeAlgorithm;
	double MS_alpha;
	long *ExchangedinEncoding;
	CNode *encodingNode;
	unsigned long *BF_varSeq;
	unsigned long *BF_chkSeq;

	void Encode(int  * MsgSeq,int  * EncodingSeq);
	unsigned long CalErrorBits(int  * EncodingSeq, unsigned char *DecodingSeq);
	//	double *L;
	void ParamInit(int MaxIte, int DecAlg, double MS_a);
	void Initial(string CodeFileName);
	void ConstructGenMatrix();
	bool EncodeCheck(int  * EncodingSeq);
	void Decode(double * LLR,unsigned char *DecodingSeq);
	void Decode_BP(double * LLR, unsigned char *DecodingSeq);
	void Decode_MS(double * LLR, double alpha, unsigned char *DecodingSeq);
	void Decode_BF(double * LLR, unsigned char *DecodingSeq);
	
};
#pragma once
#include "Simulation.h"
#include "Complex.h"
#include "GF.h"
#include "Rand.h"
#include <string>
#include <iostream>
#include <fstream>
#include "nbldpc.h"
#include <cmath>
using namespace std;

class CComm
{
public:
	CComm(void);
	~CComm(void);
	int modOrder; int MOD_BIT_PER_SYM;
	int codeOrder; int CODE_BIT_PER_SYM;
	// msg
	int* TX_MSG_SYM; int* RX_MSG_SYM; int MSG_SYM_LEN;
	int* TX_MSG_BIT; int* RX_MSG_BIT; int MSG_BIT_LEN;
	// code
	int* TX_CODE_SYM; int* RX_DECODE_SYM; int CODE_SYM_LEN;
	int* TX_CODE_BIT; int* RX_DECODE_BIT; int CODE_BIT_LEN;
	// puncture
	int* PUN_SYM; int PUN_SYM_LEN;
	int* PUN_BIT; int PUN_BIT_LEN;
	// modulate
	CComplex* CONSTELLATION;
	CComplex* TX_MOD_SYM; CComplex* RX_MOD_SYM; int MOD_SYM_LEN;
	int* TX_MOD_BIT; int MOD_BIT_LEN;
	// demodulate decode llr
	double* RX_LLR_BIT;
	double** RX_LLR_SYM;
	double* BIT_P0;
	double* BIT_P1;
	double* SYM_P;

public:
	CNBLDPC NBLDPC;
	CRand Rand;
//	bool Initial(int GFq, int nQAM, string NBLDPCFileName, int PunctureVarDegree, int maxIter, int randMsg, int deocdemethod, int ems_nm, int ems_nc, double ems_factor, double ems_offset, int randseed);
	bool Initial(CSimulation &sim);
	int Transmission(void);
	int GenerateMessage(void);
	int Encode(void);
	int Puncture(void);
	int Modulate(void);
	int Channel_AWGN(void);
	int Demodulate(void);
	int Decode(void);
	int regPN[11];
	int GenPN(void);
	// noise
	double sigma_n;
	double CodeRate;
	double SetEbN0(CSimulation &sim);
	// compute the error frame, bit, symbol
//	int Err(double simcycle, double& errFrame, double& errBit, double& errSym, double& fer, double& ber, double& ser);
	int Err(CSimulation &sim);
	int randomMsg;
	
	double **RX_DEMOD_LLR_SYM;
	bool DecodeCorrect;
};


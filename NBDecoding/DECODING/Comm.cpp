#include "Comm.h"


CComm::CComm(void)
	: modOrder(0)
	, codeOrder(0)
	, TX_MSG_SYM(nullptr)
	, TX_MSG_BIT(nullptr)
	, TX_CODE_SYM(nullptr)
	, TX_CODE_BIT(nullptr)
	, TX_MOD_SYM(nullptr)
	, RX_MOD_SYM(nullptr)
	, RX_LLR_BIT(nullptr)
	, RX_LLR_SYM(nullptr)
	, RX_DECODE_SYM(nullptr)
	, RX_DECODE_BIT(nullptr)
	, MOD_BIT_PER_SYM(0)
	, CODE_BIT_PER_SYM(0)
	, MSG_SYM_LEN(0)
	, MSG_BIT_LEN(0)
	, CODE_SYM_LEN(0)
	, CODE_BIT_LEN(0)
	, MOD_SYM_LEN(0)
	, PUN_SYM(nullptr)
	, PUN_SYM_LEN(0)
	, RX_MSG_SYM(nullptr)
	, RX_MSG_BIT(nullptr)
	, PUN_BIT(nullptr)
	, PUN_BIT_LEN(0)
	, TX_MOD_BIT(nullptr)
	, MOD_BIT_LEN(0)
	, sigma_n(0)
	, CodeRate(0)
	, BIT_P0(nullptr)
	, BIT_P1(nullptr)
	, SYM_P(nullptr)
	, randomMsg(0)
	, CONSTELLATION(NULL)
	, RX_DEMOD_LLR_SYM(NULL)
	, DecodeCorrect(false)
{
}


CComm::~CComm(void)
{
}


//bool CComm::Initial(int GFq, int nQAM, string NBLDPCFileName, int PunctureVarDegree, int maxIter, int randMsg, int deocdemethod, int ems_nm, int ems_nc, double ems_factor, double ems_offset, int randseed)
bool CComm::Initial(CSimulation &sim)
{
	// transfer parameters
	modOrder = sim.nQAM;
	codeOrder = sim.GFq;
	MOD_BIT_PER_SYM = log(sim.nQAM * 1.0) / log(2.0);
	CODE_BIT_PER_SYM = log(sim.GFq * 1.0) / log(2.0);
	randomMsg = sim.randomMsg;
	
	// intial NBLDPC
	NBLDPC.Initial(sim);

	// msg block
	MSG_SYM_LEN = NBLDPC.CodeLen - NBLDPC.ChkLen;
	MSG_BIT_LEN = MSG_SYM_LEN * CODE_BIT_PER_SYM;
	// code block
	CODE_SYM_LEN = NBLDPC.CodeLen;
	CODE_BIT_LEN = CODE_SYM_LEN * CODE_BIT_PER_SYM;
	// puncture
	PUN_SYM_LEN = NBLDPC.PunctureLen;
	PUN_BIT_LEN = PUN_SYM_LEN * CODE_BIT_PER_SYM;
	// modulate block
	MOD_SYM_LEN = (CODE_SYM_LEN - PUN_SYM_LEN) * CODE_BIT_PER_SYM / MOD_BIT_PER_SYM;
	MOD_BIT_LEN = MOD_SYM_LEN * MOD_BIT_PER_SYM;
	// code rate
	CodeRate = double(MSG_SYM_LEN) / double(CODE_SYM_LEN - PUN_SYM_LEN);


	// allocate memory
	CONSTELLATION = new CComplex[modOrder];
	TX_MSG_SYM = new int[MSG_SYM_LEN]; TX_MSG_BIT = new int[MSG_BIT_LEN];
	RX_MSG_SYM = new int[MSG_SYM_LEN]; RX_MSG_BIT = new int[MSG_BIT_LEN];
	TX_CODE_SYM = new int[CODE_SYM_LEN]; TX_CODE_BIT = new int[CODE_BIT_LEN];
	RX_DECODE_SYM = new int[CODE_SYM_LEN]; RX_DECODE_BIT = new int[CODE_BIT_LEN];
	PUN_SYM = new int[PUN_SYM_LEN]; PUN_BIT = new int[PUN_BIT_LEN];
	TX_MOD_SYM = new CComplex[MOD_SYM_LEN]; TX_MOD_BIT = new int[MOD_BIT_LEN];
	RX_MOD_SYM = new CComplex[MOD_SYM_LEN];

	RX_LLR_SYM = new double*[CODE_SYM_LEN]; RX_LLR_BIT = new double[CODE_BIT_LEN];
	for(int s = 0; s < CODE_SYM_LEN;s ++)
	{
		RX_LLR_SYM[s] = new double[codeOrder - 1];
	}
	RX_DEMOD_LLR_SYM = new double *[MOD_SYM_LEN];
	for(int s = 0; s < MOD_SYM_LEN; s ++)
	{
		RX_DEMOD_LLR_SYM[s] = new double[modOrder - 1];
	}
	BIT_P0 = new double[CODE_BIT_PER_SYM];
	BIT_P1 = new double[CODE_BIT_PER_SYM];
	SYM_P = new double[codeOrder];
		
	// intial PN register
	regPN[0] = 1;	regPN[1] = 0;	regPN[2] = 1;	regPN[3] = 0;
	regPN[4] = 0;	regPN[5] = 0;	regPN[6] = 1;	regPN[7] = 1;
	regPN[8] = 0;	regPN[9] = 0;	regPN[10] = 1;

	// initial Puncture Position
	for(int p_s_len = 0; p_s_len < PUN_SYM_LEN; p_s_len ++)
	{
		PUN_SYM[p_s_len] = NBLDPC.PuncturePosition[p_s_len];
		for(int b_p_s = 0; b_p_s < CODE_BIT_PER_SYM; b_p_s ++)
		{
			PUN_BIT[p_s_len * CODE_BIT_PER_SYM + b_p_s] 
			= CODE_BIT_PER_SYM * NBLDPC.PuncturePosition[p_s_len] + b_p_s;
		}
	}

	// intial random generator
	Rand.IX = Rand.IY = Rand.IZ = sim.randomseed;

	// initial modulation and demodulation constellation
	ifstream fcons(sim.ConstellationFileName);
	string consrub;
	int index;
	for(int k = 0; k < modOrder; k ++)
	{
		fcons >> consrub >> index;
		fcons >> consrub >> CONSTELLATION[index].Real >> consrub >> CONSTELLATION[index].Image;
	}
	fcons.close();

	return false;
}


int CComm::Transmission(void)
{
	GenerateMessage();
	Encode();
	Puncture();
	Modulate();
	Channel_AWGN();
	Demodulate();
	Decode();
	return 0;
}


int CComm::GenerateMessage(void)
{
	// generate TX_MSG_BIT & TX_MSG_SYM
	for(int b = 0; b < MSG_BIT_LEN; b ++)
	{
		if(randomMsg)
		{
			TX_MSG_BIT[b] = GenPN();
		}
		else
		{
			TX_MSG_BIT[b] = 0;//GenPN();
		}
	}
	for(int s = 0; s < MSG_SYM_LEN; s ++)
	{
		TX_MSG_SYM[s] = 0;
		for(int b_p_s = 0; b_p_s < CODE_BIT_PER_SYM; b_p_s ++)
		{
			TX_MSG_SYM[s] += (TX_MSG_BIT[s * CODE_BIT_PER_SYM + b_p_s] << (CODE_BIT_PER_SYM - 1 - b_p_s));
		}
	}
	return 0;
}


int CComm::Encode(void)
{
	// TX_MSG_SYM --> TX_CODE_SYM
	if(randomMsg)
	{
		NBLDPC.Encode(TX_MSG_SYM, TX_CODE_SYM);
	}
	else
	{
		for(int s = 0; s < CODE_SYM_LEN; s ++)
		{
			TX_CODE_SYM[s] = 0;
		}
	}
	
	// TX_CODE_SYM --> TX_CODE_BIT
	for(int s = 0; s < CODE_SYM_LEN; s ++)
	{
		for(int b_p_s = 0; b_p_s < CODE_BIT_PER_SYM; b_p_s ++)
		{
			TX_CODE_BIT[s * CODE_BIT_PER_SYM + b_p_s] 
			= ((TX_CODE_SYM[s] & (1 << (CODE_BIT_PER_SYM - 1 - b_p_s))) == 0)? 0 : 1;
		}
	}
	/*
	// TX_MSG_SYM --> TX_MSG_BIT
	for(int s = 0; s < MSG_SYM_LEN;s ++)
	{
		for(int b_p_s = 0; b_p_s < CODE_BIT_PER_SYM; b_p_s ++)
		{
			TX_MSG_BIT[s * CODE_BIT_PER_SYM + b_p_s]
			= ((TX_MSG_SYM[s] & (1 << (CODE_BIT_PER_SYM - 1 - b_p_s))) == 0)? 0 : 1;
		}
	}
	*/
	// TX_CODE_BIT --> TX_MSG_BIT
	for(int b = 0; b < MSG_BIT_LEN; b ++)
	{
		TX_MSG_BIT[b] = TX_CODE_BIT[b];
	}
	return 0;
}


int CComm::Modulate(void)
{
	// TX_MOD_BIT --> TX_MOD_SYM
	int constellation_index;
	for(int s = 0; s < MOD_SYM_LEN; s ++)
	{
//		TX_MOD_SYM[s].Real = 1 - 2 * TX_MOD_BIT[s];
//		TX_MOD_SYM[s].Image = 0;
		constellation_index = 0;
		for(int b_p_s = 0; b_p_s < MOD_BIT_PER_SYM; b_p_s ++)
		{
			constellation_index += (TX_MOD_BIT[s * MOD_BIT_PER_SYM + b_p_s] << ( MOD_BIT_PER_SYM - 1 - b_p_s ));
		}
		TX_MOD_SYM[s].Real = CONSTELLATION[constellation_index].Real;
		TX_MOD_SYM[s].Image = CONSTELLATION[constellation_index].Image;
	}
	return 0;
}


int CComm::Channel_AWGN(void)
{
	// only support bpsk now
	for(int s = 0; s < MOD_SYM_LEN; s ++)
	{
		RX_MOD_SYM[s].Real = TX_MOD_SYM[s].Real + Rand.Rand_Norm(0, sigma_n);
		RX_MOD_SYM[s].Image = TX_MOD_SYM[s].Image + Rand.Rand_Norm(0, sigma_n);
	}
	return 0;
}


int CComm::Demodulate(void)
{
	if(modOrder == 2)
	{
		// RX_MOD_SYM --> RX_LLR_BIT --> RX_LLR_SYM
		// only support bpsk now
		int p_i = 0;
		for(int b = 0; b < CODE_BIT_LEN; b ++)
		{
			if(PUN_BIT[p_i] == b && PUN_BIT_LEN != 0)
			{
				p_i ++;
				RX_LLR_BIT[b] = 0;
			}
			else
			{
				RX_LLR_BIT[b] = -2 * RX_MOD_SYM[b - p_i].Real / (sigma_n * sigma_n);
			}
		}
		// RX_LLE_BIT --> RX_LLR_SYM
		for(int s = 0; s < CODE_SYM_LEN; s ++)
		{
			for(int q = 1; q < codeOrder; q ++)
			{
				RX_LLR_SYM[s][q - 1] = 0;
				for(int b_p_s = 0; b_p_s < CODE_BIT_PER_SYM; b_p_s ++)
				{
					if((q & (1 << (CODE_BIT_PER_SYM - 1 - b_p_s))) != 0)
					{
						RX_LLR_SYM[s][q - 1] += RX_LLR_BIT[s * CODE_BIT_PER_SYM + b_p_s];
					}
				}
			}
		}
	}
	/*
	else if(modOrder < codeOrder)
	{
		for(int s = 0; s < MOD_SYM_LEN; s ++)
		{
			for(int q = 1; q < modOrder; q ++)
			{
				RX_DEMOD_LLR_SYM[s][q - 1] = 
					( (2 * RX_MOD_SYM[s].Real - CONSTELLATION[0].Real - CONSTELLATION[q].Real ) 
					* (CONSTELLATION[q].Real - CONSTELLATION[0].Real)
					+ (2 * RX_MOD_SYM[s].Image - CONSTELLATION[0].Image - CONSTELLATION[q].Image ) 
					* (CONSTELLATION[q].Image - CONSTELLATION[0].Image) ) / (2 * sigma_n * sigma_n);
			}
		}
		int p_i = 0;
		for(int s = 0; s < CODE_SYM_LEN; s ++)
		{
			if(PUN_SYM[p_i] == s && PUN_SYM_LEN != 0)
			{
				p_i ++;
				for(int q = 1; q < codeOrder; q ++)
				{
					RX_LLR_SYM[s][q - 1] = 0;
				}
			}
			else
			{

			}
		}
	}
	*/
	else if(modOrder == codeOrder)
	{
		int p_i = 0;
		for(int s = 0; s < CODE_SYM_LEN; s ++)
		{
			if(PUN_SYM[p_i] == s && PUN_SYM_LEN != 0)
			{
				p_i ++;
				for(int q = 1; q < codeOrder; q ++)
				{
					RX_LLR_SYM[s][q - 1] = 0;
				}
				
			}
			else
			{
				for(int q = 1; q < codeOrder; q ++)
				{
					RX_LLR_SYM[s][q - 1] = ( (2 * RX_MOD_SYM[s - p_i].Real - CONSTELLATION[0].Real - CONSTELLATION[q].Real ) * (CONSTELLATION[q].Real - CONSTELLATION[0].Real) 
						+ (2 * RX_MOD_SYM[s - p_i].Image - CONSTELLATION[0].Image - CONSTELLATION[q].Image ) * (CONSTELLATION[q].Image - CONSTELLATION[0].Image) ) / (2 * sigma_n * sigma_n);
				}
			}
		}
	}
	else
	{
		cout << "This module ( code order ~= modulation order ) haven't been developed!" << endl;
		exit(-1);

	}
	return 0;
}


int CComm::Decode(void)
{
	// NBLDPC deocding
	DecodeCorrect = NBLDPC.Decoding(RX_LLR_SYM, RX_DECODE_SYM);
	// RX_DECODE_SYM --> RX_DECODE_BIT
	for(int s = 0; s < CODE_SYM_LEN; s ++)
	{
		for(int b_p_s = 0; b_p_s < CODE_BIT_PER_SYM; b_p_s ++)
		{
			RX_DECODE_BIT[s * CODE_BIT_PER_SYM + b_p_s]
			= ((RX_DECODE_SYM[s] & (1 << (CODE_BIT_PER_SYM - 1 - b_p_s))) == 0)? 0 : 1;
		}
	}
	// RX_DECODE_SYM --> RX_MSG_SYM
	for(int s = 0; s < MSG_SYM_LEN; s ++)
	{
		RX_MSG_SYM[s] = RX_DECODE_SYM[s];
	}
	// RX_DECODE_BIT --> RX_MSG_BIT
	for(int b = 0; b < MSG_BIT_LEN; b ++)
	{
		RX_MSG_BIT[b] = RX_DECODE_BIT[b];
	}
	return 0;
}


int CComm::GenPN(void)
{
	/*shift the shift register*/
	for (int i = 10; i >= 1; i--)
	{
		regPN[i] = regPN[i - 1];
	}
	/*calculate the output*/
	regPN[0] = regPN[10] ^ regPN[3];
	/*output the result*/
	return regPN[10];
}


int CComm::Puncture(void)
{
	// TX_CODE_BIT --> TX_MOD_BIT
	int p_i = 0;
	int m_b = 0;
	for(int b = 0; b < CODE_BIT_LEN; b ++)
	{
		if(PUN_BIT[p_i] == b && PUN_BIT_LEN != 0) // this bit is punctured
		{
			p_i ++;
		}
		else
		{
			TX_MOD_BIT[m_b] = TX_CODE_BIT[b];
			m_b ++;
		}
	}
	return 0;
}


double CComm::SetEbN0(CSimulation &sim)
{
	// reset the seed
	Rand.IX = Rand.IY = Rand.IZ = sim.randomseed;
	// reset the PN register
	regPN[0] = 1;	regPN[1] = 0;	regPN[2] = 1;	regPN[3] = 0;
	regPN[4] = 0;	regPN[5] = 0;	regPN[6] = 1;	regPN[7] = 1;
	regPN[8] = 0;	regPN[9] = 0;	regPN[10] = 1;
	// set sigma
	double EbN0 = pow(10.0, sim.EbN0 / 10.0);
	sigma_n = 1.0 / sqrt(2 * MOD_BIT_PER_SYM * CodeRate * EbN0);
	return sigma_n;
}


//int CComm::Err(double simcycle, double& errFrame, double& errBit, double& errSym, double& fer, double& ber, double& ser)
int CComm::Err(CSimulation &sim)
{
	// BER, SER, FER
	double errSym, errBit, errFrame;
	errSym = errBit = errFrame = 0;
	for(int s = 0; s < MSG_SYM_LEN; s ++)
	{
		if(TX_MSG_SYM[s] != RX_MSG_SYM[s])
		{
			errSym ++;
		}
	}

	for(int b = 0; b < MSG_BIT_LEN; b ++)
	{
		if(TX_MSG_BIT[b] != RX_MSG_BIT[b])
		{
			errBit ++;
		}
	}
	sim.errSym += errSym;
	sim.errBit += errBit;
	sim.errFrame += (errSym != 0)? 1:0;

	if(DecodeCorrect && (errSym != 0))
	{
		sim.U_errSym += errSym;
		sim.U_errBit += errBit;
		sim.U_errFrame +=  1;
	}

	sim.SER = sim.errSym / (sim.simCycle * MSG_SYM_LEN);
	sim.BER = sim.errBit / (sim.simCycle * MSG_BIT_LEN);
	sim.FER = sim.errFrame / sim.simCycle;

	sim.U_SER = sim.U_errSym / (sim.simCycle * MSG_SYM_LEN);
	sim.U_BER = sim.U_errBit / (sim.simCycle * MSG_BIT_LEN);
	sim.U_FER = sim.U_errFrame / sim.simCycle;

	return 0;
}

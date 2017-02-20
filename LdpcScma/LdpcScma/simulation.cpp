#include <iostream>
#include <fstream>
#include <string>
#include "simulation.h"

using namespace std;

int GenPN(PNSeed *initPN)
{
	int result = 0;
	int i = 0;

	// shift the shift register
	for (i = 10; i >= 1; i--)
	{
		initPN->reg[i] = initPN->reg[i - 1];
	}

	// calculate the output
	initPN->reg[0] = initPN->reg[10] ^ initPN->reg[3];
	result = initPN->reg[10];

	// output the result
	return result;
}

void CSimulate::ReadProfile()
{
	/*打开Profile文件*/
	ifstream fin("Profile.txt");
	if (!fin.is_open())
	{
		cerr << "Cannot open Profile.txt" << endl;
		exit(0);
	}
	string rub;
	/*读取仿真参数*/
	fin >> rub >> rub;
	fin >> rub >> rub >> startSNR;
	fin >> rub >> rub >> stepSNR;
	fin >> rub >> rub >> stopSNR;
	fin >> rub >> rub >> RandomSeed;
	fin >> rub >> rub >> maxIteration;
	/*读取ldpc码文件*/
	fin >> rub >> rub;
	fin >> rub >> rub >> CodeFileName;
	/*读取调制参数*/
	fin >> rub >> rub;
	fin >> rub >> ModulationType;
	fin >> rub >> rub;
	fin >> rub >> DecodeAlgorithm;
	fin >> rub >> rub >> MS_alpha;
	fin.close();
}



void CSimulate::Initial()
{
	//读Profile
	ReadProfile();
	//初始化ldpc
	ldpc.ParamInit(maxIteration, DecodeAlgorithm, MS_alpha);
	ldpc.Initial(CodeFileName);
	ldpc.ConstructGenMatrix();//生成G矩阵
							  //初始化modulate

	//分配消息空间
	MsgSeq = new unsigned char[ldpc.MsgLen];
	MsgLen = ldpc.MsgLen;
	//输出初始化结果
	cout << "  Code File: " << CodeFileName << "\n  MaxInteration: " << ldpc.maxIteration
		<< "\tStart: " << startSNR << "  Stop: " << stopSNR << "  Step: " << stepSNR << '\n'
		<< "  Decoder Algorithm:  " << ldpc.DecodeAlgorithm << '\n' << endl;
}


void CSimulate::Simulate() {
	ReadProfile();
	Initial();
	for (int i = startSNR; i <= stopSNR; i = i + stepSNR)
	{
		double sigma;
		for (int ii = 0; ii < J; ii++)
		{
			for (int jj = 0; jj < MsgLen; jj++)
			{
				data[ii][jj] = GenPN(&PN);
			}
		}
		for (int ii = 0; ii < J; ii++)
		{
			ldpc.Encode(data[ii],ldpcData[ii]);
		}
		for (int ii = 0; ii < LdpcLen/2; ii++)
		{
			int ModSig[6];
			for (int jj = 0; jj < J; jj++)
			{
				if (ldpcData[jj][2 * ii] == 0 && ldpcData[jj][2 * ii + 1] == 0)
					ModSig[jj] = 0;
				if (ldpcData[jj][2 * ii] == 0 && ldpcData[jj][2 * ii + 1] == 1)
					ModSig[jj] = 1;
				if (ldpcData[jj][2 * ii] == 1 && ldpcData[jj][2 * ii + 1] == 0)
					ModSig[jj] = 2;
				if (ldpcData[jj][2 * ii] == 1 && ldpcData[jj][2 * ii + 1] == 1)
					ModSig[jj] = 3;
			}
			ScmaMapping(ModSig, ScmaDataRe[ii], ScmaDataIm[ii]);
			AwgnChannel.AWGNChannel(ScmaDataRe[ii], ScmaDataIm[ii], sigma, K);

		}
	}


}
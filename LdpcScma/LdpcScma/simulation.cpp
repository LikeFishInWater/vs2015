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
	/*��Profile�ļ�*/
	ifstream fin("Profile.txt");
	if (!fin.is_open())
	{
		cerr << "Cannot open Profile.txt" << endl;
		exit(0);
	}
	string rub;
	/*��ȡ�������*/
	fin >> rub >> rub;
	fin >> rub >> rub >> startSNR;
	fin >> rub >> rub >> stepSNR;
	fin >> rub >> rub >> stopSNR;
	fin >> rub >> rub >> RandomSeed;
	fin >> rub >> rub >> maxIteration;
	/*��ȡldpc���ļ�*/
	fin >> rub >> rub;
	fin >> rub >> rub >> CodeFileName;
	/*��ȡ���Ʋ���*/
	fin >> rub >> rub;
	fin >> rub >> ModulationType;
	fin >> rub >> rub;
	fin >> rub >> DecodeAlgorithm;
	fin >> rub >> rub >> MS_alpha;
	fin.close();
}



void CSimulate::Initial()
{
	//��Profile
	ReadProfile();
	//��ʼ��ldpc
	ldpc.ParamInit(maxIteration, DecodeAlgorithm, MS_alpha);
	ldpc.Initial(CodeFileName);
	ldpc.ConstructGenMatrix();//����G����
							  //��ʼ��modulate

	//������Ϣ�ռ�
	MsgSeq = new unsigned char[ldpc.MsgLen];
	MsgLen = ldpc.MsgLen;
	//�����ʼ�����
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
#pragma once
#include "memory.h"
#include "math.h"
class CSCMA {
public:
	int T;
	int **F;
	int **h;
	int **v;
	int **a;
	double *p;
	double ***Sre;
	double ***Sim;
	double ***D;
	double ***Dinit;
	double ***Dtemp;
	double **De;
	const double zeros4[4] = {};

	CSCMA();
	~CSCMA();
	bool ScmaMapping(int * ModSig, double* MapSigRe, double * MapSigIm);
	bool ScmaDecoding(double * RecRe, double * RecIm, double * LogDec, double ** pp, double sigma);

};


int T;//iteration number
int F[][4] = { { 0, 1, 0, 1 },{ 1, 0, 1, 0 },{ 1, 1, 0, 0 },{ 0, 0, 1, 1 },{ 1, 0, 0, 1 },{ 0, 1, 1, 0 } };
int h[][3] = { {2,3,5},{1,3,6},{2,4,6},{1,4,5} };
int v[][2] = { {2,4}, {1,3} ,{1,2}, {3,4}, {1,4} ,{2,3} };
int a[2][16] = { {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4} ,{0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3} };
double p[16];
double Sre[4][6][4] =
{
	{
		{ 0,-0.1815,0,0.7851 },{ 0.7851,0,-0.7815,0 },{ -0.6351 ,0.1392,0,0 },{ 0,0,0.7851,-0.0055 },{ -0.0055,0,0,-0.6351 },{ 0,0.7851,0.1392 ,0 }
	},
	{
		{ 0, -0.6351, 0, -0.2243 },{ -0.2243, 0, -0.6351 , 0 },{ 0.1815 , 0.4873, 0, 0 },{ 0, 0, -0.2243, -0.0193 },{ -0.0193 , 0, 0, 0.1815 },{ 0, -0.2243, 0.4873 , 0 }
	},
	{
		{ 0, 0.6351, 0, 0.2243 },{ 0.2242, 0, 0.6351, 0 } ,{ -0.1815 , -0.4873 , 0, 0 } ,{ 0, 0, 0.2243, 0.0193 } ,{ 0.0193 , 0, 0, -0.1815 },{ 0, 0.2243, -0.4873 , 0 }
	},
	{
		{ 0, 0.1815, 0, -0.7851 },{ -0.7851, 0, 0.1815, 0 } ,{ 0.6351, -0.1392, 0, 0 } ,{ 0, 0, -0.7851, 0.0055 },{ 0.0055 , 0, 0, 0.6351 } ,{ 0, -0.7851, -0.1392 , 0 }
	}
};
double Sim[4][6][4] =
{
	{
		{ 0, -0.1318,0,0 },{ 0, 0,  -0.1318, 0 },{ 0.4615,  -0.1759, 0, 0 },{ 0, 0, 0, -0.2242 },{ -0.2242, 0, 0, 0.4615 } ,{ 0, 0,  -0.1759, 0 }
	},
	{
		{ 0, -0.4615, 0, -0 },{ 0, 0, -0.4615 } ,{ -0.1318, -0.6156, 0, 0 } ,{ 0, 0,0,  -0.7848 },{ -0.7848, 0, 0,  -0.1318 },{ 0, 0, -0.6156, 0 }
	},
	{
		{ 0, 0.4615, 0,0 },{ 0, 0, 0.4615, 0 },{ 0.1318, 0.6156, 0, 0 },{ 0, 0, 0,  0.7848 } ,{ 0.7848, 0, 0, 0.1318 },{ 0, 0, 0.6156, 0 }
	},
	{
		{ 0, 0.1318, 0, 0 },{ 0, 0,  0.1318, 0 },{ 0.4615,  0.1759, 0, 0 } ,{ 0, 0, 0,  0.2242 } ,{ 0.2242, 0, 0,  -0.4615 },{ 0, 0, 0.1759, 0 }
	},
};
double D[4][6][4] ;
const double Dinit[4][6][4] = {};
double Dtemp[4][6][4];
double De[6][4];

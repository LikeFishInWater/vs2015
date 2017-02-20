#include "SCMA.h"

//ModSig[6],MapSigRe[4],MapSigIm[4]
bool ScmaMapping(int * ModSig, double * MapSigRe, double * MapSigIm )
{
	for (int i = 0; i < 4; i++)
	{
		MapSigRe[i] = 0;
		MapSigIm[i] = 0;
	}
	//MapSigRe = zeros4;
	//MapSigIm = zeros4;
	for(int i=0;i<4;i++)
		for (int j = 0; j < 6; j++)
		{
			MapSigRe[i] += Sre[ModSig[j]][j][i];
			MapSigIm[i] += Sim[ModSig[j]][j][i];
		}
	return true;
}
//RecRe[4],RecIm[4],LogDec[6][2],pp[6][4],sigma
bool ScmaDecoding(double * RecRe, double * RecIm, double ** LogDec ,double ** pp,double sigma)
{
	/*for (int ii = 0; ii < 4; ii++)
		for (int jj = 0; jj < 6; jj++)
			for (int mm = 0; mm < 4; mm++)
				D[ii][jj][mm] = Dinit[ii][jj][mm];*/
	memcpy(D, Dinit, sizeof(Dinit));
	for (int tt = 0; tt < T; tt++)
	{
		memcpy(Dtemp, D, sizeof(D));
		for (int jj = 0; jj < 6; jj++)
		{
			for (int mm = 0; mm < 4; mm++)
			{
				Dtemp[mm][jj][v[jj][0]] = Dtemp[mm][jj][v[jj][0]] * pp[jj][mm];
				Dtemp[mm][jj][v[jj][1]] = Dtemp[mm][jj][v[jj][1]] * pp[jj][mm];
			}
		}
		int he[2] = { 0,0 };
		for (int jj = 0; jj < 6; jj++)
		{
			for (int mm = 0; mm < 4; mm++)
			{
				he[0] = he[0] + Dtemp[mm][jj][v[jj][0]];
				he[1] = he[1] + Dtemp[mm][jj][v[jj][1]];
			}
			for (int mm = 0; mm < 4; mm++)
			{
				D[mm][jj][v[jj][0]] = Dtemp[mm][jj][v[jj][0]] / he[0];
				D[mm][jj][v[jj][1]] = Dtemp[mm][jj][v[jj][1]] / he[1];
			}
		}
		memcpy(Dtemp, D, sizeof(D));
		int he1 = 0;
		for (int kk = 0; kk < 4; kk++)
		{
			for (int mm = 0; mm < 4; mm++)
			{
				for (int nn = 0; nn < 16; nn++)
				{
					p[nn] = D[mm][h[kk][1]][a[0][nn]] * D[mm][h[kk][2]][a[1][nn]] * exp(-1 / (2 * pow(sigma, 2)) *
						(pow(RecRe[kk] - Sre[kk][h[kk][0]][mm] - Sre[kk][h[kk][1]][a[0][nn]] - Sre[kk][h[kk][2]][a[1][nn]], 2) + pow(RecIm[kk] - Sim[kk][h[kk][0]][mm] - Sim[kk][h[kk][1]][a[0][nn]] - Sim[kk][h[kk][2]][a[1][nn]], 2)));
				}
				he1 = 0;
				for (int nn = 0; nn < 16; nn++)
				{
					he1 += p[nn];
				}
				Dtemp[kk][h[kk][0]][mm] = he1;
				for (int nn = 0; nn < 16; nn++)
				{
					p[nn] = D[mm][h[kk][0]][a[0][nn]] * D[mm][h[kk][2]][a[1][nn]] * exp(-1 / (2 * pow(sigma, 2)) *
						(pow(RecRe[kk] - Sre[kk][h[kk][1]][mm] - Sre[kk][h[kk][0]][a[0][nn]] - Sre[kk][h[kk][2]][a[1][nn]], 2) + pow(RecIm[kk] - Sim[kk][h[kk][1]][mm] - Sim[kk][h[kk][0]][a[0][nn]] - Sim[kk][h[kk][2]][a[1][nn]], 2)));
				}
				he1 = 0;
				for (int nn = 0; nn < 16; nn++)
				{
					he1 += p[nn];
				}
				Dtemp[kk][h[kk][1]][mm] = he1;
				for (int nn = 0; nn < 16; nn++)
				{
					p[nn] = D[mm][h[kk][0]][a[0][nn]] * D[mm][h[kk][1]][a[1][nn]] * exp(-1 / (2 * pow(sigma, 2)) *
						(pow(RecRe[kk] - Sre[kk][h[kk][2]][mm] - Sre[kk][h[kk][0]][a[0][nn]] - Sre[kk][h[kk][1]][a[1][nn]], 2) + pow(RecIm[kk] - Sim[kk][h[kk][2]][mm] - Sim[kk][h[kk][0]][a[0][nn]] - Sim[kk][h[kk][1]][a[1][nn]], 2)));
				}
				he1 = 0;
				for (int nn = 0; nn < 16; nn++)
				{
					he1 += p[nn];
				}
				Dtemp[kk][h[kk][2]][mm] = he1;
			}
		}
		memcpy(D, Dtemp, sizeof(D));
	}	
	for (int jj = 0; jj < 6; jj++)
	{
		for (int mm = 0; mm < 4; mm++)
		{
			De[jj][mm] = D[mm][jj][v[jj][0]] * D[mm][jj][v[jj][1]];
			if (De[jj][mm] < 0.0001)
				De[jj][mm] = 0.0001;
		}
	}
	for (int jj = 0; jj < 6; jj++)
	{
		LogDec[jj][] = {log((De[jj][0]+De[jj][1])/(De[jj][2]+De[jj][3])),log((De[jj][0]+De[jj][2])/(De[jj][1]+De[jj][3]))};
	}
}
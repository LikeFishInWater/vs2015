#include<math.h>
#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#define pi 3.1415
#define A 1/(2*pi*sigma) /*���κ�ֵ��һ������û������ν*/
#define sigma 1.0          //��������
#define iterative 4       //������������
#define w 32              //��֯���w*w����
/*********���������Ϣ���еķ�ʽ*******************/
/******random1******/
void random1(int m[w][w])
{
	int i, j;
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			m[i][j] = rand() % 2;
}
/*******�����������*****/
void random2(int m[w][w])
{
	int i, j;
	time_t t;
	srand((unsigned)time(&t));
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
		{
			m[i][j] = rand() % 2;
		}
}
/*****����������***********/
void enc(int m[w][w], int v[w][w])
{
	int t, i, j, d[2];
	d[0] = 0;
	d[1] = 0;
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
		{
			t = d[0];
			d[0] = (m[i][j] + d[0] + d[1]) % 2;
			v[i][j] = (d[0] + d[1]) % 2;
			d[1] = t;
		}
}
/**********��֯��:double***************/
void transposef(double x[w][w])
{
	int i, j;
	double y[w][w];
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			y[i][j] = x[i][j];
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			x[j][i] = y[i][j];
}
/*********��֯��:int*****************/
void transposei(int x[w][w])
{
	int i, j;
	int y[w][w];
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			y[i][j] = x[i][j];
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			x[j][i] = y[i][j];
}
/************Turbo������**********************/
void encode(int u[w][w], int dk[w*w * 2])
{
	int v1[w][w], v2[w][w];
	int *p, *p1, *p2;
	static int d[2];
	int i;
	/*the encoder1 begin to work*/
	enc(u, v1);
	printf("\n");
	transposei(u);
	/*the encoder2 begin to work*/
	enc(u, v2);
	/*Delete and rebuild the V and out the dk[200]*/
	transposei(u);
	p = u[0];
	p1 = v1[0];
	p2 = v2[0] + 1;

	for (i = 0; i<w*w * 2; i++)
	{
		dk[i] = *p;
		p++; i++;
		dk[i] = *p1;  //ɾ��ż��λ
		p1 = p1 + 2; i++;
		dk[i] = *p;
		p++; i++;
		dk[i] = *p2;  //ɾ������λ 
		p2 = p2 + 2;    //
	}
}
/********BPSK����*******************/
void bpsk(int x[w*w * 2])
{
	int i;
	for (i = 0; i<w*w * 2; i++)
	{
		if (x[i] == 1) x[i] = 1;
		else x[i] = -1;
	}
}
/*******��֯��:double*********************/
void transpose_f(double x[w][w], double y[w][w])
{
	int i, j;
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			y[i][j] = x[j][i];
}
/*********��˹������******************/
void gauss2(double fc, double z[w*w * 2])
{
	double u[w*w * 2 + 1];
	long int p = 2;
	long int a = 5;
	int i;
	for (i = 0; i<30; i++)
		p = p * 2;
	p = p - 1;
	for (i = 0; i<6; i++)
		a = a * 5;

	u[0] = 17;
	u[0] = fmod(a*u[0], p);

	for (i = 0; i<w*w * 2; i++)
		u[i + 1] = fmod(a*u[i], p);

	for (i = 0; i<w*w * 2 + 1; i++)
		u[i] = u[i] / p;

	for (i = 0; i<w*w * 2; i++)
		z[i] = sqrt(fc)*sqrt(-2 * log(u[i]))*cos(2 * pi*u[i + 1]);
}

/******Ϊ���������յ�������C������Ϣ����X��У������Y1 Y2****/
void classed(double c[w*w * 2], double x[w][w], double y1[w][w], double y2[w][w])
{
	double *px, *py1, *py2;
	double *o1, *o2, *o3;
	/*������Ϣ����X*/
	o1 = c;
	for (px = x[0]; px<x[0] + w*w; px++)
	{
		*px = *o1;
		o1 = o1 + 2;
	}
	/*****����У������Y1******/
	o2 = c + 1;
	for (py1 = y1[0]; py1<y1[0] + w*w; py1++)
	{
		*py1 = *o2;
		py1 = py1 + 1;
		*py1 = 0;/*second delete,so second compensate"0"*/
		o2 = o2 + 4;
	}
	/*****����У������Y2******/
	o3 = c + 3;
	for (py2 = y2[0]; py2<y2[0] + w*w; py2++)
	{
		*py2 = 0;
		py2 = py2 + 1;
		*py2 = *o3;
		o3 = o3 + 4;
	}
}
/*********������������MAP�㷨********************/
void map(double x[w][w], double y[w][w], double z1[w][w], double ld[w][w])
{
	double u, v;
	double*px, *py, *pz1, *pld;
	double r1[4][w*w], r0[4][w*w];
	double a[4][w*w + 1], b[4][w*w + 1];
	double t1, t0, sa, sb;
	int i, j;
	u = v = 1.0;
	/*�����֧����r1*/
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r1[0][i] = A*(exp(*pz1) / (1 + exp(*pz1)))*exp(((*px) - (u))*((*px) - (u)) /
			(-2 * sigma))*exp(((*py) - (v))*((*py) - (v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r1[1][i] = A*(exp(*pz1) / (1 + exp(*pz1)))*exp(((*px) - (u))*((*px) - (u)) / (-2 * sigma))
			*exp(((*py) - (-v))*((*py) - (-v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r1[2][i] = A*(exp(*pz1) / (1 + exp(*pz1)))*exp(((*px) - (u))*((*px) - (u)) /
			(-2 * sigma))*exp(((*py) - (v))*((*py) - (v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r1[3][i] = A*(exp(*pz1) / (1 + exp(*pz1)))*exp(((*px) - (u))*((*px) - (u)) /
			(-2 * sigma))*exp(((*py) - (-v))*((*py) - (-v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	/*�����֧����r0*/
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r0[0][i] = A*(1 / (1 + exp(*pz1)))*exp(((*px) - (-u))*((*px) - (-u)) /
			(-2 * sigma))*exp(((*py) - (-v))*((*py) - (-v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r0[1][i] = A*(1 / (1 + exp(*pz1)))*exp(((*px) - (-u))*((*px) - (-u)) /
			(-2 * sigma))*exp(((*py) - (v))*((*py) - (v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r0[2][i] = A*(1 / (1 + exp(*pz1)))*exp(((*px) - (-u))*((*px) - (-u)) /
			(-2 * sigma))*exp(((*py) - (-v))*((*py) - (-v)) / (-2 * sigma));
		pz1++; px++; py++;
	}
	px = x[0]; py = y[0]; pz1 = z1[0];
	for (i = 0; i<w*w; i++)
	{
		r0[3][i] = A*(1 / (1 + exp(*pz1)))*exp(((*px) - (-u))*((*px) - (-u)) /
			(-2 * sigma))*exp(((*py) - (v))*((*py) - (v)) / (-2 * sigma));
		pz1++; px++; py++;
	}

	/****����ǰ��״̬����*****/
	a[0][0] = 1; a[1][0] = 0; a[2][0] = 0; a[3][0] = 0;
	for (j = 1; j<(w*w + 1); j++)
	{
		a[0][j] = a[0][j - 1] * r0[0][j - 1] + a[2][j - 1] * r1[2][j - 1];
		a[1][j] = a[0][j - 1] * r1[0][j - 1] + a[2][j - 1] * r0[2][j - 1];
		a[2][j] = a[1][j - 1] * r1[1][j - 1] + a[3][j - 1] * r0[3][j - 1];
		a[3][j] = a[1][j - 1] * r0[1][j - 1] + a[3][j - 1] * r1[3][j - 1];
		sa = a[0][j] + a[1][j] + a[2][j] + a[3][j]; //ǰ��״̬������һ��
		a[0][j] = a[0][j] / sa;
		a[1][j] = a[1][j] / sa;
		a[2][j] = a[2][j] / sa;
		a[3][j] = a[3][j] / sa;
	}
	/*****�������״̬����******/
	b[0][w*w] = 0.25; b[1][w*w] = 0.25; b[2][w*w] = 0.25; b[3][w*w] = 0.25;
	for (j = w*w - 1; j >= 0; j--)
	{
		b[0][j] = b[0][j + 1] * r0[0][j] + b[1][j + 1] * r1[0][j];
		b[1][j] = b[2][j + 1] * r1[1][j] + b[3][j + 1] * r0[1][j];
		b[2][j] = b[0][j + 1] * r1[2][j] + b[1][j + 1] * r0[2][j];
		b[3][j] = b[2][j + 1] * r0[3][j] + b[3][j + 1] * r1[3][j];
		sb = b[0][j] + b[1][j] + b[2][j] + b[3][j];//����״̬������һ��
		b[0][j] = b[0][j] / sb;
		b[1][j] = b[1][j] / sb;
		b[2][j] = b[2][j] / sb;
		b[3][j] = b[3][j] / sb;
	}
	/*��������ϢLe2*/
	pld = ld[0];
	for (j = 0; j<w*w; j++)
	{
		t1 = r1[0][j] * a[0][j] * b[1][j + 1] + r1[1][j] * a[1][j] * b[2][j + 1] + r1[2][j] *
			a[2][j] * b[0][j + 1] + r1[3][j] * a[3][j] * b[3][j + 1];
		t0 = r0[0][j] * a[0][j] * b[0][j + 1] + r0[1][j] * a[1][j] * b[3][j + 1] + r0[2][j] *
			a[2][j] * b[1][j + 1] + r0[3][j] * a[3][j] * b[2][j + 1];
		(*pld++) = log(t1 / t0);
	}
}
/*************������*********************/
void decode(double x[w][w], double y1[w][w], double y2[w][w], int dk[w][w])
{
	double z1[w][w], z2[w][w], ld[w][w], ldk[w][w], ldn[w][w];
	int i, j, k;
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			z1[i][j] = 0;
	for (k = 0; k<100; k++)
	{
		map(x, y1, z1, ld);
		for (i = 0; i<w; i++)
			for (j = 0; j<w; j++)
				z2[i][j] = ld[i][j] - z1[i][j] - 2 * x[i][j] / sigma;

		transpose_f(z2, z1);
		transposef(x);

		map(x, y2, z1, ld);
		for (i = 0; i<w; i++)
			for (j = 0; j<w; j++)
				z2[i][j] = ld[i][j] - z1[i][j] - 2 * x[i][j] / sigma;

		if (k == iterative)break;  //������������
		transposef(x);
		transpose_f(z2, z1);
	}
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			ldn[i][j] = z1[i][j] + 2 * x[i][j] / sigma + z2[i][j];
	transpose_f(ldn, ldk);
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
		{
			if (ldk[i][j]>0)dk[i][j] = 1;
			else dk[i][j] = 0;
		}
}
/*************������***********************/
void main()
{
	int m[w][w];  //����
	int d[w*w * 2];
	double c[w*w * 2]; //��������
	double x[w][w], y1[w][w], y2[w][w];//����
	int md[w][w];
	int i, j;
	double num, pe;//ͳ������
	random1(m); //�����������Ϣ����
	encode(m, d); //����Ϣ���б���
	bpsk(d);  //�Ա�������BPSK����
	gauss2(sigma, c);//������˹������
	for (i = 0; i<w*w * 2; i++)//�����ֵ�������
		c[i] = c[i] + d[i];
	classed(c, x, y1, y2);//�������˷�����Ϣ���к�У������
	decode(x, y1, y2, md);//������
	num = 0.0;//ͳ��������
	for (i = 0; i<w; i++)
		for (j = 0; j<w; j++)
			if (md[i][j] != m[i][j]) num = num + 1.0;
	pe = num / (w*w);
	printf("the number of wrong is%f\n", num);
	printf("the pe is%e\n", pe);
	while (1) {};
	return 0;
}
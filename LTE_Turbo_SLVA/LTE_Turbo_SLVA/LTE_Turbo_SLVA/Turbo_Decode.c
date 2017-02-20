#include <math.h>
#include"define.h"
#include "Turbo_Decode.h"
#include"struct.h"
#include "CRC_Decode.h"
#include "SLVA.h"
#include  "Viterbi_Decode.h"
/*
********************************************************************************
* NAME:			                        TurboDecode
* PURPOSE:		                        对一个码块进行Turbo迭代译码，Max-Log-Map
*
* Input:
infoBitLen:		                         码块的长度
input_length:                            分量译码器每个端口的输入序列长度=infoBitLen+3
decoderIn_systemBit                      进入分量译码器1的系统位
decoderIn_parityBit                      进入分量译码器1的校验位
decoderIn_afterInterleave_systemBit      进入分量译码器2的交织后系统位
decoderIn_iparityBit                     进入分量译码器2的校验位
f1:				                         QPP交织器的参数
f2:				                         QPP交织器的参数
*
* Output:                                迭代译码后分量译码器2输出的信息比特最终似然比，用于硬判。
L                                         
*
* AUTHOR: 		Yijia Xu
********************************************************************************
*/
//struct state trellis[3084+4][encoder_stateNum];
/*定义循环变量*/
	

	

void TurboDecode_SLVA(int infoBitLen,int input_length,int *gen,double *decoderIn_systemBit,double *decoderIn_parityBit,
	double *decoderIn_afterInterleave_systemBit,double *decoderIn_iparityBit,int f1,int f2,double *L,double *exLLR,int *DecoderOutput,
	struct state *trellis,double sigma2){
	int hardDec_i;
	int IT_i;
	double *L1;//分量译码器1输出的对数似然比（软量）
	double *La1;//分量译码器1的先验信息
	double *Le1;//分量译码器1输出的外附信息，作为分量编码器2的先验信息
	double *L2;//分量译码器2输出的对数似然比（软量）
	double *La2;//分量译码器2的先验信息
	double *Le2;//分量译码器2输出的外附信息，作为分量编码器1的先验信息 
	double *La_temp;
	double *La_par_temp;
	double *L_par;//校验比特软量
	double *L_ipar,*temp_L_ipar;//校验比特软量
	double *Le_par1;//分量译码器1输出的校验比特外信息
	double *La_par1;//分量译码器1的先验信息
	double *Le_par2;//分量译码器2输出的校验比特外信息
	double *La_par2;//分量译码器2的先验信息
	double *sum_LLR;//for OSD,软量叠加

	int *temp;
	//上述软量都不包括尾比特。
	int innerIT=ITNum;//内迭代次数。
	int IT_num=0;
	//struct state trellis[1024+4][encoder_stateNum];
	int i;
	
	L1 = (double *)malloc((input_length +1)*sizeof(double));
	if (L1 == NULL)
	{
		printf("Can not malloc L1!\n");
		getch();
		exit(0);
	}
	La1 = (double *)malloc((input_length +1)*sizeof(double));
	if (La1 == NULL)
	{
		printf("Can not malloc La1!\n");
		getch();
		exit(0);
	}
	Le1 = (double *)malloc((input_length +1)*sizeof(double));
	if (Le1 == NULL)
	{
		printf("Can not malloc Le1!\n");
		getch();
		exit(0);
	}
	L2 = (double *)malloc((input_length +1)*sizeof(double));
	if (L2 == NULL)
	{
		printf("Can not malloc L2!\n");
		getch();
		exit(0);
	}
	La2 = (double *)malloc((input_length +1)*sizeof(double));
	if (La2 == NULL)
	{
		printf("Can not malloc La2!\n");
		getch();
		exit(0);
	}
	Le2 = (double *)malloc((input_length +1)*sizeof(double));
	if (Le2 == NULL)
	{
		printf("Can not malloc Le2!\n");
		getch();
		exit(0);
	}
	La_temp = (double *)malloc((input_length +1)*sizeof(double));
	if (La_temp == NULL)
	{
		printf("Can not malloc La_temp!\n");
		getch();
		exit(0);
	}
	La_par_temp = (double *)malloc((input_length +1)*sizeof(double));
	if (La_par_temp == NULL)
	{
		printf("Can not malloc La_par_temp!\n");
		getch();
		exit(0);
	}
	L_par = (double *)malloc((input_length +1)*sizeof(double));
	if (L_par == NULL)
	{
		printf("Can not malloc L_par!\n");
		getch();
		exit(0);
	}
	
	L_ipar = (double *)malloc((input_length +1)*sizeof(double));
	if (L_ipar == NULL)
	{
		printf("Can not malloc L_ipar!\n");
		getch();
		exit(0);
	}
	temp_L_ipar = (double *)malloc((input_length +1)*sizeof(double));
	if (temp_L_ipar == NULL)
	{
		printf("Can not malloc temp_L_ipar!\n");
		getch();
		exit(0);
	}
	Le_par1 = (double *)malloc((input_length +1)*sizeof(double));
	if (Le_par1 == NULL)
	{
		printf("Can not malloc Le_par1!\n");
		getch();
		exit(0);
	}
	La_par1 = (double *)malloc((input_length +1)*sizeof(double));
	if (La_par1 == NULL)
	{
		printf("Can not malloc La_par1!\n");
		getch();
		exit(0);
	}
	Le_par2 = (double *)malloc((input_length +1)*sizeof(double));
	if (Le_par2 == NULL)
	{
		printf("Can not malloc Le_par2!\n");
		getch();
		exit(0);
	}
	La_par2 = (double *)malloc((input_length +1)*sizeof(double));
	if (La_par2 == NULL)
	{
		printf("Can not malloc La_par2!\n");
		getch();
		exit(0);
	}
	sum_LLR = (double *)malloc((3*maxInfoLength +1)*sizeof(double));
	if (sum_LLR == NULL)
	{
		printf("Can not malloc sum_LLR!\n");
		getch();
		exit(0);
	}

	//用于记录QPP交织器较之前后对应地址，用于解交织时查表。
	temp=(int *)malloc((infoBitLen+1)*sizeof(int));
	if (temp == NULL)
	{
		printf("Can not malloc temp!\n");
		getch();
		exit(0);
	}

	memset(L1,0,(input_length +1)*sizeof(double));
	memset(La1,0,(input_length +1)*sizeof(double));//首次迭代时初始化分量译码器1的先验概率
	memset(Le1,0,(input_length +1)*sizeof(double));
	memset(L2,0,(input_length +1)*sizeof(double));
	memset(La2,0,(input_length +1)*sizeof(double));
	memset(Le2,0,(input_length +1)*sizeof(double));
	memset(La_temp,0,(input_length +1)*sizeof(double));
	memset(La_par_temp,0,(input_length +1)*sizeof(double));
	memset(temp,0,(infoBitLen +1)*sizeof(int));
	memset(L_par,0,(input_length +1)*sizeof(double));
	memset(L_ipar,0,(input_length +1)*sizeof(double));
	memset(temp_L_ipar,0,(input_length +1)*sizeof(double));
	memset(Le_par1,0,(input_length +1)*sizeof(double));
	memset(La_par1,0,(input_length +1)*sizeof(double));
	memset(Le_par2,0,(input_length +1)*sizeof(double));
	memset(La_par2,0,(input_length +1)*sizeof(double));
	memset(sum_LLR,0,(3*maxInfoLength +1)*sizeof(double));

	//Turbo迭代译码
	for(IT_i=0;IT_i<innerIT;IT_i++){
			IT_num++;
			
		//分量译码器1译码
		Turbo_Component_Decoder(input_length, decoderIn_systemBit, decoderIn_parityBit, La1,L1,Le1,trellis,L_par,La_par1,Le_par1,sigma2);
	

		//对Le1进行交织，用作分量译码器2的先验信息
		QppInterleave(infoBitLen,Le1,La2,temp,f1,f2);
		//QppInterleave(infoBitLen,Le_par1,La_par2,temp,f1,f2);
	
		//分量译码器2译码
		Turbo_Component_Decoder(input_length, decoderIn_afterInterleave_systemBit, decoderIn_iparityBit, La2,L2,Le2,trellis,temp_L_ipar,La_par2,Le_par2,sigma2);
	
		//if(IT_num>3)  *L+=*L2;
		//对Le2进行解交织，用作分量译码器1的先验信息
		QppDeInterleave(infoBitLen,Le2,La1,temp);
		//QppDeInterleave(infoBitLen,Le_par2,La_par1,temp);

		if(IT_num>start_culLe_IT)  {           //修改此处startIT值，从此次迭代开始累加外信息
			for(i=0;i<infoBitLen;i++){
			La_temp[i]+=La1[i];
			La_par_temp[i]+=La_par1[i];
		}
		}
		else {
			for(i=0;i<infoBitLen;i++){
			La_temp[i]=La1[i];
			La_par_temp[i]=La_par1[i];
		}
		}   

		QppDeInterleave(infoBitLen,L2,L,temp);
	//	QppDeInterleave(infoBitLen,temp_L_ipar,L_ipar,temp);
		//construct exLLR
			for(i=0;i<infoBitLen;i++){
				exLLR[i]=L[i];
				//printf("L_sys=%f\t",L[i]);
				exLLR[infoBitLen+i]=L_par[i];
				//printf("L_par=%f\t",L_par[i]);
				exLLR[2*infoBitLen+i]=temp_L_ipar[i];
				//printf("L_ipar=%f\n",L_ipar[i]);
		}

		//hard dicision
		for (hardDec_i=0;hardDec_i<infoBitLen;hardDec_i++){
			//printf("L=%f\n",L[hardDec_i]);
			if(L[hardDec_i]>=0) DecoderOutput[hardDec_i]=0;
			else DecoderOutput[hardDec_i]=1;
		}

		if(IT_num>=abs(OSD_sum_flag))
			{
				memadd(sum_LLR, sum_LLR, exLLR, 3*infoBitLen, OSD_acc_factor);
			}
		

		 if(CrcCheck(DecoderOutput, infoBitLen, 24, 0)==1) break;//earlyStop
		/*
		if(addSLVA==1){
		if(IT_num>=start_slva_IT){	
			for(i=0;i<infoBitLen;i++){
			//La_temp[i]=La_temp[i]/(ITNum-start_culLe_IT+1);
		}
			func_SLVA(infoBitLen,input_length,decoderIn_systemBit,decoderIn_parityBit,DecoderOutput,
			trellis,slva1,AbsoluteDiff,BestPathState,slva2,La_temp,La_par_temp,sigma2);
		if(CrcCheck(DecoderOutput, infoBitLen, 24, 0)==1) break;
		}
		}*/

		 if((IT_num>=OSD_sum_flag)&&(OSD_sum_flag>0))
			{
				OSD_process(DecoderOutput, exLLR, gen, infoBitLen);
				if (CrcCheck(DecoderOutput, infoBitLen, 24, 0) == 1)
				{
					break;
				}
			}
		if((IT_num>=8)&&(OSD_sum_flag<0))
		{
			OSD_process(DecoderOutput, sum_LLR, gen, infoBitLen);
		}
	
	}
	
	

	free(L1);
	L1=NULL;
	free(La1);
	La1=NULL;
	free(Le1);
	Le1=NULL;
	free(L2);
	L2=NULL;
	free(La2);
	La2=NULL;
	free(Le2);
	Le2=NULL;
	free(La_temp);
	La_temp=NULL;
	free(La_par_temp);
	La_par_temp=NULL;
	free(temp);
	temp=NULL;
	free(L_par);
	L_par=NULL;
	free(L_ipar);
	L_ipar=NULL;
	free(temp_L_ipar);
	temp_L_ipar=NULL;
	free(Le_par1);
	Le_par1=NULL;
	free(La_par1);
	La_par1=NULL;
	free(Le_par2);
	Le_par2=NULL;
	free(La_par2);
	La_par2=NULL;
	free(sum_LLR);
	sum_LLR=NULL;
}

//求N维数组中的最大值函数
double max_2(double a,double b){
	if(a>=b) 
		 return a;
	else 
		return b;
	
}

double max_n(double *d,int n){
	if (n==2)
		return max_2(d[0],d[1]);
	else
		return max_2(max_n(d,n-1),d[n-1]);
}
double max_n_star(double *d, int n){
	double temp[4];
	
	temp[0] = max_2_star(d[0], d[1]);
	temp[1] = max_2_star(d[2], d[3]);
	temp[2] = max_2_star(d[4], d[5]);
	temp[3] = max_2_star(d[6], d[7]);

	temp[0] = max_2_star(temp[0], temp[1]);
	temp[1] = max_2_star(temp[2], temp[3]);

	return 	max_2_star(temp[0], temp[1]);
}

double max_2_star(double a, double b){
	double max;
	double min;
	double temp;

	if (a>b)
	{
		max = a;
		min = b;
	}
	else{
		max=b;
		min=a;
	}

	temp = max + log(1 + exp(min - max));

	return temp;
}

//交织函数，temp记录对应地址，用于解交织时直接读取。
void QppInterleave(int infoBitLen,double *beforeQpp,double *afterQpp,int *temp,int f1,int f2){
	int interleaveAddr= 0;	//Turbo_Interleave address
	int g;
	int Qpp_i;
	g = f1 + f2;
	for (Qpp_i=0; Qpp_i<infoBitLen; Qpp_i++)
	{
		temp[Qpp_i]=interleaveAddr;
		afterQpp[Qpp_i]=beforeQpp[interleaveAddr];
		//compute next Turbo_Interleave address
		interleaveAddr = (g + interleaveAddr)%infoBitLen;
		g = (g + 2*f2)%infoBitLen;
		//printf("%d",temp[Qpp_i]);
	}

}

//解交织函数
void QppDeInterleave(int infoBitLen,double *beforeDeQpp,double *afterDeQpp,int *temp){
	int j;
	int DeQpp_i;
	for(DeQpp_i=0;DeQpp_i<infoBitLen;DeQpp_i++){
		j=temp[DeQpp_i];
		afterDeQpp[j]=beforeDeQpp[DeQpp_i];	
	}

}

//Turbo分量译码器
void Turbo_Component_Decoder(int input_length,double *systemBit,double *parityBit,double *La,double *L,double *Le,struct state *trellis,double *L_par,double *La_par,double *Le_par,double sigma2){
	int LLR_i;
	initial_trellis_Turbo(input_length,trellis);
	Branch_gamma(input_length,systemBit,parityBit,La,La_par,trellis,sigma2);
	Forward_alpha(input_length,trellis);
	Backward_beta(input_length,trellis);
	for(LLR_i=0;LLR_i<input_length-encoder_regNum;LLR_i++){
	//	Likehood_Ratio_L(LLR_i,trellis,L_par);
	//	printf("L_par1=%f\n",L_par[LLR_i]);
		L[LLR_i]=Likehood_Ratio_L(LLR_i,trellis,L_par);
//		printf("L_par2=%f\n",L_par[LLR_i]);
		Le[LLR_i]=L[LLR_i]-2*systemBit[LLR_i]/sigma2-2*normfactor*La[LLR_i];
		Le_par[LLR_i]=L_par[LLR_i]-2*parityBit[LLR_i]-2*normfactor*La_par[LLR_i];
		//Le[LLR_i]=L[LLR_i]-systemBit[LLR_i]-La[LLR_i];
	}
}




void initial_trellis_Turbo(int input_length,struct state *trellis){
	int tre_i,tre_j;
for(tre_i=0;tre_i<input_length+1;tre_i++){
	trellis[encoder_stateNum*tre_i+0].last_state1=0;trellis[encoder_stateNum*tre_i+0].trans_in1=0;trellis[encoder_stateNum*tre_i+0].trans_out1[0]=0;trellis[encoder_stateNum*tre_i+0].trans_out1[1]=0;  trellis[encoder_stateNum*tre_i+0].last_state2=4;trellis[encoder_stateNum*tre_i+0].trans_in2=1;trellis[encoder_stateNum*tre_i+0].trans_out2[0]=1;trellis[encoder_stateNum*tre_i+0].trans_out2[1]=1;
	trellis[encoder_stateNum*tre_i+1].last_state1=0;trellis[encoder_stateNum*tre_i+1].trans_in1=1;trellis[encoder_stateNum*tre_i+1].trans_out1[0]=1;trellis[encoder_stateNum*tre_i+1].trans_out1[1]=1;  trellis[encoder_stateNum*tre_i+1].last_state2=4;trellis[encoder_stateNum*tre_i+1].trans_in2=0;trellis[encoder_stateNum*tre_i+1].trans_out2[0]=0;trellis[encoder_stateNum*tre_i+1].trans_out2[1]=0;
	trellis[encoder_stateNum*tre_i+2].last_state1=1;trellis[encoder_stateNum*tre_i+2].trans_in1=0;trellis[encoder_stateNum*tre_i+2].trans_out1[0]=0;trellis[encoder_stateNum*tre_i+2].trans_out1[1]=1;  trellis[encoder_stateNum*tre_i+2].last_state2=5;trellis[encoder_stateNum*tre_i+2].trans_in2=1;trellis[encoder_stateNum*tre_i+2].trans_out2[0]=1;trellis[encoder_stateNum*tre_i+2].trans_out2[1]=0;
	trellis[encoder_stateNum*tre_i+3].last_state1=1;trellis[encoder_stateNum*tre_i+3].trans_in1=1;trellis[encoder_stateNum*tre_i+3].trans_out1[0]=1;trellis[encoder_stateNum*tre_i+3].trans_out1[1]=0;  trellis[encoder_stateNum*tre_i+3].last_state2=5;trellis[encoder_stateNum*tre_i+3].trans_in2=0;trellis[encoder_stateNum*tre_i+3].trans_out2[0]=0;trellis[encoder_stateNum*tre_i+3].trans_out2[1]=1;
	trellis[encoder_stateNum*tre_i+4].last_state1=2;trellis[encoder_stateNum*tre_i+4].trans_in1=1;trellis[encoder_stateNum*tre_i+4].trans_out1[0]=1;trellis[encoder_stateNum*tre_i+4].trans_out1[1]=0;  trellis[encoder_stateNum*tre_i+4].last_state2=6;trellis[encoder_stateNum*tre_i+4].trans_in2=0;trellis[encoder_stateNum*tre_i+4].trans_out2[0]=0;trellis[encoder_stateNum*tre_i+4].trans_out2[1]=1;
	trellis[encoder_stateNum*tre_i+5].last_state1=2;trellis[encoder_stateNum*tre_i+5].trans_in1=0;trellis[encoder_stateNum*tre_i+5].trans_out1[0]=0;trellis[encoder_stateNum*tre_i+5].trans_out1[1]=1;  trellis[encoder_stateNum*tre_i+5].last_state2=6;trellis[encoder_stateNum*tre_i+5].trans_in2=1;trellis[encoder_stateNum*tre_i+5].trans_out2[0]=1;trellis[encoder_stateNum*tre_i+5].trans_out2[1]=0;
	trellis[encoder_stateNum*tre_i+6].last_state1=3;trellis[encoder_stateNum*tre_i+6].trans_in1=1;trellis[encoder_stateNum*tre_i+6].trans_out1[0]=1;trellis[encoder_stateNum*tre_i+6].trans_out1[1]=1;  trellis[encoder_stateNum*tre_i+6].last_state2=7;trellis[encoder_stateNum*tre_i+6].trans_in2=0;trellis[encoder_stateNum*tre_i+6].trans_out2[0]=0;trellis[encoder_stateNum*tre_i+6].trans_out2[1]=0;
	trellis[encoder_stateNum*tre_i+7].last_state1=3;trellis[encoder_stateNum*tre_i+7].trans_in1=0;trellis[encoder_stateNum*tre_i+7].trans_out1[0]=0;trellis[encoder_stateNum*tre_i+7].trans_out1[1]=0;  trellis[encoder_stateNum*tre_i+7].last_state2=7;trellis[encoder_stateNum*tre_i+7].trans_in2=1;trellis[encoder_stateNum*tre_i+7].trans_out2[0]=1;trellis[encoder_stateNum*tre_i+7].trans_out2[1]=1;
	
	for (tre_j=0;tre_j<encoder_stateNum;tre_j++){
	    trellis[encoder_stateNum*tre_i+tre_j].trans_out1[0]=1-2*trellis[encoder_stateNum*tre_i+tre_j].trans_out1[0];
		trellis[encoder_stateNum*tre_i+tre_j].trans_out1[1]=1-2*trellis[encoder_stateNum*tre_i+tre_j].trans_out1[1];
		trellis[encoder_stateNum*tre_i+tre_j].trans_out2[0]=1-2*trellis[encoder_stateNum*tre_i+tre_j].trans_out2[0];
		trellis[encoder_stateNum*tre_i+tre_j].trans_out2[1]=1-2*trellis[encoder_stateNum*tre_i+tre_j].trans_out2[1];		
	}
	

}
for(tre_j=0;tre_j<encoder_stateNum;tre_j++){
	if(tre_j==0){
		trellis[encoder_stateNum*0+tre_j].alpha=0;
		trellis[encoder_stateNum*input_length+tre_j].beta=0;
	}
	else{
		trellis[0*encoder_stateNum+tre_j].alpha=-1e7;
		trellis[input_length*encoder_stateNum+tre_j].beta =-1e7;
	}
}
}

void Branch_gamma(int input_length,double dV1[],double dV2[],double *La,double *La_par,struct state *trellis,double sigma2){
int tre_i,tre_j;
	for(tre_i=0;tre_i<input_length-encoder_regNum;tre_i++){
		//printf("i=%d\n",tre_i+1);
		for(tre_j=0;tre_j<encoder_stateNum;tre_j++){
			trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma1=normfactor*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[0]*La[tre_i]
		/*	+normfactor*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[1]*La_par[tre_i]*/
			+(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[1])/sigma2;
		//	+(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[1]);

			trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma2=normfactor*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[0]*La[tre_i]
		/*	+normfactor*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[1]*La_par[tre_i]*/
			+(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[1])/sigma2;
		//	+(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[1]);


			//printf("gamma1=%f\tgamma2=%f\n",trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma1,trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma2);
		}
	}
	for (tre_i=input_length-encoder_regNum;tre_i<input_length;tre_i++){
		//printf("i=%d\n",tre_i+1);
		for(tre_j=0;tre_j<encoder_stateNum;tre_j++){
		trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma1=(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[1])/sigma2;
//trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma1=(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out1[1]);

		trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma2=(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[1])/sigma2;
//trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma2=(dV1[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[0]+dV2[tre_i]*trellis[encoder_stateNum*(tre_i+1)+tre_j].trans_out2[1]);
		//printf("dv1=%f\tdv2=%f\n",dV1[tre_i],dV2[tre_i]);
		//printf("gamma1=%f\tgamma2=%f\n",trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma1,trellis[encoder_stateNum*(tre_i+1)+tre_j].gamma2);
		}
	}
}

void Forward_alpha(int input_length,struct state *trellis){
	int tre_i,tre_j;
	for(tre_i=1;tre_i<input_length;tre_i++){
		for(tre_j=0;tre_j<encoder_stateNum;tre_j++){
			trellis[encoder_stateNum*tre_i+tre_j].alpha=max_2_star(trellis[encoder_stateNum*tre_i+tre_j].gamma1+trellis[encoder_stateNum*(tre_i-1)+trellis[encoder_stateNum*tre_i+tre_j].last_state1].alpha,trellis[encoder_stateNum*tre_i+tre_j].gamma2+trellis[encoder_stateNum*(tre_i-1)+trellis[encoder_stateNum*tre_i+tre_j].last_state2].alpha);
		/*if(i<=10&&j<10)
			printf("i=%dj=%dtrellis[encoder_stateNum*i+j].alpha1=%ftrellis[encoder_stateNum*i+j].alpha2=%f\n",i,j,trellis[encoder_stateNum*i+j].gamma1+trellis[i-1][trellis[encoder_stateNum*i+j].last_state1].alpha,trellis[encoder_stateNum*i+j].gamma2+trellis[i-1][trellis[encoder_stateNum*i+j].last_state2].alpha);*/
				}
	}
}

void Backward_beta(int input_length,struct state *trellis){
	int tre_i,tre_j;
	for(tre_i=input_length-1;tre_i>0;tre_i--){
		for(tre_j=0;tre_j<encoder_stateNum;tre_j++){
			int next_state1=2*(tre_j%(encoder_stateNum/2));
			int next_state2=2*(tre_j%(encoder_stateNum/2))+1;
			if(tre_j<encoder_stateNum/2){
				trellis[encoder_stateNum*tre_i+tre_j].beta=max_2_star((trellis[encoder_stateNum*(tre_i+1)+next_state1].gamma1+trellis[encoder_stateNum*(tre_i+1)+next_state1].beta),(trellis[encoder_stateNum*(tre_i+1)+next_state2].gamma1+trellis[encoder_stateNum*(tre_i+1)+next_state2].beta));	
			}
			else{
				trellis[encoder_stateNum*tre_i+tre_j].beta=max_2_star((trellis[encoder_stateNum*(tre_i+1)+next_state1].gamma2+trellis[encoder_stateNum*(tre_i+1)+next_state1].beta),(trellis[encoder_stateNum*(tre_i+1)+next_state2].gamma2+trellis[encoder_stateNum*(tre_i+1)+next_state2].beta));	
			}
		}
	}
}

double Likehood_Ratio_L(int i,struct state *trellis,double *L_par){
	
	
		double inis1[encoder_stateNum],inis0[encoder_stateNum];
		double inis1_par[encoder_stateNum],inis0_par[encoder_stateNum];
		double max_inis1,max_inis0;
		double max_inis1_par,max_inis0_par;
	
		inis1[0]=trellis[encoder_stateNum*i+0].alpha+trellis[encoder_stateNum*(i+1)+1].beta+trellis[encoder_stateNum*(i+1)+1].gamma1;
		inis1[1]=trellis[encoder_stateNum*i+1].alpha+trellis[encoder_stateNum*(i+1)+3].beta+trellis[encoder_stateNum*(i+1)+3].gamma1;
		inis1[2]=trellis[encoder_stateNum*i+2].alpha+trellis[encoder_stateNum*(i+1)+4].beta+trellis[encoder_stateNum*(i+1)+4].gamma1;
		inis1[3]=trellis[encoder_stateNum*i+3].alpha+trellis[encoder_stateNum*(i+1)+6].beta+trellis[encoder_stateNum*(i+1)+6].gamma1;
		inis1[4]=trellis[encoder_stateNum*i+4].alpha+trellis[encoder_stateNum*(i+1)+0].beta+trellis[encoder_stateNum*(i+1)+0].gamma2;
		inis1[5]=trellis[encoder_stateNum*i+5].alpha+trellis[encoder_stateNum*(i+1)+2].beta+trellis[encoder_stateNum*(i+1)+2].gamma2;
		inis1[6]=trellis[encoder_stateNum*i+6].alpha+trellis[encoder_stateNum*(i+1)+5].beta+trellis[encoder_stateNum*(i+1)+5].gamma2;
		inis1[7]=trellis[encoder_stateNum*i+7].alpha+trellis[encoder_stateNum*(i+1)+7].beta+trellis[encoder_stateNum*(i+1)+7].gamma2;

		inis0[0]=trellis[encoder_stateNum*i+0].alpha+trellis[encoder_stateNum*(i+1)+0].beta+trellis[encoder_stateNum*(i+1)+0].gamma1;
		inis0[1]=trellis[encoder_stateNum*i+1].alpha+trellis[encoder_stateNum*(i+1)+2].beta+trellis[encoder_stateNum*(i+1)+2].gamma1;
		inis0[2]=trellis[encoder_stateNum*i+2].alpha+trellis[encoder_stateNum*(i+1)+5].beta+trellis[encoder_stateNum*(i+1)+5].gamma1;
		inis0[3]=trellis[encoder_stateNum*i+3].alpha+trellis[encoder_stateNum*(i+1)+7].beta+trellis[encoder_stateNum*(i+1)+7].gamma1;
		inis0[4]=trellis[encoder_stateNum*i+4].alpha+trellis[encoder_stateNum*(i+1)+1].beta+trellis[encoder_stateNum*(i+1)+1].gamma2;
		inis0[5]=trellis[encoder_stateNum*i+5].alpha+trellis[encoder_stateNum*(i+1)+3].beta+trellis[encoder_stateNum*(i+1)+3].gamma2;
		inis0[6]=trellis[encoder_stateNum*i+6].alpha+trellis[encoder_stateNum*(i+1)+4].beta+trellis[encoder_stateNum*(i+1)+4].gamma2;
		inis0[7]=trellis[encoder_stateNum*i+7].alpha+trellis[encoder_stateNum*(i+1)+6].beta+trellis[encoder_stateNum*(i+1)+6].gamma2;

	 
	  
		//用于求iparity的软量
		inis1_par[0]=trellis[encoder_stateNum*i+0].alpha+trellis[encoder_stateNum*(i+1)+1].beta+trellis[encoder_stateNum*(i+1)+1].gamma1;
		inis1_par[1]=trellis[encoder_stateNum*i+1].alpha+trellis[encoder_stateNum*(i+1)+2].beta+trellis[encoder_stateNum*(i+1)+2].gamma1;
		inis1_par[2]=trellis[encoder_stateNum*i+2].alpha+trellis[encoder_stateNum*(i+1)+5].beta+trellis[encoder_stateNum*(i+1)+5].gamma1;
		inis1_par[3]=trellis[encoder_stateNum*i+3].alpha+trellis[encoder_stateNum*(i+1)+6].beta+trellis[encoder_stateNum*(i+1)+6].gamma1;
		inis1_par[4]=trellis[encoder_stateNum*i+4].alpha+trellis[encoder_stateNum*(i+1)+0].beta+trellis[encoder_stateNum*(i+1)+0].gamma2;
		inis1_par[5]=trellis[encoder_stateNum*i+5].alpha+trellis[encoder_stateNum*(i+1)+3].beta+trellis[encoder_stateNum*(i+1)+3].gamma2;
		inis1_par[6]=trellis[encoder_stateNum*i+6].alpha+trellis[encoder_stateNum*(i+1)+4].beta+trellis[encoder_stateNum*(i+1)+4].gamma2;
		inis1_par[7]=trellis[encoder_stateNum*i+7].alpha+trellis[encoder_stateNum*(i+1)+7].beta+trellis[encoder_stateNum*(i+1)+7].gamma2;

		inis0_par[0]=trellis[encoder_stateNum*i+0].alpha+trellis[encoder_stateNum*(i+1)+0].beta+trellis[encoder_stateNum*(i+1)+0].gamma1;
		inis0_par[1]=trellis[encoder_stateNum*i+1].alpha+trellis[encoder_stateNum*(i+1)+3].beta+trellis[encoder_stateNum*(i+1)+3].gamma1;
		inis0_par[2]=trellis[encoder_stateNum*i+2].alpha+trellis[encoder_stateNum*(i+1)+4].beta+trellis[encoder_stateNum*(i+1)+4].gamma1;
		inis0_par[3]=trellis[encoder_stateNum*i+3].alpha+trellis[encoder_stateNum*(i+1)+7].beta+trellis[encoder_stateNum*(i+1)+7].gamma1;
		inis0_par[4]=trellis[encoder_stateNum*i+4].alpha+trellis[encoder_stateNum*(i+1)+1].beta+trellis[encoder_stateNum*(i+1)+1].gamma2;
		inis0_par[5]=trellis[encoder_stateNum*i+5].alpha+trellis[encoder_stateNum*(i+1)+2].beta+trellis[encoder_stateNum*(i+1)+2].gamma2;
		inis0_par[6]=trellis[encoder_stateNum*i+6].alpha+trellis[encoder_stateNum*(i+1)+5].beta+trellis[encoder_stateNum*(i+1)+5].gamma2;
		inis0_par[7]=trellis[encoder_stateNum*i+7].alpha+trellis[encoder_stateNum*(i+1)+6].beta+trellis[encoder_stateNum*(i+1)+6].gamma2;
		
//	  max_inis1= max_n(inis1,8); 
//	  max_inis0= max_n(inis0,8);
	  max_inis1= max_n_star(inis1,8); 
	  max_inis0= max_n_star(inis0,8);

	  max_inis1_par=max_n(inis1_par,8); 
	  max_inis0_par=max_n(inis0_par,8); 
	  L_par[i]=max_inis0_par- max_inis1_par;
	 return max_inis0- max_inis1;
	
}

void memadd(double *out, double *in1, double *in2, int length, double factor)
{
	int	i;

	for(i=0;i<length;i++)
	{
		out[i] = in1[i]*factor + in2[i];
	}
}

void OSD_gauss_elimate_to_I_P (int *gen , int *index , int *out , double *input , int row, int col)
{
	int		i , j , k;
	int		reorder_flag=0, temp;
	int     index_index=0;
	double  LLR_tmp;
	int	    col_ID_index;
	int	    *temp_gen, *temp_index;
	int		*temp_out;
	double	*temp_input;
		
	if ((temp_gen = (int *) malloc(sizeof(int)*row*col)) == NULL)
	{
		printf("can not malloc \n");
	}	

	if ((temp_out = (int *) malloc(sizeof(int)*col)) == NULL)
	{
		printf("can not malloc \n");
	}	

	if ((temp_input = (double *) malloc(sizeof(double)*col)) == NULL)
	{
		printf("can not malloc \n");
	}	

	if ((temp_index = (int *) malloc(sizeof(int)*col)) == NULL)
	{
		printf("can not malloc \n");
	}	

	for(i=0;i<col;i++)	temp_index[i] = i;

	memset(temp_gen, 0, sizeof(int)*row*col);

	memset(index , -1 , sizeof(long)*col);

	col_ID_index = 0;//消第col_ID_index列对角位

    for(i=0;i<row;i++)
	{	
		if(gen[i*col+col_ID_index]==0)
    	{
			j=i+1;
       		while(j<row)
       		{
       			if(gen[j*col+col_ID_index]==1)
       			{
       				for(k=0;k<col;k++)
       				{
       					gen[i*col+k] = gen[i*col+k]^gen[j*col+k];
       				}
       				j=row;//一旦找到最近一行，退出while循环
       			}
       			j++;		//一直找到第col_ID_index列有1的那行与第i行相加。
       		}
       	}

		if(gen[i*col+col_ID_index]==0)       //???
		{
			reorder_flag = i;
			index[index_index] = i;
			index_index++;
			i = i - 1;
		}
		else
		//cancel the other position of (COLUMN-i) column '1'
		{
			for(j=(i+1);j<row;j++)
      		{
       			if(gen[j*col+col_ID_index]==1)
       			{
       				for(k=0;k<col;k++)
       				{
       					gen[j*col+k]
       					= gen[j*col+k] ^ gen[i*col+k];			//使除第i行以外其他行的第col_ID_index列为0
       				}
       			}
       		}
       	}		
		//cancel the other position of (COLUMN-i) column '1'

		col_ID_index++;
	}

	//if reorder_flag ,reorder column
	for(i=0;i<index_index;i++)		
	{
		temp = temp_index[index[i]];

		for(k=index[i]+1;k<col;k++)
		{
			temp_index[k-1] = temp_index[k];
		}
		temp_index[col-1] =  temp;
	}

	memcpy(temp_gen, gen, sizeof(int)*row*col);
	memcpy(temp_out, out, sizeof(int)*col);
	memcpy(temp_input, input, sizeof(double)*col);

	for(i=0;i<col;i++)
	{
		if(i!=temp_index[i])
		{
			for(j=0;j<row;j++)
			{
				temp_gen[j*col+i] = gen[j*col+temp_index[i]];
			}

			temp_out[i] = out[temp_index[i]];
			temp_input[i] = input[temp_index[i]];
		}
	}

	memcpy(gen, temp_gen, sizeof(int)*row*col);
	memcpy(out, temp_out, sizeof(int)*col);
	memcpy(input, temp_input, sizeof(double)*col);

	//if reorder_flag ,reorder column

	//construct [P|I]
	for(i=row-1;i>0;i--)
	{
		for(j=i-1;j>=0;j--)
    	{
    		if(gen[j*col+i]==1)
    		{
    			for(k=0;k<col;k++)
    			{
    				gen[j*col+k]
       	 			=gen[j*col+k] ^ gen[i*col+k];
       			}
       		}
       	}
	}
	//construct [P|I]

	free(temp_gen);
	free(temp_index);
	free(temp_out);
	free(temp_input);
}

void  reorder_G_by_sitep(double *input , int *gen , int *target , int *output , int *index1 , int *index2 , int row, int col)
{
	int i , j;
	int max_index , temp;
	double maximum;
	double *belief;
	
	if ((belief = (double *) malloc(sizeof(double)*col)) == NULL)
	{
		printf("can not malloc \n");
	}	

	memcpy(target, gen, sizeof(int)*row*col);

	for(i=0 ; i<col ; i++)
	{
		belief[i] = (double)fabs(input[i]);		
	}

	for(i=0 ; i<col ; i++)
	{
		maximum = belief[i];
		max_index = i;

		for(j=i+1 ; j<col ; j++)
		{
			if(belief[j]>maximum)
			{
				maximum = belief[j];
				max_index = j;				
			}
		}		

		index1[i] = max_index;		
		
		maximum = input[i];
		input[i] = input[max_index];
		input[max_index] = maximum;		//交换软量顺序

		maximum = belief[i];
		belief[i] = belief[max_index];
		belief[max_index] = maximum;		
		
		for(j=0;j<row;j++)
		{
			temp = target[j*col+i];//第j行
			target[j*col+i] = target[j*col+max_index];
			target[j*col+max_index] = temp;
		}		//交换G矩阵列顺序
	}

	OSD_gauss_elimate_to_I_P(target , index2 , output , input , row, col);
	

	for(i=0 ; i<col ; i++)
	{
		if(input[i]<0)
		{
			output[i] = 1;
		}
		else
		{
			output[i] = 0;			
		}
	}

	free(belief);
}

void LDPC_encode (int *gen , int *input , int *output , int row, int col)
{
	long i , j;
	long  COLUMN , G_ROW , H_ROW;
	long  Max_row_weight , Max_col_weight;

	memset(output, 0, sizeof(int)*col);

	for(i=0;i<col;i++)
	{
		for(j=0;j<row;j++)
		{
			output[i] = output[i]^(input[j]&gen[j*col+i]);	
		}

//		output[i] = output[i] - (output[i]^1);	//if 0 modulate -1 and 1modulate 1
		//output[i] = -output[i] + (output[i]^1);		//if 0 modulate 1 and 1modulate -1
	}	
}

void  restore_by_index_reorder(int *index1 , int *index2 , int *out , int length)
{
	int i , j;
	int temp;
	
	for(i=length-1;i>=0;i--)
	{
		if(index2[i]>0)
		{
			for(j=length-1;j>index2[i];j--)
			{
				temp = out[j];
				out[j] = out[j-1];
				out[j-1] = temp;
			}	
		}		
	}
	
	for(i=length-1;i>=0;i--)
	{
		temp = out[i];
		out[i] = out[index1[i]];
		out[index1[i]] = temp;
	}	
	
}

double  compute_min_distance(double *noise_in , int *test , double last_min , int *out , int length)
{
	int  i;
	double  temp =0;

	for(i=0;i<length;i++)
	{
		if(((noise_in[i]>0)&&(test[i]==1))||((noise_in[i]<=0)&&(test[i]==0)))
		{
			temp = temp + (double)fabs(noise_in[i]);
		}
	}

	if(temp<last_min)
	{
		for(i=0;i<length;i++)
		{
			out[i] = test[i];
		}
		return(temp);
	}
	else
	{
		return(last_min);
	}

}

void Flipping_encode(int *data , int *code , int length)
{
	int		i;

	for(i=0;i<length;i++)
	{
		if(data[i]==1)
		{
			code[i] = code[i]^1;
		}
	}
}

void OSD_process(int *oneDecodeOut , double	*sum_LLR , int * gen , int lenCB )
{
	int	i , j;
	int	*temp_gen_matrix, *temp_codeword, *near_optimal;
	int *infor_reorder, *index1_reorder, *index2_reorder;
	int CRC_result;

	int *origin_codeword;
	/*
	//test OSD time
	time_t	start  , end;
	start = clock();
	//test OSD time
	*/

	temp_gen_matrix = (int *)malloc(lenCB * 3 * lenCB * sizeof(int));
	if (temp_gen_matrix == NULL)
	{
		printf("Can not malloc temp_gen_matrix in turboDecode_CB!\n");
		getch();
		exit(0);
	}
	memset(temp_gen_matrix, 0, sizeof(int) * lenCB * 3 * lenCB);

	temp_codeword = (int *)malloc(3 * lenCB * sizeof(int));
	if (temp_codeword == NULL)
	{
		printf("Can not malloc temp_codeword in turboDecode_CB!\n");
		getch();
		exit(0);
	}
	memset(temp_codeword, 0, sizeof(int) * 3 * lenCB);

	origin_codeword = (int *)malloc(3 * lenCB * sizeof(int));
	if (origin_codeword == NULL)
	{
		printf("Can not malloc origin_codeword in turboDecode_CB!\n");
		getch();
		exit(0);
	}
	memset(origin_codeword, 0, sizeof(int) * 3 * lenCB);

	near_optimal = (int *)malloc(lenCB * sizeof(int));
	if (near_optimal == NULL)
	{
		printf("Can not malloc temp_codeword in turboDecode_CB!\n");
		getch();
		exit(0);
	}
	memcpy(near_optimal, oneDecodeOut, sizeof(int)*lenCB);

	if ((infor_reorder = (int *) malloc(sizeof(int)*3*lenCB)) == NULL)	printf("can not malloc \n");
	if ((index1_reorder = (int *) malloc(sizeof(int)*3*lenCB)) == NULL)	printf("can not malloc \n");
	if ((index2_reorder = (int *) malloc(sizeof(int)*3*lenCB)) == NULL)	printf("can not malloc \n");

	reorder_G_by_sitep(sum_LLR , gen , temp_gen_matrix , infor_reorder , index1_reorder , index2_reorder , lenCB, 3*lenCB);

	LDPC_encode(temp_gen_matrix , infor_reorder , temp_codeword , lenCB, 3*lenCB);
	
	memcpy(origin_codeword , temp_codeword , sizeof(int)*3*lenCB);

	restore_by_index_reorder(index1_reorder , index2_reorder , temp_codeword , 3*lenCB);
 
	memcpy(oneDecodeOut, temp_codeword, sizeof(int)*lenCB);
	CRC_result = CrcCheck(oneDecodeOut, lenCB, 24, 0);
	
	if(CRC_result==1)
	{
		memcpy(near_optimal, temp_codeword, sizeof(int)*lenCB);//0阶
	}
	else
	{
		for(i=0;i<lenCB;i++)
		{
			
			Flipping_encode(&temp_gen_matrix[i*3*lenCB] , origin_codeword , 3*lenCB);
			memcpy(temp_codeword, origin_codeword, sizeof(int)*3*lenCB);
			
			restore_by_index_reorder(index1_reorder , index2_reorder , temp_codeword , 3*lenCB);
		
			memcpy(oneDecodeOut, temp_codeword, sizeof(int)*lenCB);
			CRC_result = CrcCheck(oneDecodeOut, lenCB, 24, 0);
			if(CRC_result==1)
			{
				memcpy(near_optimal, temp_codeword, sizeof(int)*lenCB);
				i = lenCB;
				
			}
			else
			{				
				
				//infor_reorder[i] = infor_reorder[i]^1;
				Flipping_encode(&temp_gen_matrix[i*3*lenCB] , origin_codeword , 3*lenCB);
			}
		}

		//printf("\n");
	}
	
	memcpy(oneDecodeOut, near_optimal, sizeof(int)*lenCB);

	//for test OSD time
	
	/*
	if(CRC_result==0)
	{
		(*OSD_Succ_time) += end - start;
	}
	*/
	//for test OSD time

	free(origin_codeword);
	free(temp_gen_matrix);
	free(temp_codeword);
	free(near_optimal);
	free(infor_reorder);
	free(index1_reorder);
	free(index2_reorder);
}

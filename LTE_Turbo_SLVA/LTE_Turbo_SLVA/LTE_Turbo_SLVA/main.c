#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include "struct.h"
#include "define.h"
#include <string.h>
//#include "Simulation.h"
#include "channel.h"
#include "Turbo_Encode.h"
#include "Turbo_Decode.h"
#include "RateMatch.h"
#include "DeRateMatch.h"
#include "CRC_Encode.h"
#include "CRC_Decode.h"
#include "SLVA.h"
#include "Modulation.h"
	


void main(){
	//32 288;44,384;56,480;64,576;82,864;96,1152
	int f1=f1vec[55],f2=f2vec[55];
	int infoBitLen=kvec[55];
	int originBitLen=infoBitLen-24;
	int rateMatchLen= 960;
	int QM=1;//调制阶数
	int *infoBit;  
	int *originBit;
	int *systemBit;
	int *parityBit;
	int *iparityBit;
	int input_length=infoBitLen+3;//送入译码器的输入序列长度(译码器每端口序列长度)。
	double  *mod_symbol_real;
	double *mod_symbol_img;
	double *channel_out_real;
	double *channel_out_img;
	double *La_mod;
	double *LLR_mod;
	double *awgn_input;
	double *awgn_output;
	double *decoderIn_systemBit;
	double *decoderIn_afterInterleave_systemBit;
	double *decoderIn_parityBit;
	double *decoderIn_iparityBit;
	//struct AWGN *awgnchannel;
	struct StatisStruct *SS;
	double *L;//迭代译码后分量译码器2输出的最终似然比，用于硬判。
	double *exLLR;//所有信息比特校验比特软量。大小6144X3
	int *DecoderOutput;//译码输出。
	FILE *fp;
	char fileName[100];
    char fn_temp[100];
	/*定义循环变量*/
	int pn_i;
	int modu_j;
	int StoP_j;
	int sysQpp_i;
	int tailbit_i;
	int temp_i;
	int SS_i;
	int frame;
	//int innerIT;
	int i,j;
	int q1,q2;
	int interleaveAddr_sys;
	int g_sys;
	int tailBitAddr_awgn_output;
	int *rateMatchOut;
	int PN[11]= {1,0,1,0,0,0,1,1,0,0,1};									/*initial rand PN seed*/
	double *tailBit;
	int tailBitAddr;
	int R;//分组交织器行数。
	int C=32;//分组交织器列数，LTE中固定为32.
	int k0;//速率匹配时从循环缓冲器中读数据的起始点
	int rv;//冗余版本号
	int Ncb;
	double R_new;//打孔后实际码率。
	/*
	struct state trellis[1024+encoder_regNum+1][encoder_stateNum]={0};
	struct SerialListVA1 slva1[1024][slva_L]={0};
	double AbsoluteDiff[1024+encoder_regNum][slva_L]={0};//经过最优路径各状态的两条路径的累加度量差的绝对值(infoBitLen X L)
	int BestPathState[1024+encoder_regNum+1][slva_L]={0};//最优路径的状态序列((infoBitLen+1) X L)
	*/
	struct state *trellis;
/*
	struct SerialListVA1 *slva1;
	double *AbsoluteDiff;
	int *BestPathState;
	struct SerialListVA2 slva2[slva_L]={0};
	*/
    int	*pcm;
	int	*gen;
	int *codeword;
	int *s;
	pcm=(int *)malloc(((2*infoBitLen)*3*infoBitLen)*sizeof(int));
	if (pcm == NULL)
	{
		printf("Can not malloc pcm!\n");
		getch();
		exit(0);
	}
	gen=(int *)malloc(((infoBitLen)*3*infoBitLen)*sizeof(int));
	if (gen == NULL)
	{
		printf("Can not malloc gen!\n");
		getch();
		exit(0);
	}
	codeword=(int *)malloc((3*infoBitLen)*sizeof(int));
	if (codeword == NULL)
	{
		printf("Can not malloc codeword!\n");
		getch();
		exit(0);
	}
	s=(int *)malloc((2*infoBitLen)*sizeof(int));
	if (s == NULL)
	{
		printf("Can not malloc s!\n");
		getch();
		exit(0);
	}

	R_new=infoBitLen/(double)rateMatchLen;
	
	trellis=(struct state *)malloc(((maxInfoLength+encoder_regNum+1)*encoder_stateNum)*sizeof(struct state));
	if (trellis == NULL)
	{
		printf("Can not malloc trellis!\n");
		getch();
		exit(0);
	}
	/*
	slva1=(struct SerialListVA1 *)malloc((maxInfoLength*slva_L)*sizeof(struct SerialListVA1));
	if (slva1 == NULL)
	{
		printf("Can not malloc slva1!\n");
		getch();
		exit(0);
	}
	AbsoluteDiff=(double *)malloc(((maxInfoLength+encoder_regNum)*slva_L)*sizeof(double));
	if (AbsoluteDiff == NULL)
	{
		printf("Can not malloc AbsoluteDiff!\n");
		getch();
		exit(0);
	}
	BestPathState=(int *)malloc(((maxInfoLength+encoder_regNum+1)*slva_L)*sizeof(int));
	if (BestPathState == NULL)
	{
		printf("Can not malloc BestPathState!\n");
		getch();
		exit(0);
	}
	*/
	tailBit=(double *)malloc((12+1)*sizeof(double));
	if (tailBit == NULL)
	{
		printf("Can not malloc tailBit!\n");
		getch();
		exit(0);
	}
	L=(double *)malloc((maxInfoLength +1)*sizeof(double));
	if (L == NULL)
	{
		printf("Can not malloc L!\n");
		getch();
		exit(0);
	}
	exLLR=(double *)malloc((3*maxInfoLength +1)*sizeof(double));
	if (exLLR == NULL)
	{
		printf("Can not malloc exLLR!\n");
		getch();
		exit(0);
	}

	DecoderOutput=(int *)malloc((maxInfoLength +1)*sizeof(int));
	if (DecoderOutput == NULL)
	{
		printf("Can not malloc DecoderOutput!\n");
		getch();
		exit(0);
	}

	

	infoBit = (int *)malloc((maxInfoLength +1)*sizeof(int));
	if (infoBit == NULL)
	{
		printf("Can not malloc infoBit!\n");
		getch();
		exit(0);
	}
	originBit=(int *)malloc((maxInfoLength +1)*sizeof(int));
	if (originBit== NULL)
	{
		printf("Can not malloc originBit!\n");
		getch();
		exit(0);
	}
	systemBit=(int *)malloc((maxInfoLength +4+1)*sizeof(int));//多一个存编码后未传输的信号
	if (systemBit== NULL)
	{
		printf("Can not malloc systemBit!\n");
		getch();
		exit(0);
	}

	parityBit=(int *)malloc((maxInfoLength +4 +1)*sizeof(int));
	if (parityBit == NULL)
	{
		printf("Can not malloc parityBit!\n");
		getch();
		exit(0);
	}

	iparityBit=(int *)malloc((maxInfoLength +4+1)*sizeof(int));
	if (iparityBit == NULL)
	{
		printf("Can not malloc iparityBit!\n");
		getch();
		exit(0);
	}
	
	awgn_input=(double *)malloc((rateMatchLen+1)*sizeof(double));
	if (awgn_input == NULL)
	{
		printf("Can not malloc awgn_input!\n");
		getch();
		exit(0);
	}

	//awgn_output=(double *)malloc((3*input_length+3 +1)*sizeof(double));
	awgn_output=(double *)malloc((rateMatchLen+1)*sizeof(double));
	if (awgn_output == NULL)
	{
		printf("Can not malloc awgn_output!\n");
		getch();
		exit(0);
	}
	
	mod_symbol_real=(double *)malloc((rateMatchLen/QM +1)*sizeof(double));
	if (mod_symbol_real== NULL)
	{
		printf("Can not mallocmod_symbol_real!\n");
		getch();
		exit(0);
	}
	mod_symbol_img=(double *)malloc((rateMatchLen/QM +1)*sizeof(double));
	if (mod_symbol_img== NULL)
	{
		printf("Can not mallocmod_symbol_img!\n");
		getch();
		exit(0);
	}
	channel_out_real=(double *)malloc((rateMatchLen/QM +1)*sizeof(double));
	if (channel_out_real == NULL)
	{
		printf("Can not malloc channel_out_real!\n");
		getch();
		exit(0);
	}
	channel_out_img=(double *)malloc((rateMatchLen/QM +1)*sizeof(double));
	if (channel_out_img == NULL)
	{
		printf("Can not malloc channel_out_img!\n");
		getch();
		exit(0);
	}
	La_mod=(double *)malloc((rateMatchLen +1)*sizeof(double));
	if (La_mod == NULL)
	{
		printf("Can not malloc La_mod!\n");
		getch();
		exit(0);
	}
	LLR_mod=(double *)malloc((rateMatchLen +1)*sizeof(double));
	if (LLR_mod == NULL)
	{
		printf("Can not malloc LLR_mod!\n");
		getch();
		exit(0);
	}
	decoderIn_systemBit=(double *)malloc((maxInfoLength +4+1)*sizeof(double));
	if (decoderIn_systemBit == NULL)
	{
		printf("Can not malloc decoderIn_systemBit!\n");
		getch();
		exit(0);
	}

	decoderIn_afterInterleave_systemBit=(double *)malloc((maxInfoLength +4+1)*sizeof(double));
	if (decoderIn_systemBit == NULL)
	{
		printf("Can not malloc decoderIn_afterInterleave_systemBit!\n");
		getch();
		exit(0);
	}

	decoderIn_parityBit=(double *)malloc((maxInfoLength +4+1)*sizeof(double));
	if (decoderIn_systemBit == NULL)
	{
		printf("Can not malloc decoderIn_parityBit!\n");
		getch();
		exit(0);
	}

	decoderIn_iparityBit=(double *)malloc((maxInfoLength +4+1)*sizeof(double));
	if (decoderIn_systemBit == NULL)
	{
		printf("Can not malloc decoderIn_iparityBit!\n");
		getch();
		exit(0);
	}

	rateMatchOut=(int *)malloc((rateMatchLen +1)*sizeof(int));
	if (rateMatchOut == NULL)
	{
		printf("Can not malloc rateMatchOut!\n");
		getch();
		exit(0);
	}

	SS= (struct StatisStruct *)malloc(sizeof(struct StatisStruct));
	if (SS == NULL)
	{
		printf("Can not malloc SS!\n");
		getch();
		exit(0);
	}

	memset(infoBit,0,(maxInfoLength +1)*sizeof(int));
	memset(originBit,0,(maxInfoLength +1)*sizeof(int));
    memset(systemBit,0,(maxInfoLength +4 +1)*sizeof(int));
    memset(parityBit,0,(maxInfoLength +4+1)*sizeof(int));
    memset(iparityBit,0,(maxInfoLength +4+1)*sizeof(int));
    memset(decoderIn_systemBit,0,(maxInfoLength +4+1)*sizeof(double));
    memset(decoderIn_afterInterleave_systemBit,0,(maxInfoLength +4+1)*sizeof(double));
    memset(decoderIn_parityBit,0,(maxInfoLength +4+1)*sizeof(double));
    memset(decoderIn_iparityBit,0,(maxInfoLength +4+1)*sizeof(double));
    memset(L,0,(maxInfoLength+1)*sizeof(double));
    memset(exLLR,0,(3*maxInfoLength+1)*sizeof(double));
    memset(DecoderOutput,0,(maxInfoLength+1)*sizeof(int));
	memset(rateMatchOut,0,(rateMatchLen+1)*sizeof(int));
	memset(tailBit,0,13*sizeof(double));
	memset(mod_symbol_real,0,(rateMatchLen/QM +1)*sizeof(int));
	memset(mod_symbol_img,0,(rateMatchLen/QM +1)*sizeof(int));
	memset(channel_out_real,0,(rateMatchLen/QM +1)*sizeof(double));
	memset(channel_out_img,0,(rateMatchLen/QM +1)*sizeof(double));
	memset(La_mod,0,(rateMatchLen+1)*sizeof(double));
	memset(LLR_mod,0,(rateMatchLen+1)*sizeof(double));
	memset(trellis,0,((maxInfoLength+encoder_regNum+1)*encoder_stateNum)*sizeof(struct state));
/*	memset(slva1,0,(maxInfoLength*slva_L)*sizeof(struct SerialListVA1));
	memset(AbsoluteDiff,0,((maxInfoLength+encoder_regNum)*slva_L)*sizeof(double));
	memset(BestPathState,0,((maxInfoLength+encoder_regNum+1)*slva_L)*sizeof(int));*/
	memset(pcm,0,((2*infoBitLen)*3*infoBitLen)*sizeof(int));
	memset(gen,0,((infoBitLen)*3*infoBitLen)*sizeof(int));
    memset(codeword,0,(3*infoBitLen)*sizeof(int));
	memset(s,0,(2*infoBitLen)*sizeof(int));


	rv=0;
	R=(int)(infoBitLen+4+31)/32;
	Ncb=3*R*C;
	k0=R*(2*rv*((int)((Ncb+8*R-1)/(8*R)))+2);	

	//自动生成输出文件名
	strcpy(fileName, "K");	
	_itoa(infoBitLen, fn_temp, 10);
	strcat(fileName, fn_temp);
	strcat(fileName, "_RL");
	_itoa(rateMatchLen, fn_temp, 10);
	strcat(fileName, fn_temp);
	strcat(fileName, "_ITNum");
	_itoa(ITNum, fn_temp, 10);
	strcat(fileName, fn_temp);
	
	strcat(fileName, "_OSD_flag");
	_itoa(OSD_sum_flag, fn_temp, 10);
	strcat(fileName, fn_temp);
	strcat(fileName, ".txt");
	
 

	printf("K=%4d\tR=%4f\tITNum=%4d\tOSD_sum_flag=%4d\n\n",infoBitLen,R_new,ITNum,OSD_sum_flag);
	printf( "Eb/No\t%10s\t%4s\t%10s\t%10s\n","NTF","NEF","FER","BER");

	if((fp=fopen(fileName,"at"))==NULL){
			printf("Cannot open file strike any key exit");
			getch();
			exit(1);
		}
	fprintf(fp,"K=%4d\tR=%4f\tITNum=%4d\tOSD_sum_flag=%4d\n\n",infoBitLen,R_new,ITNum,OSD_sum_flag);
	fprintf(fp, "Eb/No\t%10s\t%4s\t%10s\t%10s\n","NTF","NEF","FER","BER");
	fclose(fp);
	
	for(SS->snr=SNRStart;SS->snr<SNRStop;SS->snr+=SNRStep){
		if((fp=fopen(fileName,"at"))==NULL){
			printf("Cannot open file strike any key exit");
			getch();
			exit(1);
		}
		SS->total_ErrorBit_num=0;
		SS->cur_ErrorBit_num=0;
		SS->frameError=0;
		SS->frame_num=0;
	//	SS->sigma2=(double) (1.0 /  (2.0 * R_new * pow(10.0,0.1*SS->snr)) );
		SS->sigma2=(1/R_new)/3.0;
		SS->initNoise.ix=2;
	    SS->initNoise.iy=2;
	    SS->initNoise.iz=2;
		for(temp_i=0;temp_i<11;temp_i++)
		{
			SS->initPN.reg[temp_i]=PN[temp_i];
		}
		

		for(frame=0;frame<max_FrameNum;frame++){
			SS->cur_ErrorBit_num=0;
			SS->frame_num++;
			for(temp_i=0;temp_i<originBitLen;temp_i++){
				originBit[temp_i] = (char)GenPN(&SS->initPN);
			//    originBit[temp_i]=0;
			}
			/*	for(temp_i=0;temp_i<infoBitLen;temp_i++){
					//originBit[temp_i] = (char)GenPN(&SS->initPN);
				infoBit[temp_i]=0;
				}*/
			//add CRC
			CRCEncode(infoBit, originBit, originBitLen, 24, 0);
			/*	for(temp_i=0;temp_i<infoBitLen;temp_i++){
				 printf("infoBit=%d\t",infoBit[temp_i]);
			 }
			printf("\n");*/

			//	printf("rateMatchLen=%d\tR=%d\n",rateMatchLen,R);
			//Encoding
			TurboEncode(infoBitLen,infoBit,systemBit,parityBit,iparityBit,f1,f2);
			/*
			//construct codeword to check H
			for(i=0;i<infoBitLen;i++){
				codeword[i]=systemBit[i];
				codeword[infoBitLen+i]=parityBit[i];
				codeword[2*infoBitLen+i]=iparityBit[i];
			}*/
			//calculate H,G
			set_Matrix (pcm , gen , 3*infoBitLen,2*infoBitLen, infoBitLen);
			//ratematching
			TurboInterleave(rateMatchOut, rateMatchLen, infoBitLen, 0,f1, f2, 
							0, systemBit, parityBit,iparityBit, R, k0);
			//modulation
			if(QM==1){
				/*BPSK modulation*/
				for(modu_j=0;modu_j<rateMatchLen;modu_j++){
					awgn_input[modu_j]=1-2*rateMatchOut[modu_j];
				}
			}
			else{
				/*QPSK,16QAM,64QAM*/
				Modulation(QM, mod_symbol_real, mod_symbol_img, rateMatchOut ,rateMatchLen);
			}
	
			//add noise
			if(QM==1){
				AWGNChannel(SS->snr, rateMatchLen, R_new, awgn_input, awgn_output, &SS->initNoise);
			}
			else{
				AWGNChannel(SS->snr,rateMatchLen/QM,QM*R_new,mod_symbol_real,channel_out_real,&SS->initNoise);
				AWGNChannel(SS->snr,rateMatchLen/QM,QM*R_new,mod_symbol_img,channel_out_img,&SS->initNoise);
			}
	
			if(QM>1){
				//demodulation
				Demodulation(0, 1,rateMatchLen,QM, LLR_mod, channel_out_real, channel_out_img, La_mod, SS->snr);
				//deratematching
				TurboDeinterleave(LLR_mod, decoderIn_systemBit, decoderIn_parityBit, decoderIn_iparityBit,
							   infoBitLen+4, k0, R, rateMatchLen, 0, 0);
			}
			else{
				TurboDeinterleave(awgn_output, decoderIn_systemBit, decoderIn_parityBit, decoderIn_iparityBit,
							   infoBitLen+4, k0, R, rateMatchLen, 0, 0);

			}

			//对decoderIn_systemBit进行交织得decoderIn_afterInterleave_systemBit
			interleaveAddr_sys= 0;	//Turbo_Interleave address
			g_sys = f1 + f2;
			for (sysQpp_i=0; sysQpp_i<infoBitLen; sysQpp_i++)
			{
				decoderIn_afterInterleave_systemBit[sysQpp_i]=decoderIn_systemBit[interleaveAddr_sys];
				//compute next Turbo_Interleave address
				interleaveAddr_sys = (g_sys + interleaveAddr_sys)%infoBitLen;
				g_sys = (g_sys + 2*f2)%infoBitLen;
			}
			//收集尾比特
			//tailBitAddr=infoBitLen;
			for(tailbit_i=0;tailbit_i<4;tailbit_i++){
				tailBit[3*tailbit_i]=decoderIn_systemBit[infoBitLen+tailbit_i];
				tailBit[3*tailbit_i+1]=decoderIn_parityBit[infoBitLen+tailbit_i];
				tailBit[3*tailbit_i+2]=decoderIn_iparityBit[infoBitLen+tailbit_i];
		
			}
			//把尾比特还原给4路信号decoderIn_systemBit、decoderIn_parityBit、decoderIn_afterInterleave_systemBit、decoderIn_iparityBit。
			tailBitAddr=0;
			for(tailbit_i=0;tailbit_i<3;tailbit_i++){
				decoderIn_systemBit[infoBitLen+tailbit_i]=tailBit[tailBitAddr];
				tailBitAddr++;
				decoderIn_parityBit[infoBitLen+tailbit_i]=tailBit[tailBitAddr];
				tailBitAddr++;
			}
			tailBitAddr=6;
			for(tailbit_i=0;tailbit_i<3;tailbit_i++){
				decoderIn_afterInterleave_systemBit[infoBitLen+tailbit_i]=tailBit[tailBitAddr];
				tailBitAddr++;
				decoderIn_iparityBit[infoBitLen+tailbit_i]=tailBit[tailBitAddr];
				tailBitAddr++;
			}

			//Turbo decode
			TurboDecode_SLVA(infoBitLen,input_length,gen,decoderIn_systemBit,decoderIn_parityBit,
				decoderIn_afterInterleave_systemBit,decoderIn_iparityBit,f1,f2, L,exLLR,DecoderOutput,trellis,SS->sigma2);


			/*
			for(temp_i=0;temp_i<infoBitLen;temp_i++){
				 printf("DecoderOutput=%d\t",DecoderOutput[temp_i]);
			 }
			printf("\n");*/
			//错误统计
			for(SS_i=0;SS_i<infoBitLen;SS_i++){
				if(DecoderOutput[SS_i]!=infoBit[SS_i])  SS->cur_ErrorBit_num++;
			}
			SS->total_ErrorBit_num+=SS->cur_ErrorBit_num;
			 if(CrcCheck(DecoderOutput, infoBitLen, 24, 0)==0) SS->frameError++;
 				/*display the simulation*/
			 if(SS->frame_num%DisplayStep==0){
				 SS->FER=SS->frameError/((double) SS->frame_num);
				 SS->BER=SS->total_ErrorBit_num/(double)infoBitLen/(double)(SS->frame_num);
				 printf("%.2f\t%10d\t%4d\t%5.3e\t%5.3e\r",SS->snr,SS->frame_num,SS->frameError,SS->FER,SS->BER);
			 }
			if(SS->frame_num>leastFrameNum&&SS->frameError>=leastFrameErrorNum) break;

		}

		SS->FER=SS->frameError/((double) SS->frame_num);
		SS->BER=SS->total_ErrorBit_num/(double)infoBitLen/(double)(SS->frame_num);
		//printf("snr=%f\tFER=%f\n",SS->snr,SS->FER);
		printf("%.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->frame_num,SS->frameError,SS->FER,SS->BER);
		fprintf(fp,"%.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->frame_num,SS->frameError,SS->FER,SS->BER);
        fclose(fp);
	}
	if((fp=fopen(fileName,"at"))==NULL){
			printf("Cannot open file strike any key exit");
			getch();
			exit(1);
		}
	fprintf(fp,"\n");
    fclose(fp);
	//rewind(fp);

	free(infoBit);
	infoBit=NULL;
	free(originBit);
	originBit=NULL;
	free(systemBit);
	systemBit=NULL;
	free(parityBit);
	parityBit=NULL;
	free(iparityBit);
	iparityBit=NULL;
	free(awgn_input);
	awgn_input=NULL;
	free(awgn_output);
	awgn_output=NULL;
	free(decoderIn_systemBit);
	decoderIn_systemBit=NULL;
	free(decoderIn_afterInterleave_systemBit);
	decoderIn_afterInterleave_systemBit=NULL;
	free(decoderIn_parityBit);
	decoderIn_parityBit=NULL;
	free(decoderIn_iparityBit);
	decoderIn_iparityBit=NULL;
	free(SS);
	SS=NULL;
	free(L);
	L=NULL;
	free(DecoderOutput);
	DecoderOutput=NULL;
	free(rateMatchOut);
	rateMatchOut=NULL;
	free(tailBit);
	tailBit=NULL;
}

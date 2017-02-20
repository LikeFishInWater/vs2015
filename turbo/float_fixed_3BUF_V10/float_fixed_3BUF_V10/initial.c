/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "initial.h"

/*
********************************************************************************
* NAME:		GetParameter
* PURPOSE:	从初始化文件中获取仿真所需参数，具体的参数含义见Init_file.txt的说明部分

*Output:
TC:			规定Turbo码本身的一些参数的结构体，包括编码、速率匹配、调制、译码等
SP:			规定仿真中赋予的参数的结构体
lr:			规定输入译码器的LLR值定点化参数的结构体
others:		规定译码器内部alpha、beta、Le_sys、Le_par定点化字长的结构体
*
* AUTHOR:	Duck
********************************************************************************
*/
void GetParameter(TurboCode *TC, SPStruct *SP, LR *lr, OTHERS *others)
{
	FILE					*fp_cfg = NULL;							/*file pointer to the profile*/

	if(NULL == (fp_cfg=fopen("Init_file.txt","r")))
	{
		printf("can not open file: Profile.txt\n");
		getchar();
		exit(0);
	}

	TC->crcLen = 24;

	//basic parameters
	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->msgLen);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->rateMatchLen);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->maxIterNum);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->maxbigIT);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->decoder_Type);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->decMethod);		

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->decNum);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%lf", &TC->normFactor_float);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->normFactor_fixed);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->SW_Size);


	//lte parameters
	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->NL);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->Qm);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->NIR);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &TC->rvIndex);


	//simulation parameter
	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%lf", &SP->startSnr);
	fscanf(fp_cfg, "%lf", &SP->stepSnr);
	fscanf(fp_cfg, "%lf", &SP->stopSnr);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->leastErrorFrame);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->leastTestFrame);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->dispStep);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->writeFreq);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->noiseFul);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->decMode);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->recordOn);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &SP->testOn);

	//fixed parameter
	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &lr->fixed_method);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &lr->width);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%lf", &lr->temp_max);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%lf", &lr->temp_power);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &others->width_alpha);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &others->width_beta);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &others->width_le);

	GetCfg(fp_cfg);
	fscanf(fp_cfg, "%d", &others->width_le_demo);

	fclose(fp_cfg);


}

/*
********************************************************************************
* NAME:		WriteLogo
* PURPOSE:	在屏幕上显示仿真中所用的一些参数，并且将这些参数存入放译码结果的文件中

*Input:
TC:			规定Turbo码本身的一些参数的结构体，包括编码、速率匹配、调制、译码等
SS:			统计所得的FER和BER等参数
SP:			规定仿真中赋予的参数的结构体
lr:			规定输入译码器的LLR值定点化参数的结构体
others:		规定译码器内部alpha、beta、Le_sys、Le_par定点化字长的结构体
*
* AUTHOR:	Duck
********************************************************************************
*/
void WriteLogo(TurboCode *TC, StatisStruct *SS, SPStruct *SP, LR *lr, OTHERS *others)
{
	FILE					*fp_result = NULL;							/*temp file pointer to result*/
	char					temp[100];

	//decide output performance file name
	switch (TC->decMethod)
	{
	case 0: 		strcpy(SP->performance, "Max");		break;
	case 1: 		strcpy(SP->performance, "Max+");		break;
	case 2: 		strcpy(SP->performance, "Log");		break;
	default: printf("error decode method number,quit\n");_getch();exit(0);break;
	}
	_itoa(TC->msgLen, temp, 10);
	strcat(SP->performance, "_L");
	strcat(SP->performance, temp);
	_itoa(TC->rateMatchLen, temp, 10);
	strcat(SP->performance, "_R");
	strcat(SP->performance, temp);
	switch (TC->decMethod)
	{
	case 0: 			
		break;
	case 1: 				
		break;
	case 2: 				
		break;
	default: printf("error decode method number,quit\n");_getch();exit(0);break;
	}

	strcat(SP->performance, ".txt");

	//compute rate and info length
	TC->infoLen = TC->msgLen+TC->crcLen;
	TC->rate = (double)TC->infoLen/(double)TC->rateMatchLen;

	/*print simulation parameters on screen*/
	printf("*************************LTE Turbo code Simulation*************************\n");
	printf("* Message Length is %d, CRC Length is %d, Code is LTE (15,13)\n",TC->msgLen, TC->crcLen);
	printf("* Transfer Length is %d, Rate is %1.3f\n",TC->rateMatchLen, TC->rate);
	printf("* Decoder Number is %d\n",TC->decNum);
	
	switch (TC->decMethod)
	{
	case 0: 		printf("* Decoding is Max-Log-MAP,  IT = %d\t",TC->maxIterNum);printf("bigIT = %d\n",(TC->maxbigIT -1));		break;
	case 1: 		printf("* Decoding is Log-MAP,  IT = %d\n",TC->maxIterNum);printf("bigIT = %d\n",(TC->maxbigIT -1));		break;
	case 2: 		printf("* Decoding is Max-Log-MAP+,  IT = %d\n",TC->maxIterNum);printf("bigIT = %d\n",(TC->maxbigIT -1));		break;
	default:		printf("error decode method number,quit\n");_getch();exit(0);break;
	}
	switch (TC->decoder_Type)
	{
	case 0: 		printf("* Float decoding is based on slide windows with Subblock,Parallelism in each parallel subdecoder is 3, B=%d\n", TC->SW_Size);		break;
	case 1: 		printf("* Fixed decoding is based on slide windows with Subblock,Parallelism in each parallel subdecoder is 3, B=%d\n", TC->SW_Size);		break;
	default:		printf("error decoder_Type,quit\n");_getch();exit(0);break;
	}
	switch (TC->decoder_Type)
	{
	case 0: 		printf("* Const Factor for float decoder is %f\n",TC->normFactor_float);		break;
	case 1: 		printf("* Const Factor for fixed decoder is %d\n",TC->normFactor_fixed);		break;
	default:		printf("error decoder_Type,quit\n");_getch();exit(0);break;
	}

	printf("* Information bits is PN sequence, Channel is AWGN\n");
	if (SP->noiseFul)
	{
		printf("* Add Noise : yes. \n");
	}
	else
	{
		printf("* Add Noise : no. \n");
	}
	switch(TC->Qm)
	{
	case 1:		printf("* Modulation type : BPSK. \n");		break;
	case 2:		printf("* Modulation type : QPSK. \n");		break;
	case 4:		printf("* Modulation type : 16QAM. \n");	break;
	case 6:		printf("* Modulation type : 64QAM. \n");	break;
	default:	printf("error modulation type,quit\n");_getch();exit(0);break;
	}

	switch(lr->fixed_method)
	{
	case 0:			printf("* Fixed uses the max value,  max_temp = %f\n",lr->temp_max);		break;
	case 1:			printf("* Fixed uses the average power,  power_temp = %f\n",lr->temp_power);		break;
	default:		printf("error fixed_method,quit\n");_getch();exit(0);break;
	}
	printf("* The width of fixed LLR = %d\n",lr->width);
	printf("* The width of fixed alpha = %d\n",others->width_alpha);
	printf("* The width of fixed beta = %d\n",others->width_beta);
	printf("* The width of fixed Le_sys = %d\n",others->width_le);
	printf("* The width of fixed Le_par = %d\n",others->width_le_demo);

	printf("* Output Performance file : %s\n",SP->performance);

	printf("* The Eb/N0 is (%.2f, %.2f, %.2f)\n",SS->initStartSnr,SP->stepSnr,SP->stopSnr);
	printf("***************************************************************************\n\n\n");
	printf(" Eb/No\t%10s\t%4s\t%10s\t%10s\n","NTF","NEF","FER","BER");

	/*save the result to file*/
	if(NULL == (fp_result=fopen(SP->performance,"a")))
	{
		printf("\ncan not open file: Performance.txt");
		getchar();
		exit(0);
	}

	fprintf(fp_result, "*************************LTE Turbo code Simulation*************************\n");
	fprintf(fp_result, "* Message Length is %d, CRC Length is %d, Code is LTE (15,13)\n",TC->msgLen, TC->crcLen);
	fprintf(fp_result, "* Transfer Length is %d, Rate is %1.3f\n",TC->rateMatchLen, TC->rate);
	fprintf(fp_result, "* Decoder Number is %d\n",TC->decNum);
	
	switch (TC->decMethod)
	{
	case 0: 		fprintf(fp_result, "* Decoding is Max-Log-MAP,  IT = %d\t",TC->maxIterNum);fprintf(fp_result,"bigIT = %d\n",(TC->maxbigIT -1));break;
	case 1: 		fprintf(fp_result, "* Decoding is Log-MAP,  IT = %d\t",TC->maxIterNum);fprintf(fp_result,"bigIT = %d\n",(TC->maxbigIT -1));break;
	case 2: 		fprintf(fp_result, "* Decoding is Max-Log-MAP+,  IT = %d\t",TC->maxIterNum);fprintf(fp_result,"bigIT = %d\n",(TC->maxbigIT -1));break;
	default:		fprintf(fp_result, "error decode method number,quit\n");_getch();exit(0);break;
	}
	switch (TC->decoder_Type)
	{
	case 0: 		fprintf(fp_result, "* Float decoding is based on slide windows with Subblock,Parallelism in each parallel subdecoder is 3, B=%d\n", TC->SW_Size);		break;
	case 1: 		fprintf(fp_result, "* Fixed decoding is based on slide windows with Subblock,Parallelism in each parallel subdecoder is 3, B=%d\n", TC->SW_Size);		break;
	default:		fprintf(fp_result, "error radix type,quit\n");_getch();exit(0);break;
	}
	switch (TC->decoder_Type)
	{
	case 0: 		fprintf(fp_result, "* Const Factor for float decoder is %f\n",TC->normFactor_float);		break;
	case 1: 		fprintf(fp_result, "* Const Factor for fixed decoder is %d\n",TC->normFactor_fixed);		break;
	default:		printf("error decoder_Type,quit\n");_getch();exit(0);break;
	}

	fprintf(fp_result, "* Information bits is PN sequence, Channel is AWGN\n");
	if (SP->noiseFul)
	{
		fprintf(fp_result, "* Add Noise : yes. \n");
	}
	else
	{
		fprintf(fp_result, "* Add Noise : no. \n");
	}
	switch(TC->Qm)
	{
	case 1:		fprintf(fp_result,"* Modulation type : BPSK. \n");		break;
	case 2:		fprintf(fp_result,"* Modulation type : QPSK. \n");		break;
	case 4:		fprintf(fp_result,"* Modulation type : 16QAM. \n");	break;
	case 6:		fprintf(fp_result,"* Modulation type : 64QAM. \n");	break;
	default:	fprintf(fp_result,"error modulation type,quit\n");_getch();exit(0);break;
	}
	fprintf(fp_result, "* Output Performance file : %s\n",SP->performance);

	fprintf(fp_result, "* The Eb/N0 is (%.2f, %.2f, %.2f)\n",SS->initStartSnr,SP->stepSnr,SP->stopSnr);
	fprintf(fp_result, "***************************************************************************\n\n\n");
	fprintf(fp_result, " Eb/No\t%10s\t%4s\t%10s\t%10s\n","NTF","NEF","FER","BER");

	switch(lr->fixed_method)
	{
	case 0:			fprintf(fp_result,"* Fixed uses the max value,  max_temp = %f\n",lr->temp_max);		break;
	case 1:			fprintf(fp_result,"* Fixed uses the average power,  power_temp = %f\n",lr->temp_power);		break;
	default:		fprintf(fp_result,"error fixed_method,quit\n");_getch();exit(0);break;
	}
	fprintf(fp_result,"* The width of fixed LLR = %d\n",lr->width);
	fprintf(fp_result,"* The width of fixed alpha = %d\n",others->width_alpha);
	fprintf(fp_result,"* The width of fixed beta = %d\n",others->width_beta);
	fprintf(fp_result,"* The width of fixed Le_sys = %d\n",others->width_le);
	fprintf(fp_result,"* The width of fixed Le_par = %d\n",others->width_le_demo);

	fprintf(fp_result, "* Output Performance file : %s\n",SP->performance);

	fprintf(fp_result, "* The Eb/N0 is (%.2f, %.2f, %.2f)\n",SS->initStartSnr,SP->stepSnr,SP->stopSnr);
	fprintf(fp_result, "***************************************************************************\n\n\n");
	fprintf(fp_result, " Eb/No\t%10s\t%4s\t%10s\t%10s\n","NTF","NEF","FER","BER");

	fclose(fp_result);


}

/*
********************************************************************************
* NAME:		InitSimPara
* PURPOSE:	选择译码模式：0->a new decoding test；1->continue last decoding test

*Input:
SS:			统计所得的FER和BER等参数
SP:			规定仿真中赋予的参数的结构体
*
* AUTHOR:	Duck
********************************************************************************
*/
void InitSimPara(StatisStruct *SS, SPStruct *SP)
{
	FILE					*fp_temp = NULL;							/*temp file pointer to result*/
	int					i;
	RandomSeed		noise = {1,10,100};					/*random seed for awgn channel*/
	PNSeed			PN = {{1,0,1,0,0,0,1,1,0,0,1}};		/*initial PN Sequence*/

	//decoding mode is a new one
	if (!SP->decMode)
	{
		SS->initStartSnr = SP->startSnr;
		SS->initSimIndex = 1;
		SS->initFrameError = 0;
		SS->initBitError = 0;
		SS->initPN = PN;
		SS->initNoise = noise;	
	}
	else//continue last decoding test, read parameters from temp file
	{
		if(NULL == (fp_temp=fopen("Temp.txt","r")))
		{
			printf("can not open file: Temp.txt");
			getchar();
			exit(0);
		}

		GetCfg(fp_temp);
		fscanf(fp_temp, "%lf" , &SS->initStartSnr);
		GetCfg(fp_temp);
		fscanf(fp_temp, "%d" , &SS->initSimIndex);
		SS->initSimIndex++;//next frame

		GetCfg(fp_temp);
		fscanf(fp_temp, "%d" , &SS->initFrameError);
		GetCfg(fp_temp);
		fscanf(fp_temp, "%d" , &SS->initBitError);

		GetCfg(fp_temp);
		for (i=0; i<11; i++)
		{
			fscanf(fp_temp, "%d" , &SS->initPN.reg[i]);
		}

		GetCfg(fp_temp);
		fscanf(fp_temp, "%d" , &SS->initNoise.ix);
		fscanf(fp_temp, "%d" , &SS->initNoise.iy);
		fscanf(fp_temp, "%d" , &SS->initNoise.iz);

		fclose(fp_temp);
	}


}

/*
********************************************************************************
* NAME:		GetCfg
* PURPOSE:	为读取所需的参数而准备

*Input:
fp:			读取参数的文件
*
* AUTHOR:	Duck
********************************************************************************
*/
void GetCfg(FILE *fp)
{
	char ch;

	do 
	{
		fscanf(fp,"%c",&ch);
	} while (ch != ':');

}
/*
********************************************************************************
* NAME:		GenPN
* PURPOSE:	利用PN序列产生所需传输的信息序列

*Input:
initPN:		初始化PN序列发生器
*
* AUTHOR:	Duck
********************************************************************************
*/
int GenPN(PNSeed *initPN)
{
	int result = 0;
	int i = 0;

	/*shift the shift register*/
	for(i=10; i>=1; i--)
	{
		initPN->reg[i] = initPN->reg[i-1];
	}

	/*calculate the output*/
	initPN->reg[0] = initPN->reg[10] ^ initPN->reg[3];
	result = initPN->reg[10];

	/*output the result*/
	return result;

}

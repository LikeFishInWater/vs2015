/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include "define.h"
#include "simulation.h"
#include "fixed_function.h"

/*
********************************************************************************
* NAME:			Test
* PURPOSE:		对每个snr进行仿真

* Input:
snr:			比特信噪比
TC:				规定Turbo码本身的一些参数的结构体，包括编码、速率匹配、调制、译码等
SS:				统计所得的FER和BER等参数
SP:			规定仿真中赋予的参数的结构体
lr:				规定输入译码器的LLR值定点化参数的结构体
others:			规定译码器内部alpha、beta、Le_sys、Le_par定点化字长的结构体
*
* AUTHOR:		Duck
********************************************************************************
*/
void Test(double snr, TurboCode *TC, StatisStruct *SS, SPStruct *SP, LR *lr, OTHERS *others)
{
	int		*originBit;
	int		*rateMatchOut;
	double	*channelIn;
	double	*channelOut;
	int		*decodeOut;
	double	*mod_symbols_rel;
	double	*mod_symbols_img;
	double	*out_real_channel;
	double	*out_img_channel;
	double	*La;
	int		*La_fix;
	int		*channelOut_fix;

	RandomSeed		noise = {1,10,100};											/*initial rand noise seed*/
	PNSeed			PN = {{1,0,1,0,0,0,1,1,0,0,1}};									/*initial rand PN seed*/

	int goon;
	int crcResult;																					//crc check result
	int i = 0;
	int bigIT_temp=0;
	FILE *fpTest = NULL;
	FILE *fpDebug = NULL;

	int index=0;
	int decoder_Type = TC->decoder_Type;

	
	originBit = (int *)malloc((maxInfoLength-24 +1)*sizeof(int));
	if (originBit == NULL)
	{
		printf("Can not malloc originBit Struct!\n");
		getch();
		exit(0);
	}
	
	rateMatchOut = (int *)malloc((3*maxInfoLength+12 +1)*sizeof(int));
	if (rateMatchOut == NULL)
	{
		printf("Can not malloc rateMatchOut Struct!\n");
		getch();
		exit(0);
	}
	channelIn = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (channelIn == NULL)
	{
		printf("Can not malloc channelIn Struct!\n");
		getch();
		exit(0);
	}
	channelOut = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (channelOut == NULL)
	{
		printf("Can not malloc channelOut Struct!\n");
		getch();
		exit(0);
	}
	decodeOut = (int *)malloc((maxInfoLength +1)*sizeof(int));
	if (decodeOut == NULL)
	{
		printf("Can not malloc decodeOut Struct!\n");
		getch();
		exit(0);
	}
	mod_symbols_rel = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (mod_symbols_rel == NULL)
	{
		printf("Can not malloc mod_symbols_rel Struct!\n");
		getch();
		exit(0);
	}
	mod_symbols_img = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (mod_symbols_img == NULL)
	{
		printf("Can not malloc mod_symbols_img Struct!\n");
		getch();
		exit(0);
	}
	out_real_channel = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (out_real_channel == NULL)
	{
		printf("Can not malloc out_real_channel Struct!\n");
		getch();
		exit(0);
	}
	out_img_channel = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (out_img_channel == NULL)
	{
		printf("Can not malloc out_img_channel Struct!\n");
		getch();
		exit(0);
	}
	La = (double *)malloc((3*maxInfoLength+12 +1)*sizeof(double));
	if (La == NULL)
	{
		printf("Can not malloc La Struct!\n");
		getch();
		exit(0);
	}
		La_fix = (int *)malloc((3*maxInfoLength+12 +1)*sizeof(int));
	if (La_fix == NULL)
	{
		printf("Can not malloc La_fix Struct!\n");
		getch();
		exit(0);
	}
	channelOut_fix = (int *)malloc((3*maxInfoLength+12 +1)*sizeof(int));
	if (channelOut_fix == NULL)
	{
		printf("Can not malloc channelOut_fix Struct!\n");
		getch();
		exit(0);
	}

	memset(originBit,0,(maxInfoLength-24 +1)*sizeof(int));
	memset(rateMatchOut,0,(3*maxInfoLength+12 +1)*sizeof(int));
	memset(channelIn,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(channelOut,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(decodeOut,0,(maxInfoLength +1)*sizeof(int));
	memset(mod_symbols_rel,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(mod_symbols_img,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(out_real_channel,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(out_img_channel,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(La,0,(3*maxInfoLength+12 +1)*sizeof(double));
	memset(La_fix,0,(3*maxInfoLength+12 +1)*sizeof(int));
	memset(channelOut_fix,0,(3*maxInfoLength+12 +1)*sizeof(int));

	TC->sigma2 = (double) (1.0 / sqrt( 2.0  * TC->Qm * TC->rate * pow(10.0,0.1*snr) ));//TC->Qm is the modulation type
	TC->sigma2 = TC->sigma2*TC->sigma2;

	if (SS->snr != SS->initStartSnr || !SP->decMode)					//reset statistics and rand seed
	{
		SS->simIndex = 1;
		SS->frameError = 0;
		SS->bitError = 0;
		SS->initNoise = noise;					
		SS->initPN = PN;							
	}

	if (SP->testOn)
	{
		if(NULL == (fpTest=fopen("ErrorFrame.txt","r")))
		{
			printf("can not open file: ErrorFrame.txt");
			getchar();
			exit(0);
		}

		/*get  PN seed*/
		GetCfg(fpTest);
		for (i=0; i<11; i++)
		{
			fscanf(fpTest,"%d ",&SS->initPN.reg[i]);
		}

		/*get noise seed*/
		GetCfg(fpTest);
		fscanf(fpTest, "%d" , &SS->initNoise.ix);
		fscanf(fpTest, "%d" , &SS->initNoise.iy);
		fscanf(fpTest, "%d" , &SS->initNoise.iz);
		fclose(fpTest);
	}

	while(1)
	//for(index=0;index<SP->leastTestFrame;index++)
	{
		/*backup current PN and Noise Seed*/
		SS->lastNoise = SS->initNoise;
		SS->lastPN = SS->initPN;

		/*generate message bits by PN sequence*/
		for(i=0;i<TC->msgLen;i++)
		{
			originBit[i] = (char)GenPN(&SS->initPN);
		}

		/*turbo encode , include : segment, add crc, encode, interleave and rate match*/
		LTE_Turbo_Encode(rateMatchOut, originBit, TC->rateMatchLen, TC->msgLen, TC->NL, TC->Qm, TC->NIR, TC->rvIndex);

		/*modulation:BPSK;QPSK;16QAM;64QAM*/
		if(TC->Qm == 1){
			/*BPSK modulation*/
			for(i=0; i<TC->rateMatchLen; i++)
			{
			channelIn[i] = 1.0 - 2.0*rateMatchOut[i];
			}
		}
		else{
			/*QPSK,16QAM,64QAM*/
			Modulation(TC->Qm, mod_symbols_rel, mod_symbols_img, rateMatchOut,TC->rateMatchLen);
		}

		/*add noise*/
		if(SP->noiseFul)
		{
			if(TC->Qm == 1){
				AWGNChannel(snr, TC->rateMatchLen, TC->rate, channelIn, channelOut, &SS->initNoise);
			}
			else{			
				AWGNChannel(snr, (TC->rateMatchLen)/TC->Qm, TC->Qm*TC->rate, mod_symbols_rel, out_real_channel, &SS->initNoise);
				AWGNChannel(snr, (TC->rateMatchLen)/TC->Qm, TC->Qm*TC->rate, mod_symbols_img, out_img_channel, &SS->initNoise);
			}
		}
		else
		{
			if(TC->Qm == 1){
				memcpy(channelOut, channelIn, sizeof(double)*TC->rateMatchLen);
			}
			else{
				memcpy(out_real_channel, mod_symbols_rel, sizeof(double)*(TC->rateMatchLen)/TC->Qm);
				memcpy(out_img_channel, mod_symbols_img, sizeof(double)*(TC->rateMatchLen)/TC->Qm);
			}
		}

		/*BICM:BPSK and QPSK;BICM-ID:16QAM and 64QAM*/
		bigIT_temp = 0;
		while(bigIT_temp<TC->maxbigIT){
			bigIT_temp++;

			/*demodulation if is not BPSK*/
			if((TC->Qm) > 1){
				Demodulation(bigIT_temp-1, 1,TC->rateMatchLen,TC->Qm, channelOut,out_real_channel, out_img_channel, La, snr);
			}

			/*cc decode , include : deinterleave , decode and crc check*/
			if(decoder_Type == 0)//float type
			{
				crcResult = LTE_Turbo_Decode_float(decodeOut,La, channelOut, TC->msgLen, TC->rateMatchLen, TC->NL, TC->Qm, TC->NIR, TC->rvIndex, TC);
				if(crcResult == 1){
					break;
				}
			}
			else if(decoder_Type == 1)//fixed type
			{
				/*Fixed for the input of decoder*/
				Fixed_point(TC->rateMatchLen, channelOut, channelOut_fix, lr, lr->fixed_method);

				/*cc decode , include : deinterleave , decode and crc check*/
				crcResult = LTE_Turbo_Decode_fixed(decodeOut,La_fix, channelOut_fix, TC->msgLen, TC->rateMatchLen, TC->NL, TC->Qm, TC->NIR, TC->rvIndex, TC,lr,others);

				/*Defixed for the demodulation of 16QAM and 64QAM*/
				De_fixed_point(TC->rateMatchLen, La_fix, La, lr, lr->fixed_method);
			
				if(crcResult == 1){
					break;
				}
			}
			else{
				printf("Error at decoder_Type!");
				getch();
				exit(0);
			}
			//crcResult = 1;
		}

		/*statistic error bits and frames after Turbo decoder*/
		goon = statistics(TC, SS, SP, decodeOut, originBit, !crcResult);

		if (goon)
		{
			break;
		}

		SS->simIndex++;//next frame

	}
	free(originBit);
	originBit = NULL;
	free(rateMatchOut);
	rateMatchOut = NULL;
	free(channelIn);
	channelIn = NULL;
	free(channelOut);
	channelOut = NULL;
	free(decodeOut);
	decodeOut = NULL;
	free(mod_symbols_rel);
	mod_symbols_rel = NULL;
	free(mod_symbols_img);
	mod_symbols_img = NULL;
	free(out_real_channel);
	out_real_channel = NULL;
	free(out_img_channel);
	out_img_channel = NULL;
	free(La);
	La = NULL;
	free(La_fix);
	La_fix = NULL;
	free(channelOut_fix);
	channelOut_fix = NULL;
}
/*
********************************************************************************
* NAME:			statistics
* PURPOSE:		对每个帧的译码错误进行统计

* Input:
snr:			比特信噪比
TC:				Turbo码本身的一些参数，包括编码、速率匹配、调制、译码等
SP:				仿真中赋予的参数
SS:				统计所得的FER和BER等参数
*
* Return:
crcWrong:		CRC校验的结果
*
* AUTHOR:		Duck
********************************************************************************
*/
int statistics(TurboCode *TC, StatisStruct *SS, SPStruct *SP, int *decodeOut, int *originBit, int crcWrong)
{
	int i;
	char	stop;													/*keyboard input*/
	FILE *fpTemp = NULL;
	FILE *fpResult = NULL;							/*file pointer to result*/
	FILE *fpRecError = NULL;
	int length = TC->msgLen;


	SS->curError = 0;

	for(i=0; i<length; i++)
	{
		if(decodeOut[i] != originBit[i])
		{
			SS->bitError++;
			SS->curError++;
		}
	}

	if (  ((TC->crcLen != 0) && crcWrong) || 
		((TC->crcLen == 0) && (SS->curError != 0))  )
	{
		SS->frameError++;
		if (SP->recordOn)
		{
			if(NULL == (fpRecError=fopen("ErrorFrame.txt","w")))
			{
				printf("can not open file: Temp.txt");
				getchar();
				exit(0);
			}

			/*save  PN seed*/
			fprintf(fpRecError,"current PN:\t");
			for (i=0; i<11; i++)
			{
				fprintf(fpRecError,"%d ",SS->lastPN.reg[i]);
			}
			fprintf(fpRecError,"\n");

			/*save noise seed*/
			fprintf(fpRecError,"current noise seed:\t%d %d %d\n",SS->lastNoise.ix,SS->lastNoise.iy,SS->lastNoise.iz);
			fclose(fpRecError);
		}
	}

	/*save the result to file*/
	if(NULL == (fpResult=fopen(SP->performance,"a")))
	{
		printf("\ncan not open file: Performance.txt");
		getchar();
		exit(0);
	}

	/*display the simulation*/
	if(SS->simIndex % SP->dispStep == 0)
	{
		SS->BER = (double)SS->bitError/(double)length/(double)SS->simIndex;
		SS->FER = (double)SS->frameError/(double)SS->simIndex;
		printf(" %.2f\t%10d\t%4d\t%5.3e\t%5.3e\r",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);
	}

	/*save to file*/
	if (SS->simIndex % SP->writeFreq == 0)
	{
		if(NULL == (fpTemp=fopen("Temp.txt","w")))
		{
			printf("can not open file: Temp.txt");
			getchar();
			exit(0);
		}

		/*save snr and sim_index*/
		fprintf(fpTemp,"current snr: %.2f\n",SS->snr);
		fprintf(fpTemp,"current sim index: %d\n",SS->simIndex);

		/*save error and iteration statistic*/
		fprintf(fpTemp,"current error frames: %d\n",SS->frameError);
		fprintf(fpTemp,"current error bits: %d\n",SS->bitError);

		/*save initial PN*/
		fprintf(fpTemp,"current PN:\t");
		for (i=0; i<11; i++)
		{
			fprintf(fpTemp,"%d ",SS->initPN.reg[i]);
		}
		fprintf(fpTemp,"\n");

		/*save noise seed*/
		fprintf(fpTemp,"current noise seed:\t%d %d %d\n",SS->initNoise.ix,SS->initNoise.iy,SS->initNoise.iz);
		fclose(fpTemp);
	}

	/*exit simulation when frame error and test frame satisfy the following expression*/
	if(SS->frameError >= SP->leastErrorFrame && SS->simIndex >= SP->leastTestFrame)
	{
		SS->BER = (double)SS->bitError/(double)length/(double)SS->simIndex;
		SS->FER = (double)SS->frameError/(double)SS->simIndex;
		printf(" %.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);
		fprintf(fpResult," %.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);

		if (SS->snr+SP->stepSnr>SP->stopSnr)
		{
			fprintf(fpResult, "\n\n\n");
		}
		fclose(fpResult);
		return 1;
	}


	/*stop the simulation and save current result to file*/
	if(_kbhit())
	{
		printf(" Continue<C>,  Next Pointer<N>,  Exit<E>:\t\t\t\t\r");
		_getch();
		stop = _getch();
		if('e' == stop)
		{
			SS->BER = (double)SS->bitError/(double)(length)/(double)SS->simIndex;
			SS->FER = (double)SS->frameError/(double)SS->simIndex;
			printf(" %.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);
			fprintf(fpResult," %.2f\t%10d\t%4d\t%5.3e\t%5.3e\n\n\n\n\n",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);

			if(NULL == (fpTemp=fopen("Temp.txt","w")))
			{
				printf("can not open file: Temp.txt");
				getchar();
				exit(0);
			}

			/*save snr and sim_index*/
			fprintf(fpTemp,"current snr: %.2f\n",SS->snr);
			fprintf(fpTemp,"current sim index: %d\n",SS->simIndex);

			/*save error and iteration statistic*/
			fprintf(fpTemp,"current error frames: %d\n",SS->frameError);
			fprintf(fpTemp,"current error bits: %d\n",SS->bitError);

			/*save initial PN*/
			fprintf(fpTemp,"current PN:\t");
			for (i=0; i<11; i++)
			{
				fprintf(fpTemp,"%d ",SS->initPN.reg[i]);
			}
			fprintf(fpTemp,"\n");

			/*save noise seed*/
			fprintf(fpTemp,"current noise seed:\t%d %d %d\n",SS->initNoise.ix,SS->initNoise.iy,SS->initNoise.iz);
			fclose(fpTemp);

			SS->snr = SP->stopSnr+1;//exit all
			fclose(fpResult);
			return 1;
		}
		else if ('n' == stop)
		{
			SS->BER = (double)SS->bitError/(double)(length)/(double)SS->simIndex;
			SS->FER = (double)SS->frameError/(double)SS->simIndex;
			printf(" %.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);
			fprintf(fpResult," %.2f\t%10d\t%4d\t%5.3e\t%5.3e\n",SS->snr,SS->simIndex,SS->frameError,SS->FER,SS->BER);

			fclose(fpResult);
			return 1;
		}
	}

	fclose(fpResult);
	return 0;


}
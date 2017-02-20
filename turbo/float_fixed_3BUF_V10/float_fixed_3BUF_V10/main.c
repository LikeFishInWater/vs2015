/*
*******************************************************************************
* Copyright (C) National Mobile Communications Research Laboratory 
* All rights reserved.
* 
* FILE NAME :	main.c
* ABSTRUCT:	This file performs the main function.
*	
* CURRENT VERSION:	V10
* AUTHOR:	Duck		2012-04-11
*
* REVISION HISTORY:
	V5		Duck		2011-04-21:Only support the BICM_ID of Turbo length less than 6144
	V6		Duck		2011-09-01:1.Support the BICM_ID of all lengthes; 2.Modify the order of SW_decoder according to the sliding widow in the subblock;
								   3.Modify the SW algorithm, wipe out the BPU_warm unit
	V7		Duck		2011-09-05:1.Add note;2.Add the head files for all the .c files 
	V10		Duck		2012-04-11:1.Updata the Turbo decoder without overlap in each sub-decoder for every code length in LTE standard
								   2.Put the float and fixed Turbo decoder without overlap together for hardware updata
*	
*******************************************************************************
*/
/*
********************************************************************************
*			INCLUDE FILES
********************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "define.h"
#include "struct.h"

void main()
{
	TurboCode		TC;				
	SPStruct		SP;
	StatisStruct	SS;
	LR	lr;
	OTHERS others;

	//get the simulation parameters from inital file
	GetParameter(&TC, &SP, &lr, &others);

	//Whether to start a new frame simulation: 0->a new decoding test, 1->continue last decoding test
	InitSimPara(&SS, &SP);

	//write the simulation parameters on the screen and the result file
	WriteLogo(&TC, &SS, &SP, &lr, &others);

	//simulate for each SNR
	for (SS.snr=SS.initStartSnr; SS.snr<=SP.stopSnr; SS.snr+=SP.stepSnr)
	{
		Test(SS.snr, &TC, &SS, &SP ,&lr, &others);
	}

	printf("\ntask finish\n");

	getchar();
}	


#include <math.h>
#include "define.h"
#include  "Viterbi_Decode.h"
#include "struct.h"


//struct state trellis[512+4][encoder_stateNum];
//int tre_i,tre_j;
void initial_trellis_Viterbi(int input_length,struct state *trellis){
	int tre_i,tre_j;
	trellis[0].addMetric=0;//初始化累加度量值，除0时刻0状态设为0外，其他时刻其他状态初始累加度量设为-1e7
	for(tre_j=1;tre_j<encoder_stateNum;tre_j++){
		trellis[tre_j].addMetric=-1e7;
	}
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
}

void ViterbiDecode(int infoBitLen,int input_length,double *decoderIn_systemBit,double *decoderIn_parityBit,int *DecoderOutput,
struct state *trellis,struct SerialListVA1 *slva1,double *AbsoluteDiff,int *BestPathState,struct SerialListVA2 slva2[],double *La,double *La_par,double sigma2){

	double branchMatric1=0,branchMatric2=0;
	//double weight1=0,weight2=0;
	int i,j;
	int k;//回溯时存储上一状态
	initial_trellis_Viterbi(input_length,trellis);
	for(i=1;i<input_length+1;i++){
		for(j=0;j<encoder_stateNum;j++){
			branchMatric1=trellis[encoder_stateNum*i+j].trans_out1[0]*La[i-1]
		/*	+trellis[encoder_stateNum*i+j].trans_out1[1]*La_par[i-1]   */                                           //此项为是否加校验位的外信息
			+(2*K)*(decoderIn_systemBit[i-1]*trellis[encoder_stateNum*i+j].trans_out1[0]+decoderIn_parityBit[i-1]*trellis[encoder_stateNum*i+j].trans_out1[1]);
			trellis[encoder_stateNum*i+j].weight1=trellis[encoder_stateNum*(i-1)+trellis[encoder_stateNum*i+j].last_state1].addMetric+branchMatric1;
			branchMatric2=trellis[encoder_stateNum*i+j].trans_out2[0]*La[i-1]
/*	    	+trellis[encoder_stateNum*i+j].trans_out2[1]*La_par[i-1]             */                                   //此项为是否加校验位的外信息
			+(2*K)*(decoderIn_systemBit[i-1]*trellis[encoder_stateNum*i+j].trans_out2[0]+decoderIn_parityBit[i-1]*trellis[encoder_stateNum*i+j].trans_out2[1]);
			trellis[encoder_stateNum*i+j].weight2=trellis[encoder_stateNum*(i-1)+trellis[encoder_stateNum*i+j].last_state2].addMetric+branchMatric2;
			if(trellis[encoder_stateNum*i+j].weight1>=trellis[encoder_stateNum*i+j].weight2){
				trellis[encoder_stateNum*i+j].addMetric=trellis[encoder_stateNum*i+j].weight1;
				trellis[encoder_stateNum*i+j].memory_last_state=trellis[encoder_stateNum*i+j].last_state1;
				trellis[encoder_stateNum*i+j].memory_trans_in=trellis[encoder_stateNum*i+j].trans_in1;
			}
			else{
				trellis[encoder_stateNum*i+j].addMetric=trellis[encoder_stateNum*i+j].weight2;
				trellis[encoder_stateNum*i+j].memory_last_state=trellis[encoder_stateNum*i+j].last_state2;
				trellis[encoder_stateNum*i+j].memory_trans_in=trellis[encoder_stateNum*i+j].trans_in2;
			}
		//printf("addMatric=%f\t",trellis[encoder_stateNum*i+j].addMetric);
		}
	}
	
 //回溯。
	k=0;
	BestPathState[input_length]=k;
	slva2[0].BestPathAddMetric=trellis[input_length*encoder_stateNum+k].addMetric;
	for(i=input_length-1;i>infoBitLen-1;i--){
		k=trellis[encoder_stateNum*(i+1)+k].memory_last_state;
		BestPathState[i]=k;
	}
	//printf("k=%d\taddMatric=%f\t",k,trellis[infoBitLen][k].addMetric);
	
	for(i=0;i<slva_L;i++){
	slva2[i].MergeTime=input_length;
	}
	for(i=infoBitLen-1;i>=0;i--){
		DecoderOutput[i]=trellis[encoder_stateNum*(i+1)+k].memory_trans_in;
		slva1[i].BestPathOut=DecoderOutput[i];
		k=trellis[encoder_stateNum*(i+1)+k].memory_last_state;
		BestPathState[i]=k;
		//printf("bestpathstate=%d\t",BestPathState[i][0]);
	}
	for(i=0;i<input_length;i++){
		AbsoluteDiff[i]=fabs(trellis[encoder_stateNum*(i+1)+BestPathState[i+1]].weight1-trellis[encoder_stateNum*(i+1)+BestPathState[i+1]].weight2);
	}
	AbsoluteDiff[slva2[0].MergeTime-1]=MAX;
}
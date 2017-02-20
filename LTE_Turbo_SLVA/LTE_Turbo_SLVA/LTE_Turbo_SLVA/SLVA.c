#include <math.h>
#include "define.h"
#include  "Viterbi_Decode.h"
#include "struct.h"
#include "SLVA.h"
#include"CRC_Decode.h"

void func_SLVA(int infoBitLen,int input_length,double *decoderIn_systemBit,double *decoderIn_parityBit,int *DecoderOutput,
	struct state *trellis,struct SerialListVA1 *slva1,double *AbsoluteDiff,int *BestPathState,struct SerialListVA2 slva2[],double *La,double *La_par,double sigma2){
	int l=0;
	int i,j,k;
	int revised_last_state;
	int revised_trans_in;
	double minAbsoluteDiff;
	int selectedPathNum;
	ViterbiDecode(infoBitLen,input_length,decoderIn_systemBit,decoderIn_parityBit,DecoderOutput,trellis,slva1,AbsoluteDiff,BestPathState,slva2,La,La_par,sigma2);
	
	if(CrcCheck(DecoderOutput, infoBitLen, 24, 0)==1)   return;
	//printf("crccheck=%d\n",CrcCheck(DecoderOutput, infoBitLen, 24, 0));
	while(l<slva_L-1){
		l++;
		//分别求前l条最优序列的待选汇入点，并记录待选路径累加度量。
		for(i=0;i<l;i++){
			slva2[i]. CandiMergeTime=1;
			minAbsoluteDiff=AbsoluteDiff[0+(infoBitLen+encoder_regNum)*i];
			for(k=1;k<input_length;k++){
				if(AbsoluteDiff[k+(infoBitLen+encoder_regNum)*i]<minAbsoluteDiff){
					minAbsoluteDiff=AbsoluteDiff[k+(infoBitLen+encoder_regNum)*i];
					slva2[i]. CandiMergeTime=k+1;
				}
			}
			
			slva2[i].PathAddMetric=slva2[i].BestPathAddMetric-minAbsoluteDiff;
		}

		//从前l条待选路径中选择累加度量最大的路径，作为第l+1条最优路径，并记录汇入点。
		slva2[l].BestPathAddMetric=slva2[0].PathAddMetric;
		slva2[l].MergeTime=slva2[0]. CandiMergeTime;
		selectedPathNum=0;
		for(i=1;i<l;i++){
			if(slva2[i].PathAddMetric>slva2[l].BestPathAddMetric){
				slva2[l].BestPathAddMetric=slva2[i].PathAddMetric;
				slva2[l].MergeTime=slva2[i]. CandiMergeTime;
				selectedPathNum=i;
			}
		}
		
		slva2[selectedPathNum].MergeTime=slva2[l].MergeTime;//MergeTime(l')中存储汇聚时刻。
		//修改汇入点的前向路径。
		if(trellis[encoder_stateNum*slva2[l].MergeTime+BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum]].last_state1==
			trellis[encoder_stateNum*slva2[l].MergeTime+BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum]].memory_last_state){
		revised_last_state=
		trellis[encoder_stateNum*slva2[l].MergeTime+BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum]].last_state2;
		revised_trans_in=
			trellis[encoder_stateNum*slva2[l].MergeTime+BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum]].trans_in2;

		}
		else{
			revised_last_state=
		trellis[encoder_stateNum*slva2[l].MergeTime+BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum]].last_state1;
			revised_trans_in=
				trellis[encoder_stateNum*slva2[l].MergeTime+BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum]].trans_in1;
		}
		//回溯，得到本次选出的最优路径状态序列及译码输出
		BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*l]=BestPathState[slva2[l].MergeTime+(infoBitLen+encoder_regNum+1)*selectedPathNum];
		slva1[slva2[l].MergeTime-1+infoBitLen*l].BestPathOut=revised_trans_in;
		BestPathState[slva2[l].MergeTime-1+(infoBitLen+encoder_regNum+1)*l]=revised_last_state;
		for(j=slva2[l].MergeTime-2;j>=0;j--){
		slva1[j+infoBitLen*l].BestPathOut=trellis[encoder_stateNum*(j+1)+BestPathState[j+1+(infoBitLen+encoder_regNum+1)*l]].memory_trans_in;
		BestPathState[j+(infoBitLen+encoder_regNum+1)*l]=trellis[encoder_stateNum*(j+1)+BestPathState[j+1+(infoBitLen+encoder_regNum+1)*l]].memory_last_state;
		}
		//汇入点后的输出序列与待选路径相同
		for(j=slva2[l].MergeTime;j<infoBitLen;j++){
			slva1[j+infoBitLen*l].BestPathOut=slva1[j+infoBitLen*selectedPathNum].BestPathOut;
			BestPathState[j+1+(infoBitLen+encoder_regNum+1)*l]=BestPathState[j+1+(infoBitLen+encoder_regNum+1)*selectedPathNum];
		}
		//尾比特不输出
		for(j=infoBitLen;j<input_length;j++){
			BestPathState[j+1+(infoBitLen+encoder_regNum+1)*l]=BestPathState[j+1+(infoBitLen+encoder_regNum+1)*selectedPathNum];
		}
		//把相应候选路径的相应汇入点扣除。
		AbsoluteDiff[slva2[l].MergeTime-1+(infoBitLen+encoder_regNum)*selectedPathNum]=MAX;
		
		for(j=0;j<infoBitLen;j++){
			DecoderOutput[j]=slva1[j+infoBitLen*l].BestPathOut;
		}
		if(CrcCheck(DecoderOutput, infoBitLen, 24, 0)==1) break;
		
	
		//计算此次选出的最优路径的AbsoluteDiff.
		for(j=0;j<input_length;j++){
				AbsoluteDiff[j+(infoBitLen+encoder_regNum)*l]=fabs(trellis[encoder_stateNum*(j+1)+BestPathState[j+1+(infoBitLen+encoder_regNum+1)*l]].weight1-trellis[encoder_stateNum*(j+1)+BestPathState[j+1+(infoBitLen+encoder_regNum+1)*l]].weight2);
			}
		AbsoluteDiff[slva2[l].MergeTime-1+(infoBitLen+encoder_regNum)*l]=MAX;
		
	}
}

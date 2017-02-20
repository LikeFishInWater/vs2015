
#include "define.h"

void initial_trellis_Viterbi(int input_length,struct state *trellis);
void ViterbiDecode(int infoBitLen,int input_length,double *decoderIn_systemBit,double *decoderIn_parityBit,int *DecoderOutput,struct state *trellis,
struct SerialListVA1 *slva1,double *AbsoluteDiff,int *BestPathState,struct SerialListVA2 slva2[],double *La,double *La_par,double sigma2);
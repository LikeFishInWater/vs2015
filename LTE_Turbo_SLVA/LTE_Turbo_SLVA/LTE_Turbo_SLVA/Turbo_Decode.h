#include<math.h>

void TurboDecode_SLVA(int infoBitLen,int input_length,int *gen,double *decoderIn_systemBit,double *decoderIn_parityBit,
	double *decoderIn_afterInterleave_systemBit,double *decoderIn_iparityBit,int f1,int f2,double *L,double *exLLR,int *DecoderOutput,
	struct state *trellis,double sigma2);
void QppInterleave(int infoBitLen,double *beforeQpp,double *afterQpp,int *temp,int f1,int f2);
void QppDeInterleave(int infoBitLen,double *beforeDeQpp,double *afterDeQpp,int *temp);
void Turbo_Component_Decoder(int input_length,double *systemBit,double *parityBit,double *La,double *L,double *Le,struct state *trellis,double *L_par,double *La_par,double *Le_par, double sigma2);

double max_2(double a,double b);
double max_n(double *d,int n);
double max_n_star(double *d, int n);
void initial_trellis_Turbo(int input_length,struct state *trellis);
void Branch_gamma(int input_length,double dV1[],double dV2[],double *La,double *La_par,struct state *trellis,double sigma2);
void Forward_alpha(int input_length,struct state *trellis);
void Backward_beta(int input_length,struct state *trellis);
double Likehood_Ratio_L(int i,struct state *trellis,double *L_par);
void memadd(double *out, double *in1, double *in2, int length, double factor);
void OSD_gauss_elimate_to_I_P (int *gen , int *index , int *out , double *input , int row, int col);
void  reorder_G_by_sitep(double *input , int *gen , int *target , int *output , int *index1 , int *index2 , int row, int col);
void LDPC_encode (int *gen , int *input , int *output , int row, int col);
void  restore_by_index_reorder(int *index1 , int *index2 , int *out , int length);
double  compute_min_distance(double *noise_in , int *test , double last_min , int *out , int length);
void Flipping_encode(int *data , int *code , int length);
void OSD_process(int *oneDecodeOut , double	*sum_LLR , int * gen , int lenCB);

double max_2_star(double a, double b);
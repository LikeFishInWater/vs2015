

#include "define.h"

void TurboEncode(int infoBitLen, int *infoBit, int *systemBit, int *parityBit, int *iParityBit, int f1, int f2);
void set_Matrix (int *h , int *g , int n , int m , int k);
void index_2nd_constituent_encoder(int length, int *PP, int *GG, int col_ID);
void set_Matrix_CRC(int *g, int m, int n);
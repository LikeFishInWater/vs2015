#ifndef Modu_H
#define Modu_H

void Modulation(int modType, double *mod_symbols_rel, double *mod_symbols_img, int *input ,int length_input);

double trans_pdf(double detected_symb_rel, double detected_symb_img, double symb_rel, double symb_img, double snr, int modType);

void Demodulation(int bigIT, int demodType,int lenOutput,int modType, double *LLR, double *detected_symb_rel, double *detected_symb_img, double *La, double snr);

void Demodulation_LogMAP(int bigIT, int lenOutput,int modType, double *LLR, double *detected_symb_rel, double *detected_symb_img, double *La, double snr );

void Demodulation_MaxLogMAP(int bigIT, int lenOutput,int modType, double *LLR, double *detected_symb_rel, double *detected_symb_img, double *La, double snr );

#endif
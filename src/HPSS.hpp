
#ifndef _HPSS_HPP_
#define _HPSS_HPP_

#include<complex>
#include<cmath>

//#include<omp.h>

#include"slideBlock.hpp"
#include"myfft.h"

class HPSS{
public:
  HPSS(){};
  void init(int, int, int, double, double);
  ~HPSS();

  void push_new_data(double*);
  void update(int);
  void pop(double*, double*);

  bool eof(){return push_pop_count < 0 ? true : false;} 
  void flag_on(){count_flag = true;}
  bool filled(){return push_pop_count >= block_size ? true : false;}

private:
  int block_size;
  int frame_length;
  int frame_shift;
  double w_h;
  double w_p;

  int push_pop_count;
  bool count_flag;

  SlideBlock<double> *W;
  SlideBlock<double> *H, *H_buffer;
  SlideBlock<double> *P, *P_buffer;
  SlideBlock<double> *maskH, *maskH_buf;
  SlideBlock<double> *maskP, *maskP_buf;
  SlideBlock<double> *phaseSpec;

  double *amplitude, *phase, *mask_def;
  std::complex<double> *fft_ed, *Hcompspec, *Pcompspec;

  FFT_forward  *fft_forward;
  FFT_backward *fft_backward_H, *fft_backward_P;
  static std::complex<double> J; // sqrt(-1)

};
std::complex<double> HPSS::J (0.0, 1.0);

////////////////////////////////////////////////////////////////////////////////////
void HPSS::init(int a, int b, int c, double d, double e)
{
  block_size = a;
  frame_length = b;
  frame_shift = c;
  push_pop_count = 0;
  count_flag = false;
  w_h = d;
  w_p = e;

  W =         new SlideBlock<double>(block_size, frame_length/2 + 1); 
  H =         new SlideBlock<double>(block_size, frame_length/2 + 1);
  P =         new SlideBlock<double>(block_size, frame_length/2 + 1);
  H_buffer =  new SlideBlock<double>(block_size, frame_length/2 + 1);
  P_buffer =  new SlideBlock<double>(block_size, frame_length/2 + 1);
  phaseSpec = new SlideBlock<double>(block_size, frame_length/2 + 1);    
  maskH =     new SlideBlock<double>(block_size, frame_length/2 + 1);
  maskP =     new SlideBlock<double>(block_size, frame_length/2 + 1);
  maskH_buf = new SlideBlock<double>(block_size, frame_length/2 + 1);
  maskP_buf = new SlideBlock<double>(block_size, frame_length/2 + 1);
  mask_def = new double[frame_length/2 + 1];

  amplitude = new double[frame_length/2 + 1];
  phase = new double[frame_length/2 + 1];
  fft_ed = new std::complex<double>[frame_length/2 + 1];
  Hcompspec = new std::complex<double>[frame_length/2 + 1];
  Pcompspec = new std::complex<double>[frame_length/2 + 1];

  fft_forward = new FFT_forward(frame_length);
  fft_backward_H = new FFT_backward(frame_length);
  fft_backward_P = new FFT_backward(frame_length);
}


HPSS::~HPSS(){
  delete W;
  delete H;
  delete P;
  delete H_buffer;
  delete P_buffer;
  delete phaseSpec;

  delete maskH;
  delete maskP;
  delete maskH_buf;
  delete maskP_buf;
  delete []mask_def;

  delete []amplitude;
  delete []phase;
  delete []fft_ed;
  delete []Hcompspec;
  delete []Pcompspec;

  delete fft_forward;
  delete fft_backward_H;
  delete fft_backward_P;
}

// push new data
void HPSS::push_new_data(double *x){
  if(count_flag == false)push_pop_count++;
  for(int i = 0 ; i < frame_length; i++){
    fft_forward->in[i] = x[i] * sin(M_PI * i/ frame_length);
  }
  fft_forward->exec();
  for(int i = 0; i < frame_length/2 + 1; i++){
    amplitude[i] = abs(fft_forward->out[i]);
    phase[i] =     arg(fft_forward->out[i]);
    mask_def[i] = 0.5;
  }

  W->push(&(amplitude[0]));
  H->push(&(amplitude[0]));
  P->push(&(amplitude[0]));
  phaseSpec->push(&(phase[0]));
  maskH->push(&(mask_def[0]));
  maskP->push(&(mask_def[0]));
}

// HPSS updating formula
inline void HPSS::update(int n){
  static double H_A = 2 * w_h + 2;
  static double P_A = 2 * w_p + 2;

//  #pragma omp parallel
  for(int iter = 0; iter < n; iter++){
//    #pragma omp for schedule(guided)
    for(int t = 1; t < block_size-1; t++){
      for(int k = 1; k < frame_length/2; k++){
        double w = pow((*W)[t][k],2);

        double H_B = w_h * ((*H)[t+1][k] + (*H)[t-1][k]);
        double H_C = 2 * (*maskH)[t][k] * w;
        (*H_buffer)[t][k] = (H_B + sqrt(H_B*H_B + 4 * H_A * H_C))/(2*H_A);

        double P_B = w_p * ((*P)[t][k+1] + (*P)[t][k-1]);
        double P_C = 2 * (*maskP)[t][k] * w;
        (*P_buffer)[t][k] = (P_B + sqrt(P_B*P_B + 4 * P_A * P_C))/(2*P_A);
      }
    }
//    #pragma omp for schedule(guided)
    for(int t = 1; t < block_size-1; t++){
      for(int k = 1; k < frame_length/2; k++){
        (*H)[t][k] = (*H_buffer)[t][k];
        (*P)[t][k] = (*P_buffer)[t][k];
	      double tmp = pow((*H)[t][k],2)
	                 + pow((*P)[t][k],2);
        if(tmp > 0.0){
          (*maskH)[t][k] = pow((*H)[t][k],2) / tmp;
          (*maskP)[t][k] = pow((*P)[t][k],2) / tmp;
        }else{
          (*maskH)[t][k] = 0.5;
          (*maskP)[t][k] = 0.5;
        }
      }
    }
  }
}


//pop
void HPSS::pop(double *Hout, double *Pout){
  push_pop_count--;
  H->pop(&(Hout[0])); // trash
  P->pop(&(Pout[0])); // trash
  W->pop(&(amplitude[0])); // used
  phaseSpec->pop(&(phase[0]));// used
  maskH->pop(&(Hout[0])); // used
  maskP->pop(&(Pout[0])); // usedB

  // amplitude
  for(int j = 0; j < frame_length/2 + 1; j++){
    Hout[j] = amplitude[j] * Hout[j]; // Hout in rhs means time-frequency mask
    Pout[j] = amplitude[j] * Pout[j]; 
  }
    
  // phase
  for(int j = 0; j < frame_length/2 + 1; j++){
    std::complex<double> isou(cos(phase[j]),sin(phase[j]));
    std::complex<double> conj(cos(phase[j]),-sin(phase[j]));
    Hcompspec[j] = Hout[j] * isou;
    Pcompspec[j] = Pout[j] * isou;
  }

  // inverse STFT
  for(int i = 0; i < frame_length/2 + 1; i++){
    fft_backward_H->in[i] = Hcompspec[i];
    fft_backward_P->in[i] = Pcompspec[i];
  }
  fft_backward_H->exec();
  fft_backward_P->exec();
  for(int i = 0; i < frame_length; i++){
    Hout[i] = fft_backward_H->out[i] * sin(M_PI * i / frame_length);
    Pout[i] = fft_backward_P->out[i] * sin(M_PI * i / frame_length);
  }
}

#endif


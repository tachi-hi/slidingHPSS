
#include<complex>
#include<fftw3.h>

// wrappaer of fftw
class FFT_forward{
public:
  FFT_forward(int size)
  {
    real = new double[size];
    cmplx = new std::complex<double> [size/2+1];
    p = fftw_plan_dft_r2c_1d(
                        size,
                        real,
                        reinterpret_cast<fftw_complex*>(cmplx),
                        FFTW_ESTIMATE);
    in = real;
    out = cmplx;
  }
  ~FFT_forward(){
    fftw_destroy_plan(p);
    delete[] real;
    delete[] cmplx;
  }
  void exec(){
    fftw_execute(p);
  }

  double *in;
  std::complex<double> *out;
private:
  fftw_plan p;
  double *real;
  std::complex<double> *cmplx;
};

class FFT_backward{
public:
  FFT_backward(int size__)
  {
    size = size__;
    real = new double[size];
    cmplx = new std::complex<double> [size/2+1];
    p = fftw_plan_dft_c2r_1d(
                          size,
                          reinterpret_cast<fftw_complex*>(cmplx),
                          real,
                          FFTW_ESTIMATE);
    in = cmplx;
    out = real;
  }
  ~FFT_backward(){
    fftw_destroy_plan(p);
    delete[] real;
    delete[] cmplx;
  }

  void exec(){
    fftw_execute(p);
    for(int i = 0; i < size; i++){
      out[i] /= static_cast<double>(size); // bug? (static_cast<double>(size)/2 + 1); 
    }
  }

  double *out;
  std::complex<double> *in;
private:
  fftw_plan p;
  double *real;
  std::complex<double> *cmplx;
  int size;
};



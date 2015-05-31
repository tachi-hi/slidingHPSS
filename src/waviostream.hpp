/**************************************************************
  wav file io

Coded by Hideyuki Tachibana (tachi-hi @ github)

History:
  2007 Dec. wavio class First implementation
  2009 Jul. Refactering (template class)
  2009 Aug. Removed Boost
  2011 Feb. Refactering (removed some obsolete functions)
**************************************************************/

#include<iostream>
#include<fstream>
#include<cstdio>
#include<cstdlib>
#include<cstring>

#include<cmath>
#include<vector>

#include<string>
#include<algorithm>

namespace wav{
  struct Header;
  class MonoChannel;
  class Signal;

  enum {left = 0, right = 1};

  // realization, quantization
  // unsigned char, signed short -> double 
  template<typename T> double realization(T){return 0.0;}; // do not use this function, but C++ grammar requires the defeinition
  template<> double realization(unsigned char x){return static_cast<double>(x + 128) / 256.0;};
  template<> double realization(signed short x) {return static_cast<double>(x)/32768.0;};
  // double -> unsigned char, signed short
  template<typename T> T quantize(double){;}; // do not use this function, but C++ grammar requires the defeinition
  template<> unsigned char quantize<unsigned char>(double x){return static_cast<unsigned char>(x * 256 - 128);};
  template<> signed short  quantize<signed short> (double x){return static_cast<signed short> (x * 32768);};
};

/****************************************************************************************/
// header
/****************************************************************************************/
struct wav::Header{
  char riff[4];
  unsigned int filesize;
  char wavefmt[8];
  unsigned int waveformat;
  unsigned short int pcm;
  unsigned short int n_channel;
  unsigned int sampling_rate;
  unsigned int bytes_per_second;
  unsigned short int block_per_sample;
  unsigned short int bits_per_sample;
  char data[4];
  unsigned int n_byte;
};

/****************************************************************************************/
// 1 channel monaural signal
/****************************************************************************************/
class wav::MonoChannel{
public:
  MonoChannel(){ flag = false; }
  void init(int buffer_size_){
    buffer_size = buffer_size_;
    data = new double[buffer_size];
    flag = true;
    current_point = 0;
  }
  ~MonoChannel(){ if(flag) delete []data; }
  void insert(double x){
    if(flag){
      data[current_point] = x;
      current_point = (current_point < buffer_size - 1 ? current_point + 1 : 0);
    }
  };
  double& operator[](int i){
#ifdef DEBUG
    if(flag)
#endif
      return data[i + current_point < buffer_size ? i + current_point : i + current_point - buffer_size];
  }
private:
  double *data;
  bool flag;
  int current_point;
  int buffer_size;
};


/****************************************************************************************/
// multi channel signal
/****************************************************************************************/
class wav::Signal{
public:
  Signal(){}
  void init(int n_channel, int buf_size){
    buffer_size = buf_size;
    data.reserve(n_channel);
    for(int i = 0; i < n_channel; i++){
      data[i].init(buf_size);
    }
  }
  ~Signal(){}
  int length(){return buffer_size;}//‚±‚ñ‚ÈŠÖ”•K—v‚¾‚ë‚¤‚©H
  MonoChannel& operator[](int n){return data[n];}
private:
  std::vector<MonoChannel> data;
  int buffer_size;
};

/****************************************************************************************/
//wavistream
/****************************************************************************************/
class wavistream{
public:
  wavistream(const char*, int);
  ~wavistream();
  void read(int n){if(header.bits_per_sample == 8)  __read<unsigned char>(n); else  __read<signed short>(n);};
  void copy(double* x, int ch, int n){for(int i = 0; i < n; i++)x[i] = signal[ch][i];}; // temporary
  bool eof(){return feof(fp);}
  wav::Header header;
private:
  wav::Signal signal;
  FILE *fp;
  int current_seek; // where we are reading 
  template<typename T> void __read(int n);
};

wavistream::wavistream(const char *fn, int buffer_size){
  fp = fopen(fn, "r");
  if( !fp ){
    std::cerr << "cannot open " << fn << "!" <<std::endl;
    exit(1);
  }
  // read header
  if(!fread(&header, sizeof(header), 1, fp));
  current_seek = 0; 
  signal.init(header.n_channel, buffer_size);
}

wavistream::~wavistream(){
  fclose(fp);
}

template<typename T> void // unsigned char or signed short
wavistream::__read(int n){
  T tmp;
  double tmp_double;
  // fseek head
  fseek(fp, sizeof(wav::Header) + current_seek, SEEK_SET);
  // read data
  for(int i = 0; i < n ; i++){
    for(int ch = 0; ch < header.n_channel; ch++){
      if(!eof()){
        if(!fread(&tmp, sizeof(T), 1, fp));
        tmp_double = wav::realization(tmp);
        signal[ch].insert( tmp_double );
      }else{
        signal[ch].insert( 0.0 );
      }
    }
  }
  current_seek += n * header.n_channel * sizeof(T);
}

/****************************************************************************************/
//wavostream
/****************************************************************************************/
class wavostream{
public:
  wavostream(const char*, int, int);
  ~wavostream();
  void set(double*, int, int);
  void write(int n){if(header.bits_per_sample == 8)  __write<unsigned char>(n); else  __write<signed short>(n);};;
  void write_header();
  wav::Header header;
  
private:
  wav::Signal signal;
  FILE *fp;
  template<typename T> void __write(int);
  int signal_length;
  int buffer_size;
};

wavostream::wavostream(const char *fn, int ch, int buffer_size){
  fp = fopen(fn, "w");
  if( !fp ){
    std::cerr << "cannot open " << fn << "!" <<std::endl;
    exit(1);
  }
  signal_length = 0;

  // header is filled by 0 
  char tmp[sizeof(wav::Header)];
  fwrite(tmp, 1, sizeof(wav::Header), fp);

  signal.init(ch, buffer_size);
  header.n_channel = ch;
  this->buffer_size = buffer_size;
}

wavostream::~wavostream(){
  write_header();
  fclose(fp);
}

void wavostream::set(double *x, int ch, int n){
  for(int t = 0; t < n; t++){
    signal[ch].insert( x[t] > 1.0 ? 1.0 : x[t] < -1.0 ? -1.0 : x[t]); // saturation
  }
}

// write data
// write from the tail
template<typename T> void // unsigned char or signed short
wavostream::__write(int n){
  for(int j = buffer_size - n; j < buffer_size; j++){
    for(int ch = 0; ch < header.n_channel; ch++){
      T tmp = wav::quantize<T>(signal[ch][j]);
      fwrite(&tmp, sizeof(T), 1, fp);
    }
  }
  signal_length += n;
}

void wavostream::write_header(void){
  if(header.sampling_rate == 0 || header.n_channel == 0 || header.bits_per_sample == 0){
    std::cerr << "sampling_rate, n_channel & bit_rate are not set!" << std::endl;
    exit(1);
  }

  strncpy( header.riff, "RIFF", 4);
  header.filesize = (header.bits_per_sample / 8) * signal_length * header.n_channel + 36;
  strncpy( header.wavefmt, "WAVEfmt ", 8);
  header.waveformat = header.bits_per_sample;
  header.pcm = 1;
  header.bytes_per_second = (header.bits_per_sample / 8) * header.n_channel * header.sampling_rate;
  header.block_per_sample = (header.bits_per_sample / 8) * header.n_channel;
  strncpy( header.data, "data", 4);
  header.n_byte = (header.bits_per_sample / 8) * signal_length * header.n_channel; 
  fseek(fp, 0, SEEK_SET);
  fwrite(&header, sizeof(header), 1, fp);

}



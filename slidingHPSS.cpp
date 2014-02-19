#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <new>

#include"waviostream.hpp"
#include"HPSS.hpp"

#include<unistd.h>
#include<getopt.h>
using namespace std;


int main(int argc, char **argv){
  std::string input_file_name("");
  std::string H_file_name("_H.wav");
  std::string P_file_name("_P.wav");
  int frame(512);
  int block_size(100);
  double sigmaH(0.30); double wh = 11.;
  double sigmaP(0.30); double wp = 11.;
  
  while (1) {
    int option_index = 0;
    static struct option long_options[] = {
      {"help",    0,      0,   'h'},
      {"info",    0,      0,   'h'},
      {"input",   1,      0,   'i'},
      {"Hout",    1,      0,   'H'},
      {"Pout",    1,      0,   'P'},
      {"frame",   1,      0,   'f'},
      {"block",   1,      0,   'b'},
      {"sigmaH",  1,      0,   'Q'},
      {"sigmaP",  1,      0,   'W'},
      {0,         0,      0,   0} 
    };
    int c = getopt_long(argc, argv, "hi:H:P:f:b:Q:W:",
                        long_options, &option_index);
    if (c == -1) break;
    
    switch (c) {
    case'h':{
      cerr << "Harmonic/percussive sound separation" << endl;
      cerr << "Usage:" << argv[0] << " -i[input file name] [and other options you need]" << endl;
      cerr << "Options:" << endl;
      cerr << " --help or --info [-h]\t Display this document." << endl;
      cerr << " --input  [-i] <arg>\t Input file name.\n\t\t\t The file should be monaural PCM .wav file" << endl;
      cerr << " --Hout   [-H] <arg>\t Output H name, default: _H.wav" << endl;
      cerr << " --Pout   [-P] <arg>\t Output P name, default: _P.wav" << endl;
      cerr << " --frame  [-f] <arg>\t Frame length of the STFT, default: 512[sample]" << endl;
      cerr << " --block  [-b] <arg>\t Block size of the HPSS, default: 30" << endl;
      cerr << " --sigmaH [-Q] <arg>\t H weighting constant of the HPSS, default: 0.3" << endl;
      cerr << " --sigmaP [-W] <arg>\t P weighting constant of the HPSS, default: 0.3" << endl;
      cerr << "(c) Hideyuki Tachibana 2008--2011." << endl;
      cerr << "tachibana@hil.t.u-tokyo.ac.jp" << endl;
      exit(1);
    }
    case'i':{input_file_name = optarg; break;}
    case'H':{H_file_name = optarg;break;}
    case'P':{P_file_name = optarg;break;}
    case'f':{frame = atoi(optarg);break;}
    case'b':{block_size = atoi(optarg);break;}
    case'Q':{sigmaH = atof(optarg); break;}
    case'W':{sigmaP = atof(optarg); break;}
    default:
      cerr << "?? getopt returned character code 0" << c << endl;
    }
    wh = 1.0/sigmaH/sigmaH;
    wp = 1.0/sigmaP/sigmaP; 
  }
  /*********************************************************************************************************/
  
  wavistream *input;
  wavostream *H, *P;
  input = new wavistream(input_file_name.c_str(), frame);
  int n_channel = input->header.n_channel;
  H = new wavostream(H_file_name.c_str(), n_channel, frame/2);
  P = new wavostream(P_file_name.c_str(), n_channel, frame/2);
  P->header = H->header = input->header;
  
  HPSS *hpss = new HPSS[n_channel];
  for(int i = 0; i < n_channel; i++){
    hpss[i].init(block_size, frame, frame/2, wh, wp);
  }
  const int iteration_one_step = 1; 
  
  std::vector<std::vector<double> > frame_in, frame_H, frame_P;
  std::vector<std::vector<double> > old_H, old_P; 
  for(int i = 0; i < n_channel; i++){
    std::vector<double> frame_in_tmp(frame);
    frame_in.push_back(frame_in_tmp);
    frame_H.push_back(frame_in_tmp);
    frame_P.push_back(frame_in_tmp);
    std::vector<double> oldH_tmp(frame/2);
    old_H.push_back(oldH_tmp);
    old_P.push_back(oldH_tmp);
  }

  // initial frame
  input->read(frame/2);
  for(int i = 0; i < block_size - 1; i++){ 
    input->read(frame/2);
    for(int ch = 0; ch < n_channel; ch++){
      input->copy(&(frame_in[ch][0]), ch, frame);
      hpss[ch].push_new_data(&(frame_in[ch][0]));
      hpss[ch].update(iteration_one_step);
    }
  }

  while( !hpss[0].eof()){
    input->read(frame/2);
    for(int ch = 0; ch < n_channel; ch++){
      input->copy(&(frame_in[ch][0]), 0, frame);
      hpss[ch].push_new_data(&(frame_in[ch][0]));
      hpss[ch].update(iteration_one_step);
  
      if( input->eof() )
        hpss[ch].flag_on();

      // pop
      hpss[ch].pop(&(frame_H[ch][0]), &(frame_P[ch][0]));
      for(int i = 0; i < frame/2; i++){
        frame_H[ch][i] += old_H[ch][i];
        frame_P[ch][i] += old_P[ch][i];
        old_H[ch][i] = frame_H[ch][frame/2 + i];
        old_P[ch][i] = frame_P[ch][frame/2 + i];
      }
      H->set(&(frame_H[ch][0]), ch, frame/2);
      P->set(&(frame_P[ch][0]), ch, frame/2);
    }

    // save
    H->write(frame/2);
    P->write(frame/2);
  }
  delete input;
  delete H;
  delete P;
  delete[] hpss;
  
  return 0;
}



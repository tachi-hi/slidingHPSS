sliding HPSS
============

Coded by : Hideyuki Tachibana


What's this?
------------
This is an example of the implementation of "sliding HPSS" described in the following paper.

* N. Ono, K. Miyamoto, H. Kameoka, S. Sagayama "A real-time equalizer of harmonic and percussive components in music signals" in Proc. ISMIR 2008.

This program is coded for following paper [(IEEE Xplore)](https://doi.org/10.1109/TASLP.2013.2287052) on singing voice enhancement,
in which it is applied twice with different parameter sets.
```bibtex
@article{tachibana2014singing,
  author={Hideuyki Tachibana and Nobutaka Ono and Shigeki Sagayama},
  journal={IEEE/ACM Transactions on Audio, Speech, and Language Processing}, 
  title={Singing Voice Enhancement in Monaural Music Signals Based on 
  	Two-stage Harmonic/Percussive Sound Separation on Multiple Resolution Spectrograms}, 
  year={2014},
  volume={22},
  number={1},
  pages={228-237}
}
```
See also the following conferene paper [(IEEE Xplore)](https://doi.org/10.1109/ICASSP.2010.5495764).
```bibtex,
@inproceedings{tachibana2010melody,
  author={Hideyuki Tachibana and Tatsuma Ono and Nobutaka Ono and Shigeki Sagayama},
  booktitle={2010 IEEE International Conference on Acoustics, Speech and Signal Processing}, 
  title={Melody line estimation in homophonic music audio signals 
  	based on temporal-variability of melodic source}, 
  year={2010},
  pages={425-428}
}
```
How to use (on UNIX like system.)
---------------------------------

### Build

Download the codes as follows.

	cd your_working_directory
	git clone https://github.com/tachi-hi/slidingHPSS

Install `FFTW3` (`libfftw3`) in your system. If you are an Ubuntu user,

	apt-get install libfftw3-dev

Then type the follwoing commands to build the code and execute the obtained binary.

	bash setup.sh
	./bin/slidingHPSS -h

then usage massage will be displayed.

### Preprocessing: Stereo 44.1kHz MP3 -> Mono 16kHz WAV

`YourMP3file.mp3` is converted to a suitable format by following commands.

	mpg123 -w wavefile_tmp.wav yourMP3file.mp3
	sox wavfile_tmp.wav -r 16000 -c 1 input.wav
	rm wavfile_tmp.wav

### Singing Voice Enhancement

Just run the script file `twostageHPSSscripts/twostageHPSS.sh`. The file named `input.wav` in the working directory will be separated into three files named `_H.wav`, `_P.wav`, and `_V.wav`.

### Singing Voice Suppression

You can similarly obtain a "karoake" signal just by mixing the two files `_H.wav` and `_P.wav` as follows.

	sox -m _H.wav _P.wav _karaoke.wav

See also [euterpe](https://github.com/tachi-hi/euterpe), an automatic audio-to-audio karaoke system.

## License
Free

### Academic Use
Please cite one of the papers above.

## Links

### Other implementations of HPSS

+ Our group
	+ [(non-sliding) HPSS](https://github.com/tachi-hi/HPSS)
	+ [vocal suppression based on multi-stage HPSS ("euterpe")](https://github.com/tachi-hi/euterpe)
	+ HPSS with GUI (http://hil.t.u-tokyo.ac.jp/software/) ... [expired link]
+ Others
	+ [librosa](http://librosa.github.io/librosa/): This audio library for Python contains a variant of HPSS.

### Data
+ [MIR-1K](https://sites.google.com/site/unvoicedsoundseparation/mir-1k)


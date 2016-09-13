sliding HPSS
============

Coded by : Hideyuki Tachibana


What's this?
------------
This is an example of the implementation of "sliding HPSS" described in the following paper.

* N. Ono, K. Miyamoto, H. Kameoka, S. Sagayama "A real-time equalizer of harmonic and percussive components in music signals" in Proc. ISMIR 2008.

This program is coded for following paper on singing voice enhancement,
in which it is applied twice with different parameter sets.
* H. Tachibana, N. Ono, S. Sagayama "Singing Voice Enhancement in Monaural Music Signals Based on Two-stage Harmonic/Percussive Sound Separation on Multiple Resolution Spectrogram," IEEE/ACM Trans. ASLP 22(1), 228--237, 2014.


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

## License
Free

### Academic Use
Please cite one of the papers above.

## Links

### Other implementations of HPSS

+ Our group
	+ [(non-sliding) HPSS](https://github.com/tachi-hi/HPSS)
	+ [HPSS with GUI](http://hil.t.u-tokyo.ac.jp/software/HPSS/)
+ Others
	+ [librosa](http://librosa.github.io/librosa/): This audio library for Python contains a variant of HPSS.

### Data
+ [MIR-1K](https://sites.google.com/site/unvoicedsoundseparation/mir-1k)


sliding HPSS
============

Coded by : Hideyuki Tachibana


What's this?
------------
This is an example of the implementation of "sliding HPSS" described in the following paper.

* N. Ono, K. Miyamoto, H. Kameoka, S. Sagayama "A real-time equalizer of harmonic and percussive components in music signals" in Proc. ISMIR 2008.

This program is coded for following paper on singing voice enhancement.
in which it is applied twice with different parameter sets.
* H. Tachibana, N. Ono, S. Sagayama "Singing Voice Enhancement in Monaural Music Signals Based on Two-stage Harmonic/Percussive Sound Separation on Multiple Resolution Spectrogram," IEEE/ACM Trans. ASLP 22(1), 228--237, 2014.


How to use (on UNIX like system.)
---------------------------------

+ Download the codes.
+ Install `FFTW3` (`libfftw3`) in your system.

Then type the command
	make
	./slidingHPSS -h

then usage command will be displayed.

License
-------
Free.

Academic Use
------------
Please cite one of the papers above.


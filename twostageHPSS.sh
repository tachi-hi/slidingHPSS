#!/usr/bin/bash

# This is an example of two-stage HPSS

./slidingHPSS --input input.wav --block 100 --frame 512  --Hout _H1.wav --Pout _P.wav -Q 0.3 -W 0.3 
./slidingHPSS --input _H1.wav   --block 100 --frame 4096 --Hout _H.wav  --Pout _V.wav -Q 0.3 -W 0.3 


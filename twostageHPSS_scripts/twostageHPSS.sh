#!/usr/bin/bash

BINDIR=./bin
HPSS=${BINDIR}/slidingHPSS
# This is an example of two-stage HPSS

${HPSS} --input input.wav --block 100 --frame 128  --Hout _H1.wav --Pout _P.wav -Q 0.3 -W 0.3 
${HPSS} --input _H1.wav   --block 100 --frame 8192 --Hout _H.wav  --Pout _V.wav -Q 0.3 -W 0.3 


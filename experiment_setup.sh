#!/bin/bash

YOUR_WORKING_DIRECTORY=experiments

bash setup.sh

# create a directory 
if [ ! -d $YOUR_WORKING_DIRECTORY ];
then mkdir $YOUR_WORKING_DIRECTORY; fi

if [ ! -d $YOUR_WORKING_DIRECTORY/tmp ];
then mkdir $YOUR_WORKING_DIRECTORY/tmp; fi

if [ ! -d $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX ];
then mkdir $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX
touch $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/place_here_MIR1K_data; fi

if [ ! -d $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/Wavfile ];
then mkdir $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/Wavfile; fi

# Example files
if [ ! -d $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/Wavfile/ ];
then 
touch $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/Wavfile/Ani_1_01.wav
touch $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/Wavfile/Ani_1_02.wav
touch $YOUR_WORKING_DIRECTORY/MIR-1K_for_MIREX/Wavfile/Ani_1_03.wav
fi



cp ./bin/slidingHPSS $YOUR_WORKING_DIRECTORY
cp -r ./twostageHPSS_scripts/R/* $YOUR_WORKING_DIRECTORY



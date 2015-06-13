#!/bin/bash

# make
cd src; make; cd ..

# create a directory "bin"
if [ ! -d bin ];
then
mkdir bin
fi

# mv
mv src/slidingHPSS bin


#!/bin/bash
cd ~
mkdir mcptam_deps
cd mcptam_deps

#TooN
wget http://www.edwardrosten.com/cvd/TooN-2.2.tar.gz
tar vxf TooN-2.2.tar.gz
cd TooN-2.2
./configure
make
make install
cd ..

#libcvd
wget http://www.edwardrosten.com/cvd/libcvd-20150407.tar.gz
tar vxf libcvd-20150407.tar.gz
cd libcvd-20150407
export CXXFLAGS=-D_REENTRANT
./configure --without-ffmpeg
make
make install
cd ..

#gvars3
wget http://www.edwardrosten.com/cvd/gvars-3.0.tar.gz
tar vxf gvars-3.0.tar.gz
cd gvars-3.0
./configure --disable-widgets
make
make install
cd ../../

rm -rf mcptam_deps

#!/bin/bash
cd third_party/local
PREFIX_PATH=`pwd`
cd ../glog
./configure --prefix=$PREFIX_PATH
make install
make clean

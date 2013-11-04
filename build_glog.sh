#!/bin/bash
if [ ! -d third_party/local ]
then
    mkdir third_party/local
fi

cd third_party/local
PREFIX_PATH=`pwd`

if [ ! -d third_party/glog]
then
    svn checkout http://google-glog.googlecode.com/svn/trunk/ glog
    cd glog
    ./configure --prefix=$PREFIX_PATH
    make install
    make clean
fi


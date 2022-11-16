#!/bin/bash

export Boost_INCLUDE_DIR=${BOOST_INC}
export Boost_LIBRARIES=${BOOST_LIB}
export LCC2=TRUE

cd ./build
cmake ..
make

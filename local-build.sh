#!/bin/bash
mkdir -p build && cd build
cmake .. -DENABLE_SANITIZERS=ON
make

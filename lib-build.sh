#!/bin/bash
cmake -B build -DCMAKE_INSTALL_PREFIX=$HOME/.local -DENABLE_SANITIZERS=OFF
cmake --build build --target install
#!/bin/bash
cd ../build
cmake -DCMAKE_BUILD_TYPE=Debug -Dtest=ON ..
make
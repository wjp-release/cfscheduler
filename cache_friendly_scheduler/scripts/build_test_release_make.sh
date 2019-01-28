#!/bin/bash
cd ../build
cmake -DCMAKE_BUILD_TYPE=Release -Dtest=ON ..
make
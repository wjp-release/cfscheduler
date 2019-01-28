#!/bin/bash
cd ../build
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug -Dtest=ON ..
ninja
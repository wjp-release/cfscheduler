#!/bin/bash
cd ../build
cmake -GNinja -DCMAKE_PREFIX_PATH=/usr/local/tbb/ -DCMAKE_BUILD_TYPE=Debug -Dbench=ON -Dtest=ON ..
ninja
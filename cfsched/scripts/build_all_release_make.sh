#!/bin/bash
cd ../build
cmake -DCMAKE_PREFIX_PATH=/usr/local/tbb/ -DCMAKE_BUILD_TYPE=Release -Dbench=ON -Dtest=ON ..
make
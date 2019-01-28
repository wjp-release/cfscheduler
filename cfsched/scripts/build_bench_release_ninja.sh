#!/bin/bash
cd ../build
cmake -GNinja -DCMAKE_PREFIX_PATH=/usr/local/tbb/ -DCMAKE_BUILD_TYPE=Release -Dbench=ON ..
ninja
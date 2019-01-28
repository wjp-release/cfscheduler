#!/bin/bash
cd ../build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -Dtest=ON ..
ninja
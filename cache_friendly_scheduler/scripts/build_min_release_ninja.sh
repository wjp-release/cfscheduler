#!/bin/bash
cd ../build
cmake -GNinja -DCMAKE_BUILD_TYPE=Release  ..
ninja

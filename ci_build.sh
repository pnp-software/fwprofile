#!/usr/bin/env bash

set -x

if [ $BUILD_TYPE == "default" ]; then
    mkdir tmp
    BUILD_PREFIX=$PWD/tmp

    CONFIG_OPTS=()
    
    # Build, check, and install fwprofile from local source
    (./autogen.sh && ./configure "${CONFIG_OPTS[@]}" && make check-verbose VERBOSE=1 && make install) || exit 1
else
    cd ./builds/${BUILD_TYPE} && ./ci_build.sh
fi

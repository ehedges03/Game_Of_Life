#!/bin/bash

scriptDir=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")

pushd $scriptDir/..
Vendor/Binaries/Premake/Linux/premake5 --cc=clang gmake2
popd

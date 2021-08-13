#!/bin/sh
apiInfoFile=$(mktemp)
buildDir="./nvimClient"
includeDir="./include"
nvim --api-info > $apiInfoFile
mkdir -p $buildDir
cp -r $includeDir/* $buildDir/
node ./src/index.js $apiInfoFile > ./$buildDir/impl/Client.hpp
find ./$buildDir/ -name "*.hpp" | xargs clang-format -style="{Language: Cpp, BasedOnStyle: LLVM, ColumnLimit: 120}" -i

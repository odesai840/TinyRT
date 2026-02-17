@echo off

pushd ..
git submodule update --init --recursive
cmake -S . -B build
cmake --build build
popd
pause

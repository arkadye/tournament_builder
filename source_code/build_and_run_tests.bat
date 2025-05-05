set TARGET="Release"

if [%1] neq [] set TARGET=%1

pushd .

cd %~dp0

cmake .
cmake --build . --config %TARGET%
ctest . -C %TARGET%

popd
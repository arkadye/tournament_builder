set TARGET="Release"

if [%1] neq [] set TARGET=%1

pushd .

cd %~dp0/..

cmake -D DEV_MODE=On .
cmake --build . --config %TARGET%
ctest . -C %TARGET%

popd
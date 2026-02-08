setlocal
set TARGET=Release

if [%1] neq [] set TARGET=%1

pushd .

cd %~dp0\..

cmake .
cmake --build . --config %TARGET%
documentation_builder\%TARGET%\document_builder.exe --input documentation_builder\documentation_src --output ..
SET /P DOFORREAL="This will edit the above files. Do you want to continue (Y/[N])?"

IF /I %DOFORREAL% NEQ "Y" GOTO END

documentation_builder\%TARGET%\document_builder.exe --input documentation_builder\documentation_src --output .. --real

:END
popd
endlocal
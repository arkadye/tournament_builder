setlocal
echo "CALLED clean_workspace.bat arg = [%1]"
if [%1]==[] goto NOARG

set TARGET=%1
echo "Running clean on %TARGET%"

pushd .
cd %TARGET%

del *.vcxproj
del *.vcxproj.*
del *.vsproj
del *.vsproj.*
del *.csproj
del *.sln
del *.slnx

rmdir /S/Q .vs
rmdir /S/Q _build
rmdir /S/Q _intermediate
rmdir /S/Q _out


popd
goto FINISH

:NOARG
echo "No argument found. Using local directory (%~dp0) and running resursively."
pushd .
cd %~dp0
call clean_workspace.bat %~dp0
call clean_workspace.bat %~dp0/cli_app
call clean_workspace.bat %~dp0/cli_lib
call clean_workspace.bat %~dp0/cli_dbg
call clean_workspace.bat %~dp0/clib
call clean_workspace.bat %~dp0/cpplib
call clean_workspace.bat %~dp0/documentation_builder
call clean_workspace.bat %~dp0/dotnet_dll
call clean_workspace.bat %~dp0/dotnet_gui
call clean_workspace.bat %~dp0/scripts
popd 
goto FINISH



:FINISH
echo "FINISHED clean_workspace.bat arg = [%1]"
endlocal
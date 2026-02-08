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
del *.sln
del *.slnx
del cmake_install.cmake
del CMakeCache.txt
del *.cmake
del *.tcl

rmdir /S/Q .vs
rmdir /S/Q x64
rmdir /S/Q CMakeFiles
rmdir /S/Q Debug
rmdir /S/Q Release
rmdir /S/Q MinSizeRel
rmdir /S/Q Testing
rmdir /S/Q RelWithDebInfo
rmdir /S/Q ALL_BUILD.dir
rmdir /S/Q Continuous.dir
rmdir /S/Q Experimental.dir
rmdir /S/Q INSTALL.dir
rmdir /S/Q Nightly.dir
rmdir /S/Q NightlyMemoryCheck.dir
rmdir /S/Q RUN_TESTS.dir
rmdir /S/Q ZERO_CHECK.dir
rmdir /S/Q document_builder.dir
rmdir /S/Q tournament_builder_clib.dir
rmdir /S/Q tournament_builder_cpplib.dir
rmdir /S/Q tournament_builder_cli.dir
rmdir /S/Q tournament_builder_cli_lib.dir
rmdir /S/Q tournament_builder_dbg.dir


popd
goto FINISH

:NOARG
echo "No argument found. Using local directory (%~dp0) and running resursively."
pushd .
cd %~dp0
call clean_workspace.bat %~dp0
call clean_workspace.bat %~dp0/cli
call clean_workspace.bat %~dp0/cli_dbg
call clean_workspace.bat %~dp0/clib
call clean_workspace.bat %~dp0/cpplib
call clean_workspace.bat %~dp0/cpplib
call clean_workspace.bat %~dp0/documentation_builder
call clean_workspace.bat %~dp0/mono
call clean_workspace.bat %~dp0/scripts
popd 
goto FINISH



:FINISH
echo "FINISHED clean_workspace.bat arg = [%1]"
endlocal
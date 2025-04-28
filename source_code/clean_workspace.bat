pushd .
cd @%~dp0
del *.vcxproj
del *.vcxproj.*
del *.sln
del cmake_install.cmake
del CMakeCache.txt
rmdir /S/Q .vs
rmdir /S/Q x64
rmdir /S/Q CMakeFiles
rmdir /S/Q Debug
rmdir /S/Q Release
rmdir /S/Q MinSizeRel
rmdir /S/Q RelWithDebInfo

cli/clean_workspace.bat
cli_dbg/clean_workspace.bat
clib/clean_workspace.bat
cpplib/clean_workspace.bat
mono/clean_workspace.bat
popd
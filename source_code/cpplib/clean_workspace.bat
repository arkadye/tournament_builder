pushd .
cd %~dp0
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
rmdir /S/Q tournament_builder_cpplib.dir
popd
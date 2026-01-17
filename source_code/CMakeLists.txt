cmake_minimum_required(VERSION 3.22.0)
project(tournament_builder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CTest)
enable_testing()

add_subdirectory(cpplib)
add_subdirectory(clib)
add_subdirectory(cli)
add_subdirectory(cli_dbg)
add_subdirectory(mono)
add_subdirectory(documentation_builder)
# GLOBAL CMAKE

cmake_minimum_required(VERSION 3.31)

set(PROJECTNAME tournament_builder)
project(${PROJECTNAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(static_library)
add_subdirectory(cli)
add_subdirectory(cli_dbg)
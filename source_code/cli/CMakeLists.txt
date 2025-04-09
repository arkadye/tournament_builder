# CLI CMAKE
cmake_minimum_required(VERSION 3.31)

set(PROJECTNAME tournament_builder_cli)
set(LIBNAME tournament_builder_cli_lib)
project(${PROJECTNAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("../include" "../thirdparty/cxxopts/cxxopts/include" "../thirdparty/nlohmann/json/single_include")

set(MAIN_FILE "../src/cli/cli_main.cpp")
set(INVOKE_HEADER "../include/cli/invoke_cli.h")
set(INVOKE_SOURCE "../src/cli/invoke_cli.cpp")

#$<TARGET_OBJECTS:${LIBNAME}>
add_executable(${PROJECTNAME} ${MAIN_FILE} $<TARGET_OBJECTS:tournament_builder_static_lib> ${INVOKE_SOURCE})

source_group("src" FILES ${MAIN_FILE} ${INVOKE_HEADER} ${INVOKE_SOURCE})

target_sources(${PROJECTNAME} PUBLIC ${INVOKE_HEADER} ${INVOKE_SOURCE})
target_sources(${PROJECTNAME} PRIVATE ${MAIN_FILE})
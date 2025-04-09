# This invokes the CLI versions, but is entirely controled from code.
# This makes running in a debugger much easier, becuase there's no need to faff
# around with settings. Just put them all in there.

cmake_minimum_required(VERSION 3.31)

set(PROJECTNAME tournament_builder_dbg)
project(${PROJECTNAME})

include_directories("../include" "../thirdparty/cxxopts/cxxopts/include"  "../thirdparty/nlohmann/json/single_include")

set(MAINFILE "../src/cli_dbg/dbg_main.cpp")
set(INVOKE_HEADER "../include/cli/invoke_cli.h")
set(INVOKE_SOURCE "../src/cli/invoke_cli.cpp")

add_executable(${PROJECTNAME} ${MAINFILE} $<TARGET_OBJECTS:tournament_builder_static_lib>)

source_group("src" ${MAINFILE})
source_group("src/external" ${INVOKE_HEADER} ${INVOKE_SOURCE})

file(GLOB EXAMPLE_INPUT_FILES "../../examples/in/*.json")
file(GLOB EXAMPLE_OUTPUT_FILES "../../examples/out/*.json")

set(EXAMPLE_FILES ${EXAMPLE_INPUT_FILES} ${EXAMPLE_OUTPUT_FILES})

source_group("examples/inputs" FILES ${EXAMPLE_INPUT_FILES})
source_group("examples/outputs" FILES ${EXAMPLE_OUTPUT_FILES})

target_sources(${PROJECTNAME} PRIVATE ${MAINFILE} ${INVOKE_HEADER} ${INVOKE_SOURCE} ${EXAMPLE_FILES})
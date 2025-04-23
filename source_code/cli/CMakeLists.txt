cmake_minimum_required(VERSION 3.22.0)
set(CLIEXE tournament_builder_cli)
project(${CLIEXE})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CLI_MAINFILE "src/cli_main.cpp")
set(INVOKE_HEADER "include/invoke_cli.h")
set(INVOKE_SOURCE "src/invoke_cli.cpp")

set(CLILIB tournament_builder_cli_lib)
add_library(${CLILIB} STATIC ${INVOKE_SOURCE})
add_executable(${CLIEXE} ${CLI_MAINFILE} $<TARGET_OBJECTS:${CPPLIB}> $<TARGET_OBJECTS:${CLILIB}>)

#TODO: Remove dependency on nlohmann/json
set(INCLUDE_DIRS "include" "../cpplib/include" "../thirdparty/nlohmann/json/single_include" "../thirdparty/cxxopts/cxxopts/include")
target_include_directories(${CLILIB} PRIVATE ${INCLUDE_DIRS})
target_include_directories(${CLIEXE} PRIVATE ${INCLUDE_DIRS})

source_group("src" FILES ${CLI_MAINFILE} ${INVOKE_HEADER} ${INVOKE_SOURCE})

target_sources(${CLIEXE} PRIVATE ${INVOKE_HEADER})
target_sources(${CLIEXE} PRIVATE ${CLI_MAINFILE})

set(CLILIB tournament_builder_cli_lib PARENT_SCOPE)

# Install instructions below.

set(INSTALL_TARGET CLI)
install(TARGETS ${CLIEXE} DESTINATION ${INSTALL_TARGET})
file(GLOB PACKAGE_EXTRAS package_extras/*)
install(FILES ${PACKAGE_EXTRAS} DESTINATION ${INSTALL_TARGET})
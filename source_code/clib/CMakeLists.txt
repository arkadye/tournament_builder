cmake_minimum_required(VERSION 3.31)

set(CINTERFACELIB tournament_builder_clib)
project(${CINTERFACELIB})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# TODO: Set things up so this does not need the nlohmann/json include.
include_directories("include" "../cpplib/include" "../thirdparty/nlohmann/json/single_include" "../thirdparty/cxxopts/cxxopts/include")

set(CINTERFACE_HEADER "include/tournament_builder.h")
set(CINTERFACE_SOURCE "src/tournament_builder_clib.cpp")
set(C_FILES ${CINTERFACE_HEADER} ${CINTERFACE_SOURCE})
set(CINTERFACE_BUILD_SOURCES ${CINTERFACE_SOURCE} $<TARGET_OBJECTS:${CPPLIB}>)
add_library(${CINTERFACELIB} SHARED ${CINTERFACE_BUILD_SOURCES})
source_group("src" FILES ${C_FILES})
target_sources(${CINTERFACELIB} PRIVATE ${CINTERFACE_SOURCE})
target_sources(${CINTERFACELIB} PUBLIC ${CINTERFACE_HEADER})

target_compile_definitions(${CINTERFACELIB} PRIVATE TOURNAMENT_BUILDER_ENABLE_EXPORT_TO_DLL)

set(CINTERFACELIB tournament_builder_clib PARENT_SCOPE)
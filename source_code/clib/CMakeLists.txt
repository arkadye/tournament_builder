cmake_minimum_required(VERSION 3.22.0)
set(CINTERFACELIB tournament_builder_clib)
project(${CINTERFACELIB})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DEV_MODE)
	set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CINTERFACE_SOURCE "src/tournament_builder_clib.cpp")
set(CINTERFACE_EXPORT_HEADER "./include/tournament_builder/clib_export.h")
set(CINTERFACE_HEADER "include/tournament_builder.h")
set(C_FILES ${CINTERFACE_HEADER} ${CINTERFACE_SOURCE})
set(CINTERFACE_BUILD_SOURCES ${CINTERFACE_SOURCE} $<TARGET_OBJECTS:${CPPLIB}>)
add_library(${CINTERFACELIB} SHARED ${CINTERFACE_BUILD_SOURCES})

include(GenerateExportHeader)
generate_export_header(${CINTERFACELIB} EXPORT_FILE_NAME ${CINTERFACE_EXPORT_HEADER})

# TODO: Set things up so this does not need the nlohmann/json include.
target_include_directories(${CINTERFACELIB} PRIVATE "include" "../cpplib/include" "../thirdparty/nlohmann/json/single_include" "../thirdparty/cxxopts/cxxopts/include")

source_group("src" FILES ${C_FILES})
target_sources(${CINTERFACELIB} PRIVATE ${CINTERFACE_SOURCE})
target_sources(${CINTERFACELIB} PRIVATE ${CINTERFACE_HEADER} ${CINTERFACE_EXPORT_HEADER})

#target_compile_definitions(${CINTERFACELIB} PRIVATE TOURNAMENT_BUILDER_ENABLE_EXPORT_TO_DLL)

set(CINTERFACELIB tournament_builder_clib PARENT_SCOPE) 

set(INSTALL_TARGET c_library)
install(TARGETS ${CINTERFACELIB} DESTINATION "${INSTALL_TARGET}/lib")
install(FILES ${CINTERFACE_HEADER} DESTINATION "${INSTALL_TARGET}/include")
install(FILES ${CINTERFACE_EXPORT_HEADER} DESTINATION "${INSTALL_TARGET}/include/tournament_builder")
install(TARGETS ${CINTERFACELIB} RUNTIME DESTINATION "mono")

file(GLOB PACKAGE_EXTRAS package_extras/*)
install(FILES ${PACKAGE_EXTRAS} DESTINATION "${INSTALL_TARGET}")
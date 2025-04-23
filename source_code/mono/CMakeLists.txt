cmake_minimum_required(VERSION 3.22.0)
set(MANAGEDDLL tournament_builder_mono)
project(${MANAGEDDLL} CSharp)

set(MONO_SOURCE "src/tournament_builder_mono.cs")

#add_executable(${MANAGEDDLL} ${MONO_SOURCE})
add_library(${MANAGEDDLL} SHARED ${MONO_SOURCE})
set_target_properties(${MANAGEDDLL} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
target_link_libraries(${MANAGEDDLL} PRIVATE ${CINTERFACELIB})

target_sources(${MANAGEDDLL} PRIVATE ${MONO_SOURCE})
target_include_directories(${MANAGEDDLL} PRIVATE "../clib/tournament_builder.h")
source_group("src" ${MONO_SOURCE})

set(INSTALL_TARGET mono)
install(TARGETS ${MANAGEDDLL} DESTINATION ${INSTALL_TARGET})
install(IMPORTED_RUNTIME_ARTIFACTS  ${MANAGEDDLL} DESTINATION ${INSTALL_TARGET})

file(GLOB PACKAGE_EXTRAS package_extras/*)
install(FILES ${PACKAGE_EXTRAS} DESTINATION ${INSTALL_TARGET})
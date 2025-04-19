set(MANAGEDDLL tournament_builder_mono)
project(${MANAGEDDLL} CSharp)

set(MONO_SOURCE "src/tournament_builder_mono.cs")


#add_executable(${MANAGEDDLL} ${MONO_SOURCE})
add_library(${MANAGEDDLL} SHARED ${MONO_SOURCE})
set_target_properties(${MANAGEDDLL} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
target_link_libraries(${MANAGEDDLL} PRIVATE ${CINTERFACELIB})

target_sources(${MANAGEDDLL} PRIVATE ${MONO_SOURCE})
source_group("src" ${MONO_SOURCE})
source_group("template" ${MONO_SOURCE_TEMPLATE})
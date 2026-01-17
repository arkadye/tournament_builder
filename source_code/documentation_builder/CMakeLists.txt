cmake_minimum_required(VERSION 3.22.0)
set(DOCUMENT_BUILDER document_builder)
project(${DOCUMENT_BUILDER})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DEV_MODE)
	set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(INCLUDE_DIRS "../thirdparty/cxxopts/cxxopts/include")
set(DOCUMENT_BUILDER_MAINFILE "src/main.cpp")

add_executable(${DOCUMENT_BUILDER} ${DOCUMENT_BUILDER_MAINFILE})

set(TOP_DOCS_FILES "documentation_src/README.md")
source_group("docs" FILES ${TOP_DOCS_FILES})

set(DOCS_FILES "documentation_src/documentation/readme.md")
source_group("docs/documentation" FILES ${DOCS_FILES})

set(DOCS_JSON_FILES
	"documentation_src/documentation/json/competitor.md"
	"documentation_src/documentation/json/entry_list.md"
	"documentation_src/documentation/json/error_messages.md"
	"documentation_src/documentation/json/naming_rules.md"
	"documentation_src/documentation/json/readme.md"
	"documentation_src/documentation/json/references.md"
	"documentation_src/documentation/json/tags.md"
	"documentation_src/documentation/json/world.md"
	"documentation_src/documentation/json/competition.md")
source_group("docs/documentation/json" FILES ${DOCS_JSON_FILES})

set(DOCS_JSON_DESCRIPTORS_FILES
	"documentation_src/documentation/json/descriptors/knockout_bracket.md"
	"documentation_src/documentation/json/descriptors/readme.md"
	"documentation_src/documentation/json/descriptors/round_of_matches.md"
	"documentation_src/documentation/json/descriptors/round_robin.md"
	"documentation_src/documentation/json/descriptors/competitor_view.md"
	"documentation_src/documentation/json/descriptors/DEBUG_never_resolves.md")
source_group("docs/documentation/json/descriptors" FILES ${DOCS_JSON_DESCRIPTORS_FILES})

set(DOCS_JSON_EVENTS_FILES
	"documentation_src/documentation/json/events/resolve_reference.md"
	"documentation_src/documentation/json/events/set_finishing_positions.md"
	"documentation_src/documentation/json/events/add_tags.md"
	"documentation_src/documentation/json/events/readme.md"
	"documentation_src/documentation/json/events/resolve_all_references.md")
source_group("docs/documentation/json/events" FILES ${DOCS_JSON_EVENTS_FILES})

set(ALL_DOCS ${TOP_DOCS_FILES} ${DOCS_FILES} ${DOCS_JSON_FILES} ${DOCS_JSON_DESCRIPTORS_FILES} ${DOCS_JSON_EVENTS_FILES})

source_group("src" FILES ${DOCUMENT_BUILDER_MAINFILE})
target_include_directories(${DOCUMENT_BUILDER} PRIVATE ${INCLUDE_DIRS})
target_sources(${DOCUMENT_BUILDER} PRIVATE ${DOCUMENT_BUILDER_MAINFILE} ${ALL_DOCS})
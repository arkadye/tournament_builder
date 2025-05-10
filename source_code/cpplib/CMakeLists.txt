cmake_minimum_required(VERSION 3.22.0)
set(CPPLIB tournament_builder_cpplib)
project(${CPPLIB})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DEV_MODE)
	set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(SOURCE_FILES
	"src/tournament_builder.cpp"
	"src/tournament_builder_native.cpp"
	"src/tournament_builder_json_obj.cpp"
)

set(SOURCE_HEADERS
	"include/tournament_builder.hpp"
	"include/tournament_builder_native.hpp"
	"include/tournament_builder_json_obj.hpp"
)

set(TYPE_SOURCES
	"src/tournament_builder/name.cpp"
	"src/tournament_builder/competitor.cpp"
	"src/tournament_builder/competition.cpp"
	"src/tournament_builder/exceptions.cpp"
	"src/tournament_builder/random.cpp"
	"src/tournament_builder/reference.cpp"
	"src/tournament_builder/ireferencable.cpp"
	"src/tournament_builder/tag.cpp"
	"src/tournament_builder/itaggable.cpp"
	"src/tournament_builder/token.cpp"
	"src/tournament_builder/stats.cpp"
	"src/tournament_builder/world.cpp"
)

set(TYPE_HEADERS
	"include/tournament_builder/name.hpp"
	"include/tournament_builder/competitor.hpp"
	"include/tournament_builder/competition.hpp"
	"include/tournament_builder/exceptions.hpp"
	"include/tournament_builder/random.hpp"
	"include/tournament_builder/reference.hpp"
	"include/tournament_builder/ireferencable.hpp"
	"include/tournament_builder/stats.hpp"
	"include/tournament_builder/tag.hpp"
	"include/tournament_builder/itaggable.hpp"
	"include/tournament_builder/token.hpp"
	"include/tournament_builder/world.hpp"
	"include/tournament_builder/generic_utils.hpp"
)
set(TYPES ${TYPE_SOURCES} ${TYPE_HEADERS})
	
set(DESCRIPTOR_HEADERS
	"include/tournament_builder/descriptors/descriptor_handle.hpp"
	"include/tournament_builder/descriptors/descriptor_base.hpp"
	"include/tournament_builder/descriptors/descriptor_base_internal.hpp"
	"include/tournament_builder/descriptors/round_robin_descriptor.hpp"
	"include/tournament_builder/descriptors/round_of_matches_descriptor.hpp"
	"include/tournament_builder/descriptors/knockout_bracket.hpp"
	"include/tournament_builder/descriptors/DEBUG_never_resolves.hpp"
	"include/tournament_builder/descriptors/competitor_view.hpp"
)

set(DESCRIPTOR_SOURCES

	"src/tournament_builder/descriptors/descriptor_base.cpp"
	"src/tournament_builder/descriptors/descriptor_base_internal.cpp"
	"src/tournament_builder/descriptors/round_robin_descriptor.cpp"
	"src/tournament_builder/descriptors/round_of_matches_descriptor.cpp"
	"src/tournament_builder/descriptors/knockout_bracket.cpp"
	"src/tournament_builder/descriptors/DEBUG_never_resolves.cpp"
	"src/tournament_builder/descriptors/competitor_view.cpp"
)
source_group("src/descriptors" FILES ${DESCRIPTOR_HEADERS} ${DESCRIPTOR_SOURCES})

set(EVENT_HEADERS
	"include/tournament_builder/events/event_base_impl.hpp"
	"include/tournament_builder/events/event_base.hpp"
	"include/tournament_builder/events/resolve_references_event.hpp"
	"include/tournament_builder/events/set_finishing_positions_event.hpp"
	"include/tournament_builder/events/add_tags_event.hpp"
)

set(EVENT_SOURCES
	"src/tournament_builder/events/event_base_impl.cpp"
	"src/tournament_builder/events/event_base.cpp"
	"src/tournament_builder/events/resolve_references_event.cpp"
	"src/tournament_builder/events/set_finishing_positions_event.cpp"
	"src/tournament_builder/events/add_tags_event.cpp"
)
source_group("src/events" FILES ${EVENT_HEADERS} ${EVENT_SOURCES})

set (JSON_SOURCES
	"src/tournament_builder/json/json_helpers.cpp"
	"src/tournament_builder/serialize.cpp"
)
set(JSON_HEADERS
	"include/tournament_builder/json/json_helpers.hpp"
	"include/tournament_builder/serialize.hpp"
)
set(JSON_FILES ${JSON_SOURCES} ${JSON_HEADERS})
	
set(NLOHMANN_NATVIS "../thirdparty/nlohmann/json/nlohmann_json.natvis")

set(LIB_FILES ${SOURCE_FILES} ${DESCRIPTOR_SOURCES} ${JSON_SOURCES} ${EVENT_SOURCES} ${TYPE_SOURCES} ${TYPE_HEADERS} ${DESCRIPTOR_HEADERS} ${EVENT_HEADERS} ${SOURCE_HEADERS}  ${JSON_HEADERS})
set(HEADER_FILES ${SOURCE_HEADERS})

source_group("src/json" FILES ${JSON_FILES})
source_group("src/descriptors" FILES ${DESCRIPTORS})
source_group("src/types" FILES ${TYPES})
source_group("src" FILES ${SOURCE_FILES} ${SOURCE_HEADERS})

add_library(${CPPLIB} STATIC ${LIB_FILES})

target_sources(${CPPLIB} PRIVATE ${LIB_FILES})
target_sources(${CPPLIB} PUBLIC ${HEADER_FILES})
if(EXISTS ${NLOHMANN_NATVIS})
target_sources(${CPPLIB} PRIVATE ${NLOHMANN_NATVIS})
endif()
target_include_directories(${CPPLIB} PRIVATE "include" "../thirdparty/nlohmann/json/single_include")

set(CPPLIB tournament_builder_cpplib PARENT_SCOPE)
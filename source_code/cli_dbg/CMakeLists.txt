set(DBG_NAME tournament_builder_dbg)
project(${DBG_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(DEV_MODE)
	set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
endif()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DBG_MAINFILE "src/dbg_main.cpp")

add_executable(${DBG_NAME} ${DBG_MAINFILE} $<TARGET_OBJECTS:${CPPLIB}> $<TARGET_OBJECTS:${CLILIB}>)

target_include_directories(${DBG_NAME} PRIVATE "include" "../cli/include" "thirdparty/nlohmann/json/single_include")

source_group("src" ${DBG_MAINFILE})

cmake_path(SET EXAMPLES_BASE_PATH NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/../../examples")
message(STATUS "Examples full path: ${EXAMPLES_BASE_PATH}")
cmake_path(RELATIVE_PATH EXAMPLES_BASE_PATH BASE_DIRECTORY	${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "Examples relative path: ${EXAMPLES_BASE_PATH}")

function(add_example_file filename)
	cmake_path(SET BASE_PATH ${EXAMPLES_BASE_PATH})
	cmake_path(SET INPUT_PATH "${BASE_PATH}/in/${filename}")
	cmake_path(SET OUTPUT_PATH "${BASE_PATH}/out/${filename}")
	set(EXAMPLE_INPUT_FILES ${EXAMPLE_INPUT_FILES} ${INPUT_PATH} PARENT_SCOPE)
	set(EXAMPLE_OUTPUT_FILES ${EXAMPLE_OUTPUT_FILES} ${OUTPUT_PATH} PARENT_SCOPE)
endfunction()

add_example_file("add_tags.json")
add_example_file("complex_elim.json")
add_example_file("round_of_matches.json")
add_example_file("round_robin.json")
add_example_file("set_finishing_positions.json")
add_example_file("simple_elim.json")
add_example_file("test_references.json")
add_example_file("DEBUG_never_resolves.json")
add_example_file("test_multireferences.json")

set(EXAMPLE_FILES ${EXAMPLE_INPUT_FILES} ${EXAMPLE_OUTPUT_FILES})

source_group("examples/inputs" FILES ${EXAMPLE_INPUT_FILES})
source_group("examples/outputs" FILES ${EXAMPLE_OUTPUT_FILES})

target_sources(${DBG_NAME} PRIVATE ${MAINFILE} ${INVOKE_SOURCE} ${EXAMPLE_FILES})
target_compile_definitions(${DBG_NAME} PRIVATE EXAMPLES_FOLDER_DEFAULT_LOCATION="${EXAMPLES_BASE_PATH}")
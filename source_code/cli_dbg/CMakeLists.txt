set(DBG_NAME tournament_builder_dbg)
project(${DBG_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(DBG_MAINFILE "src/dbg_main.cpp")

add_executable(${DBG_NAME} ${DBG_MAINFILE} $<TARGET_OBJECTS:${CPPLIB}> $<TARGET_OBJECTS:${CLILIB}>)

target_include_directories(${DBG_NAME} PRIVATE "include" "../cli/include" "thirdparty/nlohmann/json/single_include")

source_group("src" ${DBG_MAINFILE})

function(add_example_file filename)
	set(BASE_PATH "../../examples")
	set(INPUT_PATH "${BASE_PATH}/in/${filename}")
	set(OUTPUT_PATH "${BASE_PATH}/out/${filename}")
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
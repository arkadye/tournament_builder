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
	cmake_path(SET INPUT_PATH "${BASE_PATH}/in/${filename}.json")
	cmake_path(SET OUTPUT_PATH "${BASE_PATH}/out/${filename}.json")
	set(EXAMPLE_INPUT_FILES ${EXAMPLE_INPUT_FILES} ${INPUT_PATH} PARENT_SCOPE)
	set(EXAMPLE_OUTPUT_FILES ${EXAMPLE_OUTPUT_FILES} ${OUTPUT_PATH} PARENT_SCOPE)
endfunction()

function(add_error_example_file filename)
	cmake_path(SET BASE_PATH ${EXAMPLES_BASE_PATH})
	cmake_path(SET INPUT_PATH "${BASE_PATH}/in/errors/${filename}.json")
	cmake_path(SET OUTPUT_PATH "${BASE_PATH}/out/errors/${filename}.json")
	set(EXAMPLE_ERROR_INPUT_FILES ${EXAMPLE_ERROR_INPUT_FILES} ${INPUT_PATH} PARENT_SCOPE)
	set(EXAMPLE_ERROR_OUTPUT_FILES ${EXAMPLE_ERROR_OUTPUT_FILES} ${OUTPUT_PATH} PARENT_SCOPE)
endfunction()

add_example_file("add_tags")
add_example_file("complex_elim")
add_example_file("round_of_matches")
add_example_file("round_robin")
add_example_file("set_finishing_positions")
add_example_file("simple_elim")
add_example_file("test_references")
add_example_file("DEBUG_never_resolves")
add_example_file("test_multireferences")
add_example_file("test_tags")

add_error_example_file("ERROR_invalid_competitor_view_1")
add_error_example_file("ERROR_invalid_competitor_view_2")
add_error_example_file("ERROR_invalid_competitor_view_3")
add_error_example_file("ERROR_invalid_descriptor_type")
add_error_example_file("ERROR_mismatches_brackets")
add_error_example_file("ERROR_knockout_bracket_wrong_entry_list_size")
add_error_example_file("ERROR_round_robin_invalid_times_played")
add_error_example_file("ERROR_round_robin_entry_list_has_byes")
add_error_example_file("ERROR_event_type_does_not_exist")
add_error_example_file("ERROR_set_finishing_positions_non_name_type")
add_error_example_file("ERROR_set_finishing_positions_overly_nested")
add_error_example_file("ERROR_set_finishing_positions_reference_in_entry_list")
add_error_example_file("ERROR_set_finishing_positions_not_all_entries_placed")
add_error_example_file("ERROR_set_finishing_positions_place_element_not_on_entry_list")
add_error_example_file("ERROR_missing_fields")	
add_error_example_file("ERROR_empty_name")	
add_error_example_file("ERROR_invalid_name")
add_error_example_file("ERROR_reference_outer_too_many_args")
add_error_example_file("ERROR_reference_outer_non_numeric_arg")
add_error_example_file("ERROR_reference_outer_non_positive_arg")
add_error_example_file("ERROR_reference_outer_from_top_level")
add_error_example_file("ERROR_reference_any_min_greater_than_max")
add_error_example_file("ERROR_reference_any_negative_max_arg")
add_error_example_file("ERROR_reference_any_negative_min_arg")
add_error_example_file("ERROR_reference_any_non_numeric_arg")
add_error_example_file("ERROR_reference_any_too_many_args")
add_error_example_file("ERROR_reference_invalid_special_token")
add_error_example_file("ERROR_invalid_format")


set(EXAMPLE_FILES ${EXAMPLE_INPUT_FILES} ${EXAMPLE_OUTPUT_FILES})

source_group("examples/inputs" FILES ${EXAMPLE_INPUT_FILES})
source_group("examples/outputs" FILES ${EXAMPLE_OUTPUT_FILES})

target_sources(${DBG_NAME} PRIVATE ${MAINFILE} ${INVOKE_SOURCE} ${EXAMPLE_FILES})
target_compile_definitions(${DBG_NAME} PRIVATE EXAMPLES_FOLDER_DEFAULT_LOCATION="${EXAMPLES_BASE_PATH}")

set(EXAMPLE_ERROR_FILES ${EXAMPLE_ERROR_INPUT_FILES} ${EXAMPLE_ERROR_OUTPUT_FILES})

source_group("examples/inputs_errors" FILES ${EXAMPLE_ERROR_INPUT_FILES})
source_group("examples/outputs_errors" FILES ${EXAMPLE_ERROR_OUTPUT_FILES})

target_sources(${DBG_NAME} PRIVATE ${MAINFILE} ${INVOKE_SOURCE} ${EXAMPLE_FILES} ${EXAMPLE_ERROR_FILES})

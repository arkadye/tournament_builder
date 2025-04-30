cmake_minimum_required(VERSION 3.22.0)
project(tournament_builder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CTest)

add_subdirectory(cpplib)
add_subdirectory(clib)
add_subdirectory(cli)
add_subdirectory(cli_dbg)
add_subdirectory(mono)

# Testing
enable_testing()
function(test_example_file filename)
	set(BASE_PATH "../examples")
	set(INPUT_PATH "${BASE_PATH}/in/${filename}.json")
	set(OUTPUT_PATH "${BASE_PATH}/out/${filename}.json")
	add_test(NAME ${filename} COMMAND "$<TARGET_FILE:${CLIEXE}>" "--input" "${INPUT_PATH}" "--test-reference" "${OUTPUT_PATH}")
endfunction()

test_example_file("add_tags")
test_example_file("complex_elim")
test_example_file("DEBUG_never_resolves")
test_example_file("round_of_matches")
test_example_file("round_robin")
test_example_file("set_finishing_positions")
test_example_file("simple_elim")
test_example_file("test_references")
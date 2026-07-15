--- cli_lib

project "cli_lib"
	targetname "tournament_builder_cli_lib"
	language "C++"
	kind "StaticLib"
	includedirs { "../thirdparty/cxxopts/cxxopts/include" , "../cpplib/include" , "../thirdparty/nlohmann/json/include" } -- TODO remove nlohmann JSON dependency here
	files { "thirdparty/nlohmann/json/nlohmann_json.natvis" }
	location "_build"
	links { "cpplib" }
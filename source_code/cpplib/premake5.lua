--- cpplib

project "cpplib"
	targetname "tournament_builder_cpp_lib"
	language "C++"
	kind "StaticLib"
	includedirs { "../thirdparty/nlohmann/json/include" }
	files { "../thirdparty/nlohmann/json/nlohmann_json.natvis" }
	location "cpplib/_build"
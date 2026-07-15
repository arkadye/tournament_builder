--- clib

project "clib"
	targetname "tournament_builder_clib"
	language "C"
	kind "SharedLib"
	includedirs { "../cpplib/include" , "../thirdparty/nlohmann/json/include"} -- TODO remove nlohmann JSON dependency here
	defines { "IS_EXPORTING_TOURNAMENT_BUILDER_CLIB" }
	location "_build"
	links { "cpplib" }
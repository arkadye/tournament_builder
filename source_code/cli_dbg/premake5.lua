--- cli_dbg

project "cli_dbg"
	targetname "tournament_builder_clidbg"
	language "C++"
	kind "ConsoleApp"
	includedirs {"../cli_lib/include"}
	files { "../../examples/**.json" , "../../example_templates/**.json" }
	location "_build"
	links {"cli_lib"}
--- cli_app

project "cli_app"
	targetname "tournament_builder_cli"
	language "C++"
	kind "ConsoleApp"
	includedirs {"../cli_lib/include"}
	location "_build"
	links {"cli_lib" }
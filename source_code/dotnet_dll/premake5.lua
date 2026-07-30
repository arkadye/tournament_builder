--- dotnet_dll

project "dotnet_dll"
	targetname "tournament_builder_dotnet"
	language "C#"
	kind "SharedLib"
	links {"clib"}
	copylocal{"clib"}
	location "_build"
		
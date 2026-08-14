--- dotnet_dll

project "dotnet_dll"
	targetname "tournament_builder_dotnet"
	language "C#"
	kind "SharedLib"
	prebuildcommands {
		"python3 %{!wks.location}/scripts/python/build.py project clib config release platform x86",
		"{COPYFILE} %[%{!wks.location}]clib/_out/Release/x86/tournament_builder_clib.dll %[%{!cfg.buildtarget.directory}]"
	}
	location "_build"
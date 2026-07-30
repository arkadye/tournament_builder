--- Tournament Builder premake5.lua

workspace "tournament_builder"
	configurations {"Debug" , "Release" }
	includedirs { "%{prj.name}/include" }
	platforms{ "x86" , "x64" , "Any CPU" }
	targetdir "%{prj.name}/_out/%{cfg.buildcfg}"
	objdir "%{prj.name}/_intermediate/%{cfg.buildcfg}"
	startproject "cli_dbg"
	fatalwarnings "All"
	
	files { "%{prj.name}/include/**.h", "%{prj.name}/include/**.hpp" , "%{prj.name}/src/**.cpp" , "%{prj.name}/src/**.cs", "%{prj.name}/premake5.lua","%{prj.name}/**.natvis" }
	cppdialect "C++20"
	---	warnings "Extra" // TODO make it compile with this enabled.

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	filter "platforms:x86"
		architecture "x86"

	filter "platforms:x64"
		architecture "x86_64"
	

group "2. C interface"
include "clib"

group "3. C# interface"
include "dotnet_dll"
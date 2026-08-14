--- Tournament Builder premake5.lua

workspace "tournament_builder_native"
	configurations {"Debug" , "Release" }
	includedirs { "%{prj.name}/include" }
	
	targetdir "%{prj.name}/_out/%{cfg.buildcfg}/%{cfg.platform}"
	objdir "%{prj.name}/_intermediate/%{cfg.buildcfg}/%{cfg.platform}"
	startproject "cli_dbg"
	fatalwarnings "All"

	platforms { "x86" , "x64" } 
	
	files { "%{prj.name}/include/**.h", "%{prj.name}/include/**.hpp" , "%{prj.name}/src/**.cpp" , "%{prj.name}/premake5.lua","%{prj.name}/**.natvis" }
	cppdialect "C++20"
	---	warnings "Extra" // TODO make it compile with this enabled.

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"
		
group "1. Native C++ Library"
include "cpplib"

group "2. Native CLI Application"
include "cli_app"
include "cli_lib"

group "3. C interface"
include "clib"

group "4. Debug helper application"
include "cli_dbg"

group "5. Extras"
include "documentation_builder"

project "scripts"
	kind "Utility"
	files { "scripts/**.bat" , "scripts/**.py" }

project "build_process"
	kind "Utility"
	files { "premake5.lua" }

workspace "tournament_builder_dotnet"
	configurations {"Debug" , "Release" }
	targetdir "%{prj.name}/_out/%{cfg.buildcfg}/%{cfg.platform}"
	files { "%{prj.name}/src/**.cs", "%{prj.name}/premake5.lua" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

	include "dotnet_dll"
	include "dotnet_gui"


---dotnet_gui

project "dotnet_gui"
	targetname "tournament_builder_gui"
	language "C#"
	kind "WindowedApp"
	links {
		"dotnet_dll",
		"System",
		"System.ComponentModel",
		"System.Data",
		"System.Drawing",
		"System.Memory",
		"System.Text.Json",
		"System.Windows",
		"System.Windows.Forms"
	}
	copylocal {"dotnet_dll"}
	prebuildcommands {
		"{COPYFILE} %[%{!wks.location}]clib/_out/Release/x86/tournament_builder_clib.dll %[%{!cfg.buildtarget.directory}]"
	}
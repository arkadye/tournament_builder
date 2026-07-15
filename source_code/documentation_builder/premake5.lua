--- document builder

project "documentation_builder"
	language "C++"
	kind "ConsoleApp"
	location "documentation_builder/_build"
	files { "documentation_builder/documentation_src/**.md" , "../examples/**.json" , "../example_templates/**.json" }
	includedirs { "../thirdparty/cxxopts/cxxopts/include" }
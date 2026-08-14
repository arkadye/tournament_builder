import os
import sys
import subprocess
from pathlib import Path
import tempfile

BUILDER=None
SOLUTIONS=set()
PROJECTS=set()
CONFIGS=set()
PLATFORMS=set()

SUPPORTED_SOLUTIONS = { "native" , "dotnet" }

SUPPORTED_CONFIGS = { "Debug" , "Release" }

NATIVE_PROJECTS = { "cpplib", "cli_app", "cli_lib", "clib", "cli_dbg" }
DOTNET_PROJECTS = { "dotnet_dll" , "dotnet_gui" }

SUPPORTED_NATIVE_PLATFORMS = { "x86" , "x64" }
SUPPORTED_DOTNET_PLATFORMS = { "Any CPU" }

SOLUTION_DIR = Path(__file__).parent.parent.parent.resolve()

# Parse arg

last_arg = None
for arg in sys.argv:
	lower_arg = arg.lower()
	if last_arg == "builder":
		if builder is not None:
			print("Cannot set 'builder' arg: builder is already set to " + BUILDER)
		else:
			BUILDER = arg
	elif last_arg == "solution":
		SOLUTIONS.add(lower_arg)
	elif last_arg == "project":
		PROJECTS.add(lower_arg)
		if lower_arg in NATIVE_PROJECTS:
			SOLUTIONS.add("native")
		if lower_arg in DOTNET_PROJECTS:
			SOLUTIONS.add("dotnet")
	elif last_arg == "config":
		CONFIGS.add(lower_arg)
	elif last_arg == "platform":
		PLATFORMS.add(lower_arg)
		if lower_arg in SUPPORTED_NATIVE_PLATFORMS:
			SOLUTIONS.add("native")
		if lower_arg in SUPPORTED_DOTNET_PLATFORMS:
			SOLUTIONS.add("dotnet")
	last_arg = lower_arg

# VISUAL STUDIO STUFF

def locate_devenv(vswherepath):
	if type(BUILDER) is str:
		if not BUILDER.startswith("vs20"):
			return False
	print("Trying to locate Visual Studio.")
	if type(vswherepath) is str:
		vswherepath = Path(vswherepath)
	if type(vswherepath) is not Path:
		vswherepath = Path(os.path.expandvars("%programfiles(x86)%")) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
		print("vswhere path not set. Trying " + str(vswherepath))
	if not vswherepath.exists():
		print("vswhere not found at " + str(vswherepath))
		return None
	runvswhereresult = subprocess.run([vswherepath], capture_output = True, text=True)
	if runvswhereresult.returncode != 0:
		print("vswhere failed with code " + str(runvswhereresult.returncode))
		return None
	for line in iter(runvswhereresult.stdout.splitlines()):
		if not line.startswith("productPath:"):
			continue
		keyval = line.split(':', maxsplit=1)
		result = keyval[1].strip()
		print("Found devenv at " + result)
		return result
	return None

def build_visual_studio_project_run_args(devenvpath, sln, project, config, platform, outputfile):
	buildargs = []
	buildargs.append(str(devenvpath))
	buildargs.append(str(sln))
	buildargs.append("/Build")
	buildargs.append("{0}|{1}".format(config, platform))
	if project is not None:
		buildargs.append("/Project")
		buildargs.append(project)
	if outputfile is not None:
		buildargs.append("/Out")
		buildargs.append(str(outputfile))
	return buildargs

def visual_studio_find_sln_file(suffix):
	rootstr = "tournament_builder_" + suffix
	slnx_candidate = SOLUTION_DIR / (rootstr + ".slnx")
	if slnx_candidate.exists():
		return slnx_candidate
	sln_candidate = SOLUTION_DIR / (rootstr + ".sln")
	if sln_candidate.exists():
		return sln_candidate
	return None

def build_visual_studio_project(devenvpath, sln, project, config, platform):
	
	output_filename = "msvc_output_{}_{}_{}".format(Path(sln).stem, config,platform)
	if project is not None:
		output_filename = output_filename + project
	output_filename = output_filename.replace(" ","")
	output_filename = output_filename + ".log"
	outputfile = Path(tempfile.gettempdir()) / output_filename
	outputfile.unlink(missing_ok=True)
	args = build_visual_studio_project_run_args(devenvpath, sln, project, config, platform, outputfile)
	
	print("Starting build: Solution={} ; Config={}|{} Project={}.".format(sln, config,platform, project))
	print("Project=None builds all projects.")
	print("Command line: " + " ".join(args))
	print("This may take some time. Please wait.")

	buildprocess = subprocess.run(args, text=True, capture_output = True)
	with open(outputfile) as buildlog:
		for line in buildlog:
			print(line)
	print("Build finished. Output:")
	print(buildprocess.stdout)
	print("Build return code = " + str(buildprocess.returncode))


def build_visual_studio_platform_config(devenvpath, slnfile, projects, config, platform):
	if len(projects) == 0:
		projects = [None]

	# Correction for Visual Studio:
	if platform == "x86":
		platform = "Win32"

	for project in projects:
		build_visual_studio_project(devenvpath, slnfile, project, config, platform)
		

def build_visual_studio_solution(devenvpath, solution, projects, configs, platforms):
	if solution not in SUPPORTED_SOLUTIONS:
		print("Solution type '{}' is not supported. Skipping.".format(solution))
		return

	sln_file = visual_studio_find_sln_file(solution)
	if sln_file is None:
		print("Cannot find solution file: tournament_builder_{}.sln(x). Skipping it. Use premake5 to generate the solution.".format(solution))
		return

	if solution == "native":
		subprojects = projects & NATIVE_PROJECTS
		if len(platforms) == 0:
			subplatforms = SUPPORTED_NATIVE_PLATFORMS
		else:
			subplatforms = platforms & SUPPORTED_NATIVE_PLATFORMS
	elif solution == "dotnet":
		subprojects = projects & DOTNET_PROJECTS
		if len(platforms) == 0:
			subplatforms = SUPPORTED_DOTNET_PLATFORMS
		else:
			subplatforms = platforms & SUPPORTED_DOTNET_PLATFORMS
	for platform in subplatforms:
		for config in configs:
			build_visual_studio_platform_config(devenvpath, sln_file, subprojects, config, platform)


def build_visual_studio(vswherepath, devenvpath, solutions, projects, configs, platforms):
	print("Trying Visual Studio build")
	if type(devenvpath) is not str:
		devenvpath = locate_devenv(vswherepath)
	if type(devenvpath) is None:
		print("Could not find Visual Studio")
		return False

	# From this point on we are committed to return True
	if len(configs) == 0:
		configs = SUPPORTED_CONFIGS

	if len(solutions) == 0:
		solutions = SUPPORTED_SOLUTIONS

	for solution_name in solutions:
		build_visual_studio_solution(devenvpath, solution_name, projects, configs, platforms)


# OTHER BUILD PLATFORM STUFF
#	Not supported yet. Feel free to add support in a pull request!

# MAIN
build_visual_studio(None, None, SOLUTIONS, PROJECTS, CONFIGS, PLATFORMS)
import os
import sys
import subprocess
from pathlib import Path

def get_arg(argname , default_val):
	return_next_arg = False
	for arg in sys.argv:
		if return_next_arg:
			return arg
		elif argname == arg:
			return_next_arg = True
	return default_val
		

examples_root = get_arg("--root", os.path.join("..","..","examples"))
native_exe_path = get_arg("--native-path", os.path.join("..","cli_app","_out","Release","tournament_builder_cli"))
dotnet_exe_path = get_arg("--dotnet_path", os.path.join("..","dotnet_gui","_out","Release","tournament_builder_gui"))

def setup_test_enabled(path, testtype):
	result = Path(path).exists()
	if not result:
		print("".join(["WARNING! Cannot find ", testtype , " exectutable '",path,"'. All ", testtype, " tests will fail!"]))
	return result

test_native = setup_test_enabled(native_exe_path,"native")
test_dotnet = setup_test_enabled(dotnet_exe_path,"dotnet")

input_path = os.path.join(examples_root,"in")

def test_file_impl(arglist):
	argument = " ".join(arglist)
	print("".join(["Running test: '",argument,"'"]), end='')
	result = subprocess.run(arglist, capture_output=True)
	success = result.returncode == 0
	if success:
		print("SUCCESS")
	else:
		print("FAIL")
		print("Failed test output:")
		print(str(result.stdout))
		print("")
	return success


def test_file_native(infile, reference):
	if not test_native:
		return False
	arglist = [native_exe_path,"--input",infile,"--test-reference",reference,"--random-seed", str(42)]
	return test_file_impl(arglist)

def test_file_dotnet(infile, reference):
	if not test_dotnet:
		return False
	arglist = [dotnet_exe_path,"--input",infile,"--output",reference,"--test","--random-seed", str(42)]
	return test_file_impl(arglist)

def test_file(filename):
	infile = os.path.join(examples_root,"in",filename)
	outfile = os.path.join(examples_root,"out",filename)
	native_result = test_file_native(infile, outfile)
	dotnet_result = test_file_dotnet(infile, outfile)
	return int(native_result) + int(dotnet_result)


def test_folder(folder_name):
	result = (0,0)
	for entry in Path(os.path.join(input_path, folder_name)).iterdir():
		if entry.is_file():
			file_result = test_file(entry.relative_to(input_path))
			result = (result[0] + file_result , result[1] + 2 - file_result)
		if entry.is_dir():
			folder_result = test_folder(entry.relative_to(input_path))
			result = (result[0] + folder_result[0] , result[1] + folder_result[1])
	return result

result = test_folder("")
print("Tests passed: " + str(result[0]))
print("tests failed: " + str(result[1]))
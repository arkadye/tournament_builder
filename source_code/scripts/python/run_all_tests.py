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
filters = get_arg("--filter", None)

def setup_test_enabled(path, testtype):
	result = Path(path).exists()
	if not result:
		print("".join(["WARNING! Cannot find ", testtype , " exectutable '",path,"'. All ", testtype, " tests will fail!"]))
	return result

test_native = setup_test_enabled(native_exe_path,"native")
test_dotnet = setup_test_enabled(dotnet_exe_path,"dotnet")

input_path = os.path.join(examples_root,"in")

# Return True on success and False on a fail.
def test_file_impl(arglist):
	argument = " ".join(arglist)
	print("".join(["Running test: '",argument,"': "]), end='')
	result = subprocess.run(arglist, capture_output=True, text=True)
	success = result.returncode == 0
	if success:
		print("SUCCESS")
	else:
		print("FAIL")
		print("Failed test output:")
		print(str(result.stdout))
		print("")
	return success

# Return True on success and False on fail or None if skipped
def test_file_native(infile, reference):
	if not test_native:
		return None
	arglist = [native_exe_path,"--input",infile,"--test-reference",reference,"--random-seed", str(42)]
	return test_file_impl(arglist)

# Return True on success and False on fail or None if skipped
def test_file_dotnet(infile, reference):
	if not test_dotnet:
		return None
	arglist = [dotnet_exe_path,"--input",infile,"--output",reference,"--test","--random-seed", str(42)]
	return test_file_impl(arglist)

# Return a tuple -> (Native result, Dotnet result)
def test_file(filename):
	infile = os.path.join(examples_root,"in",filename)
	outfile = os.path.join(examples_root,"out",filename)
	native_result = test_file_native(infile, outfile)
	dotnet_result = test_file_dotnet(infile, outfile)
	return (native_result, dotnet_result)

# Return a dictionary. Each entry is testname, then (native result,dotnet result)
def test_folder(folder_name):
	result = {}
	for entry in Path(os.path.join(input_path, folder_name)).iterdir():
		if entry.is_file():
			if filters is None or entry.stem.find(filters) != -1:
				file_to_test = entry.relative_to(input_path)
				file_result = test_file(file_to_test)
				result[file_to_test] = file_result
		elif entry.is_dir():
			folder_result = test_folder(entry.relative_to(input_path))
			result.update(folder_result)
	return result

result = test_folder("")

print("SUMMARY:")

SKIP_OUTPUT = "_"
PASS_OUTPUT = "pass"
FAIL_OUTPUT = "FAIL"

def result_to_str(result):
	if result is None:
		return SKIP_OUTPUT
	elif result:
		return PASS_OUTPUT
	else:
		return FAIL_OUTPUT

if len(result) == 0:
	print("No tests run")
else:
	native_successes = 0
	dotnet_successes = 0
	native_failures = 0
	dotnet_failures = 0
	native_skipped = 0
	dotnet_skipped = 0
	longest_name = 0

	for name,fileresult in result.items():
		longest_name = max(longest_name,len(str(name)))
		if fileresult[0] is None:
			native_skipped += 1
		elif fileresult[0]:
			native_successes += 1
		else:
			native_failures += 1
		if fileresult[1] is None:
			dotnet_skipped += 1
		elif fileresult[1]:
			dotnet_successes += 1
		else:
			dotnet_failures += 1

	print("Tests passed: {} ({},{})".format(native_successes + dotnet_successes, native_successes, dotnet_successes))
	print("Tests failed: {} ({},{})".format(native_failures + dotnet_failures, native_failures, dotnet_failures))
	print("Tests skipped: {} ({},{})".format(native_skipped + dotnet_skipped, native_skipped, dotnet_skipped))
	print("")
	line_fmt = "{:<" + str(longest_name) + "} {:<" + str(len("native")) + "} {:<" + str(len("dotnet")) + "}"
	print(line_fmt.format("FILE","NATIVE","DOTNET"))
	for name,fileresult in result.items():
		print(line_fmt.format(str(name), result_to_str(fileresult[0]), result_to_str(fileresult[1])))
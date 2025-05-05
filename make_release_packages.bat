rem Build and make the release packages.
rem Requires cmake to be installed.

IF DEFINED CONFIG (
	echo "Config set to '%CONFIG%'"
) ELSE (
	SET CONFIG="Release"
)

IF DEFINED DESTINATION (
	echo "Destination set to '%DESTINATION%'"
) ELSE (
	SET DESTINATION="../packages"
)

PUSHD .

cd /D %~dp0/source_code
call clean_workspace.bat
cmake . && cmake --build . --config %CONFIG% && cmake --install . --config Release --prefix %DESTINATION%/%CONFIG%
POPD
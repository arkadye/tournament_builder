pushd .
cd %~dp0

python3 python/run_all_tests.py --root "../../examples" --native-path "../cli_app/_out/Release/tournament_builder_cli.exe" --dotnet_path "../dotnet_gui/_out/Release/tournament_builder_gui.exe"

popd
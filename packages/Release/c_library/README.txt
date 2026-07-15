TOURNAMENT BUILDER MONO LIBRARY
Pre-alpha version.

This provides the Tournament Builder as a managed dynamic library.

This is suitable for Common Runtime Interface projects, and is also the preferred add-on type for various game engines, including Unity Engine and GameMaker.

It consists of two libraries: the managed one named "tournament_builder_mono" and its dependency "tournament_builder_clib".
The file extension depends on the platform. On windows they are DLL files.

UNITY PLUGIN INSTRUCTIONS

1. Open the Unity Editor.
2. In the Assets window right click and select "Add Asset". 
3. Select BOTH dynamic libraries and add them.

The functionality from the mono interface will now be available in Unity C# under the namespace "TournamentBuilder".

For full documentation and latest versions visit: https://github.com/arkadye/tournament_builder
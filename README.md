# Tournament Maker

Build tournaments and do basic operations on them with this tool.

## Features

This tool accepts a competition description in JSON. This can be arbitrarily complex, within the limits of the machine you are running on. From this it will generate a list of matches and associated league tables, with logic for chaining the various matches and stages together.

At some point the ability to run Swiss formats might be added.

## Who is this for?

Primarily targetted at indie devs, though there is nothing to stop AAA studios using this if they really want, this tool is to aid you building sports or sports-adjacent games. I've known a few who build certain tournament structures into the game, and then turn down "can we have a format where..." requests because would be hard to incorporate other formats.

The idea here is that this can either be used as an offline tool to help build new formats to respond to those requests, or - even easier - your game could integrate this library and bundle the tournament descriptors in a free format, parsing them, saving the results, and using them as needed. It would then be possible for modders to create their own tournament descriptors to run within the game, and all you would need to do would be to point them to the appropriate file(s) to edit, and to this documentation.

In principle it could also be used by people running actual sports tournaments as an aid and that is a use-case that is supported. So if you have a feature request for this purpose do feel free to use it. In many ways the use of JSON was motivated by this use-case, since it is relatively easy for webservers to injest.

## Quickstart guide

### Building from source

Fork this repo and integrate the source code in your favourite manner. The CMakeLists.txt file in `source_code/static_library` should integrate the source code into your project.

The C++ interface is currently not documented here, although the headers are hopefully comprehensible.

There is also a `tournament_builder_cli_dbg` project which automatically finds the files in the example folder and runs targetting them. This is convenient for things like Visual Studio. If you wish to work from Visual Studio and its like, I suggest making this your default startup project.

### Unity Engine integration

There are two DLLs required: `tournament_maker_clib.dll` and `tournament_maker_mono.dll`.

In the "Assets" panel in Unity, right-click and select "Import New Asset..." Select both those DLLs and import them.

The C# interface is now available in Unity under the `TournamentBuilder.TournamentBuilder` namespace.

More detailed documentation to come.

### As a command line tool.

The command line tool will be made available in the Releases section. It can be invoked with the command `tourmanet_builder_cli --input [input file] --output [output file]`. Leave `--input` blank to read JSON from stdin (i.e. you can pipe it in) and leave `--output` blank to write the results to stdout (or pipe it out). More in-depth documentation to come.

### Input

A World JSON object is the input. See the JSON documentation below.

### Output

A different World JSON object is the output. This means the outputs can be modified as used again as inputs.

If an error (or errors) occur, this will instead output a JSON object with just a single field: `"errors"`, with an array of error messages.

## Compatibility
 
This is designed to be a standalone command line tool or as a C++ library, but a C interface is also planned allowing bindings with any language that supports them.
 
The C bindings can be used to interface with:
 
- Lua
- Python
- C#

Between these it can be used with the following engines:

- Unreal Engine (as a C++ library)
- Unity (as a C# assembly)
- GameMaker (as a DLL)
- PyGame (via Python bindings)

Conversion to an Unreal Plugin, or a GameMaker extension, or a Unity library is on the to-do list at some point. If you do this for your own use and are happy to share do feel free to [open a pull request](https://github.com/arkadye/tournament_builder/pulls) and I'll see what I can do.

Data is stored as JSON, which is a reasonably human-readable format.

## Usage

There are, broadly, two (maybe three, depending on how you count) ways to use this.

- An offline tool to simplify the work of generating tournament structures for you. You can then
injest the created structures as you build your program.

- An inline tool where your game stores actual tournament layouts and parses them when needed.
This way your users can then modify the game files to create their own structures, using
the offline version of this tool to help.

- An integrated tool, where this is embedded into your game allowing the players to build
structures within the game.

## Roadmap

### Current state: Alpha

This is the current state. Not everything planned is in, but minimal possibly useful functionality exists. At this point, share within small(-ish) programming communities only, but it is not ready for non-programmer focussed indie-devs yet.

[See everything planned for Alpha here.](https://github.com/arkadye/tournament_builder/milestone/1)

### Beta

This is the point to start sharing in indie-dev circles, since it is now useable enough to integrate with Unity and possibly GameMaker, even if it is not feature complete.

[See everything planned for Beta here.](https://github.com/arkadye/tournament_builder/milestone/2)


### Release

[See everything planned for Release here.](https://github.com/arkadye/tournament_builder/milestone/3)

### Post-release

These are things I don't plan to work on, but will accept pull requests for.

[See everything which would currently be 'Post-release' here.](https://github.com/arkadye/tournament_builder/milestone/4)


## Contributing

The easiest way to contribute is to click on the ["Issues" tab](https://github.com/arkadye/tournament_builder/issues), and either comment on an issue you see that is similar or start your own.

Use these for bug reports, feature requests, no matter how major or minor they seem.

If you want to contribute by writing code [make a fork](https://github.com/arkadye/tournament_builder/fork), and do your thing and make a pull request. If you would like to contribute but are not sure what to do there is a list of ideas above and there may be some things in "Issues" which you think you could solve.

## Attribution

Under the terms of the license there is no need to attribute or acknowledge this work. However, if you use it I would much appreciate it if you could let me know so I can maintain a list of projects that have used this tool.

Furthermore, while I acknowledge there is no requirement to do this (and I will never know if you don't), if you create any new Descriptors, Events, or other functionality for your project - no matter how obvious or niche or major or minor - it would be a courtesy to push those additions back to the main brach via a pull request.

## Documentation navigation

Use these links to navigate all the documentation:

- **Tournament Maker** (You are here)
  - [Tournament Builder Documentation](documentation\readme.md)
    - [Input and Output JSON](documentation\json\readme.md)
      - [Competition](documentation\json\competition.md)
      - [Competition Descriptors](documentation\json\descriptors\readme.md)
        - [Competitor View](documentation\json\descriptors\competitor_view.md)
        - [DEBUG: Never resolves](documentation\json\descriptors\DEBUG_never_resolves.md)
        - [Knockout-bracket](documentation\json\descriptors\knockout_bracket.md)
        - [Round Robin](documentation\json\descriptors\round_robin.md)
        - [Round of Matches](documentation\json\descriptors\round_of_matches.md)
      - [Competitor](documentation\json\competitor.md)
      - [Entry List](documentation\json\entry_list.md)
      - [Error messages](documentation\json\error_messages.md)
      - [Events](documentation\json\events\readme.md)
        - [Add tags](documentation\json\events\add_tags.md)
        - [Resolve all references](documentation\json\events\resolve_all_references.md)
        - [Resolve reference](documentation\json\events\resolve_reference.md)
        - [Set finishing positions](documentation\json\events\set_finishing_positions.md)
      - [Names](documentation\json\naming_rules.md)
      - [References](documentation\json\references.md)
      - [Tags](documentation\json\tags.md)
      - [World object](documentation\json\world.md)
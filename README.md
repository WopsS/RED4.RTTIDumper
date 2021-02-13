# RED4.RTTIDumper

A tool that dumps the scripting RTTI from RED4 engine ([Cyberpunk 2077](https://www.cyberpunk.net)).

## How to use

1. Get the latest [RED4ext](https://github.com/WopsS/RED4ext).
2. Compile the poject.
3. Copy the build artifacts to `<game_root>/red4ext/plugins`.
4. Run the game.

## Build instructions

1. Download and install [Visual Studio 2019 Community Edition](https://www.visualstudio.com/) or a higher version.
2. Extract the content of [Premake 5](https://github.com/premake/premake-core/releases) in the `premake` directory.
3. Clone this repository.
4. Go to the `premake` directory and run `generate_project.bat`.
5. Open the solution (`RED4.RTTIDumper.sln`) located in `project` directory.

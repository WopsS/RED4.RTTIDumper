# RED4.RTTIDumper

A tool that dumps the scripting RTTI from RED4 engine ([Cyberpunk 2077](https://www.cyberpunk.net)).

## How to use

1. Get the latest [RED4ext](https://github.com/WopsS/RED4ext).
2. Compile the poject.
3. Copy the build artifacts to `<game_root>/red4ext/plugins`.
4. Run the game.

## Build instructions

1. Download and install [Visual Studio 2022 Community Edition](https://www.visualstudio.com/) or a newer version.
2. Download and install [CMake 3.23](https://cmake.org/) or a newer version.
3. Clone this repository.
4. Clone the dependencies (`git submodule update --init --recursive`).
5. Create a directory named `build`.
5. Go to the `build` directory and run `cmake ..`.
6. Open the solution (`RED4.RTTIDumper.sln`).
7. Build the projects.

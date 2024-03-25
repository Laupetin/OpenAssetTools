![OpenAssetTools Logo](repo/logo_64_64.png) OpenAssetTools
============

OpenAssetTools (OAT) is a work in progress project aiming to create a modding tools suite for older Call Of Duty titles.
It is not a reimplementation of existing modding tools or related to them but aims to be as compatible as possible in terms of formats and add support for previously unsupported titles.

In its current form, it is still incomplete and only offers a small subset of modding features.
It currently offers support for the PC versions of:

- [IW3 (COD4: Modern Warfare - 2007)](https://en.wikipedia.org/wiki/Call_of_Duty_4:_Modern_Warfare)
- [IW4 (Modern Warfare 2 - 2009)](https://en.wikipedia.org/wiki/Call_of_Duty:_Modern_Warfare_2)
- [IW5 (Modern Warfare 3 - 2011)](https://en.wikipedia.org/wiki/Call_of_Duty:_Modern_Warfare_3)
- [T5 (Black Ops - 2010)](https://en.wikipedia.org/wiki/Call_of_Duty:_Black_Ops)
- [T6 (Black Ops II - 2012)](https://en.wikipedia.org/wiki/Call_of_Duty:_Black_Ops_II) 

with very varying progress on the different games. 

## Features

While still incomplete these are the features that are currently available:

### Tools

The **Unlinker** tool allows loading any fastfile (.ff) of supported games and works standalone.
While being able to load any asset it can currently only dump a subset of these assets to disk, depending on the game.
The formats of the dumped assets are either as close as possible to the originally used formats for these games or a commonly used format.

The **Linker** tool allows building custom fastfiles (.ff) for the supported games.
It can also load existing zones and reuse their assets from in-memory.
Only a subset of assets can be read from disk, however, depending on the game.

Additionally, there are a couple of libraries and tools.
A full list of the subcomponents can be found [here](https://openassettools.dev/guide/components.html). 

## Using the tools

Either download [the latest release from Github](https://github.com/Laupetin/OpenAssetTools/releases/latest) or [build OAT yourself](#building-oat).
Building the tools yourself is only recommended if you want to do some changes to the code.
Otherwise, you are probably better off just using the latest release.

You can put OAT anywhere, it does **not** need to be placed in your game folder.
In fact, I recommend against it to be able to better distinguish OAT files from game files.

For more information, check out the ["Getting started" guide in the OAT documentation](https://openassettools.dev/guide/getting-started.html).
You can find other useful guides and reference documentation there as well.

## Building OAT

You need to clone this repository using Git.
Do not download as a zip from Github because it will not be a git repository which is required for installing the submodules.

Building OAT requires Git to be installed on your machine and to be in your PATH.
This project uses Premake to generate project files depending on your platform.

Note: The first time setting up will clone any submodules with git so the first initialization might take a bit.

## Documentation & Examples

The documentation can be found on the [OpenAssetTools.dev page](https://openassettools.dev).
It is hosted on Github Pages and its source can be found in the [OAT-Docs Repository](https://github.com/Laupetin/OAT-Docs).

For examples of mods that can be built with OAT, you can either look into the [docs folder](docs/example) for some (currently very barebones) examples
or check out one of the following projects that are using OAT:

* [JezuzLizard/t6-fastfile-mods](https://github.com/JezuzLizard/t6-fastfile-mods)
* [Jbleezy/BO2-Reimagined](https://github.com/Jbleezy/BO2-Reimagined)

If you have a mod that you believe would be a perfect fit here, don't hesitate to open a pull request to include it

### Windows

When building on Windows you will need to have Visual Studio 2022 installed.
It is also possible to use other compilers, however, due to the memory layout of structs being important you might
run into crashes or invalid data so no guarantees can be given.

Use `generate.bat` to generate Visual Studio solution files.
This will also clone any submodules with git so the first time running it might take a bit.
When this is done you will have a `build` folder with a `OpenAssetTools.sln` solution file within that you can open with Visual Studio.

Please note that due to having source files and project files in separate folders you need to pay attention that when adding new files in Visual Studio they are being placed in the correct folder.

Now you can just build the solution.
The resulting binaries can be found in `build/bin/<Debug_x86|Release_x86>`.

### Linux

When building on Linux you need to have g++ 13 or later installed.
Since you are most likely on a 64-bit machine you will also need multilib for compiling for 32-bit.
Using other compilers may also be possible, but they are not guaranteed to work out of the box. You may need to edit [premake5.lua](premake5.lua) to ensure compatibility with other compilers such as Clang.

Use `generate.sh` to generate make files.
When this is done you will have a `build` folder with a `Makefile`.
You now run `make` manually or use `./scripts/make_release.sh` or `./scripts/make_debug.sh` to build.

The resulting binaries can be found in `build/bin/<Debug_x86|Release_x86>`.

## Legal

OAT source code is licensed under [GPLv3](./LICENSE).

Extracting the contents of game files does not grant you any rights to them.
All rights remain to their respective owners.

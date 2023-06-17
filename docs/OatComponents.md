# OAT Components

The OpenAssetTools project consists of a handful of components.

## Tools

### Linker

The **Linker** tool allows building custom fastfiles (.ff) for the supported games.
It can also load existing zones and reuse their assets from in-memory.
Only a subset of assets can be read from disk however, depending on the game.

This tool has a CLI.
For a list of all args, run `Linker --help`.

### Unlinker

The **Unlinker** tool allows loading any fastfile (.ff) of supported games and works standalone.
While being able to load any asset it can currently only dump a subset of these assets to disk, depending on the game.
The formats of the dumped assets are either as close as possible to the originally used formats for these games or a commonly used format.

This tool can be used either by drag-and-dropping and fastfile onto it or using its CLI.
For a list of all args, run `Unlinker --help`.

To be able to extract images from IWDs you need to add their containing folder to the search path.
For example: `./Unlinker.exe -v --search-path "C:/Cod/IW4x/main" "C:/Cod/IW4x/zone/english/common_mp.ff"`

## Building tools

These are tools that are part of the building process of OAT.
They are compiled first and then being run in the later stages of compiling the project.

### RawTemplater

The RawTemplater allows generating files based on a template with a c precompiler like language.
The template allows the definition of variants in the form of properties that can be either true/false or have a range of values.
For each resulting cartesian product of these properties a file is generated unless skipped.

This is used for generating raw files for the shader pipeline.
An example of a template files can be found [here (particle_cloud.techset.template)](../raw/iw4/techsets/particle_cloud.techset.template).

### ZoneCodeGenerator

The ZoneCodeGenerator generates code for loading and writing asset structs from and to fastfiles.
It is very apparent that the original developers must have used a very similar technique of code generation therefore this aims
to replicate what they might have done.
Since nothing about this code generation is known however this implementation is entirely custom and results in code that fits into
the code base of OAT only.

The tool takes in a C header file and a commands file.
The c header files is parsed for structs and unions to base the code on.
The commands file describes further information necessary for code generation that is not possible to know just from the structs.

The parsing of the c header file and the commands file is implemented using the Parser component.
The format of the commands file is entirely custom to OAT.

An example of a commands file can be found [here (IW4_Commands.txt)](../src/ZoneCode/Game/IW4/IW4_Commands.txt)

## Shared components

These components split up code for reuse over the different tools.

### Common

This component contains code that is shared between other Obj based and Zone based components.

### Crypto

This component contains wrappers around used crypto and hashing third party libraries. 

### ObjCommon

This component contains code that is used for both loading and writing asset data from and to disk. 

### ObjLoading

This component contains code that is used for loading asset data from disk.

### ObjWriting

This component contains code that is used for writing asset data to disk.

### Parser

This component offers a custom Parser and Lexer implementation for parsing text based languages.
It also includes commonly used patterns and preprocessors for Parsers in the project.

Arguably there shouldn't be a custom parser implementation for this, but instead a more accepted dependency like ANTLR.
However, I wanted to write my own implementations for science so here we are.

### Utils

This component contains various utility code that is used in other tools or components.

### ZoneCode

This component contains files necessary for generating code used for loading/writing from/to fastfiles.

### ZoneCodeGeneratorLib

This component contains most of the code of ZoneCodeGenerator other than the CLI.
Can be used as a static lib.

### ZoneCommon

This component bundles code for both loading and writing from/to fastfiles. 

### ZoneLoading

This component handles loading data from fastfiles.

### ZoneWriting

This component handles writing data to fastfiles.
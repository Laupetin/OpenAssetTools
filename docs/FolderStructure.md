# Folder structure

This document is supposed to describe how i plan to handle the folder structure of the final modding tools.
The result should be oriented at the original game's modding tools.

```
├───bin
├───deffiles
├───raw
├───source_data
├───zone_dump
│   ├───source_data
│   ├───zone_raw
│   └───zone_source
├───zone_out
├───zone_raw
└───zone_source
```

## bin

Folder containing binaries for modding tools like Unlinker.exe, Linker.exe and any other.

## deffiles

Folder containing files (gdf) defining interface and properties of an asset managing tool.

## raw

Folder containing global raw assets that can be used while building any zone.

## source_data

Folder containing gdts containing asset data.
Gdts can be referenced inside a zone definition file to be used inside a zone.

## zone_dump

Contains all data dumped using Unlinker which includes raw assets in ``zone_raw``, zone definitions in ``zone_source`` and gdts in ``source_data``.

Separating the data of dumped files serves the purpose of separating it from manually created zones and prevents accidentally overwriting self created data.

## zone_out

The folder that contains the output of the linking process.

## zone_raw

Contains raw assets that should only be used for a zone with the same name as its subfolder.
Any contents must be inside appropriately named subfolders.
So assets for a zone called ``my_mod`` must be in ``zone_raw/my_mod``.

## zone_source

A folder containing all zone definition files.
Any definition file must have the name of the zone it describes.
So if you want to create a zone with the name ``my_mod`` you need to describe it inside a zone definition with the path ``zone_source/my_mod.zone``.
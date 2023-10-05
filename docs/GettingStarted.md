# Getting started

## Unlinking a fastfile

A good starting point is dumping an existing zone from your game installation to get a glance at how the raw asset data is structured.
The `Unlinker` tool will create a dump of a fastfile that can be linked again in the same form with the `Linker` tool.

The `Unlinker` tool can be used the following:
```shell
# Load the original zone from the game folder.
# Change this if you have installed your game somewhere else
GAME_FOLDER='C:/Program Files (x86)/Steam/steamapps/common/Call of Duty Modern Warfare 3'

# Verbose mode logs more details which can be useful to find errors
ARGS='--verbose'

# Specify the search-path to include the main folder to automatically load its IWD files for image dumping.
# For T6 also specify sounds;zone/all;zone/english to load appropriate IPAKs and sound banks.
# You can skip specifying the search path but it will not dump images then.
ARGS="$ARGS --search-path \"$GAME_FOLDER/main;$GAME_FOLDER/zone/english\""

ZONE_TO_UNLINK="$GAME_FOLDER/zone/english/ui.ff"

Unlinker $ARGS "$ZONE_TO_UNLINK"
```

The variables are only to better demonstrate the different parameters.
You can also of course combine them:

```shell
Unlinker --verbose "C:/Program Files (x86)/Steam/steamapps/common/Call of Duty Modern Warfare 3/zone/english/ui.ff"
```

When using Windows you can also drag and drop a fastfile onto `Unlinker` which will run the unlinking tool with just the fastfile path and no additional parameters.

After `Unlinker` ran successfully, you will have a `zone_dump` folder in your working directory.
It contains another folder `zone_raw` which contains the definitions for the unlinked fastfiles.
You can move the project definitions one level up in the folder structure to be able to use them with the `Linker` tool.
That means `zone_dump/zone_raw/ui` becomes `zone_raw/ui`

## Extending a vanilla fastfile

For an example of extending a fastfile, see [the example "ExtendZone"](example/ExtendZone).

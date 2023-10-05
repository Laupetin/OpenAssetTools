#!/bin/bash

# Load the original zone from the game folder.
GAME_FOLDER='C:/Program Files (x86)/Steam/steamapps/common/Call of Duty Modern Warfare 3'

# Load the original zone into the Linker to be able to use its assets from in-memory.
# If you want to overwrite the vanilla zone later
Linker --load "$GAME_FOLDER/zone/english/ui.ff" ExtendZoneProject
# Zone Files

`.zone` files define a "project" to be linked with OAT.
Their format is inspired by the zone files that BO3 mod tools use, their usage is differing a bit however.

They are text-based and contain a comma-separated key-based notation.
They may include line and block comments and are preprocessed using a C like processor before being parsed.

Its content can either be an entry or metadata.
Metadata has its line begin with the `>` character followed by the metadata-key, a comma `,` and the metadata-value.
An entry follows the same schema just without the `>` prefix.

For example:
```
// This is a line comment
/* 
    This is a block comment
*/

// The following is metadata
>game,IW4
>name,common_mp
>level.ipak_read,base

// The following are entries
include,common_mp_menus
ignore,code_post_gfx_mp
material,main_splash

// These are preprocessor directives
#define ADD_TECHSETS 1
#ifdef ADD_TECHSETS
techset,custom_techset
#endif
```

## Metadata

### `game` Metadata

The `game` metadata sets the game the project is for.
This metadata **must** be specified for a project to be linked.

The following values can be used to specify the game.
| Value | Game                           |
| ----- | ------------------------------ |
| IW3   | Call Of Duty: Modern Warfare   |
| IW4   | Call Of Duty: Modern Warfare 2 |
| IW5   | Call Of Duty: Modern Warfare 3 |
| T5    | Call Of Duty: Black Ops        |
| T6    | Call Of Duty: Black Ops 2      |

### `name` Metadata

The `name` sets the name of the project linking result.
In case of a fastfile the resulting file will be called `<name>.ff`.
If the fastfile uses encryption, or is signed it will consider the new name.

If not specified the name will be the same as the project name.

### `gdt` Metadata

The `gdt` metadata specifies a GDT file to be loaded by the Linker.

## Entries

With a few exceptions, entries are used to define an asset to be added.
The available assets differ per game.

### The `include` entry

The `include` entry will not add an asset.
Instead it will make the Linker read another `.zone` file and include all of its added assets, metadata and other includes in the project.

For example `include,common_mp_menus` will look for a `common_mp_menus.zone` file in all source search paths and will include its content.
Linking will abort if the file could not be found.

### The `ignore` entry

The `ignore` entry will not add an asset.
Instead it will try to read an assetlist of the specified value.
All assets in the asset list will be converted to a reference in case they get added to the project.
This makes it possible to not add assets again that were already included in another zone but instead of referenced.

For example `ignore,code_post_gfx_mp` will make all assets of `code_post_gfx_mp` be converted to references when they are added instead of the assets themselves being added.

### Other entries

Other entries are treated as assets to be added.
The available assets differ per game.

#### IW3: Call Of Duty: Modern Warfare

- `xmodelpieces`
- `physpreset`
- `xanim`
- `xmodel`
- `material`
- `techniqueset`
- `image`
- `sound`
- `soundcurve`
- `loadedsound`
- `clipmap`
- `comworld`
- `gameworldsp`
- `gameworldmp`
- `mapents`
- `gfxworld`
- `lightdef`
- `uimap`
- `font`
- `menulist`
- `menu`
- `localize`
- `weapon`
- `snddriverglobals`
- `fx`
- `impactfx`
- `aitype`
- `mptype`
- `character`
- `xmodelalias`
- `rawfile`
- `stringtable`

#### IW4: Call Of Duty: Modern Warfare 2

- `physpreset`
- `physcollmap`
- `xanim`
- `xmodelsurfs`
- `xmodel`
- `material`
- `pixelshader`
- `vertexshader`
- `vertexdecl`
- `techniqueset`
- `image`
- `sound`
- `soundcurve`
- `loadedsound`
- `clipmap`
- `comworld`
- `gameworldsp`
- `gameworldmp`
- `mapents`
- `fxworld`
- `gfxworld`
- `lightdef`
- `uimap`
- `font`
- `menulist`
- `menu`
- `localize`
- `weapon`
- `snddriverglobals`
- `fx`
- `impactfx`
- `aitype`
- `mptype`
- `character`
- `xmodelalias`
- `rawfile`
- `stringtable`
- `leaderboard`
- `structureddatadef`
- `tracer`
- `vehicle`
- `addonmapents`

#### IW5: Call Of Duty: Modern Warfare 3

- `physpreset`
- `physcollmap`
- `xanim`
- `xmodelsurfs`
- `xmodel`
- `material`
- `pixelshader`
- `vertexshader`
- `vertexdecl`
- `techniqueset`
- `image`
- `sound`
- `soundcurve`
- `loadedsound`
- `clipmap`
- `comworld`
- `glassworld`
- `pathdata`
- `vehicletrack`
- `mapents`
- `fxworld`
- `gfxworld`
- `lightdef`
- `uimap`
- `font`
- `menulist`
- `menu`
- `localize`
- `attachment`
- `weapon`
- `snddriverglobals`
- `fx`
- `impactfx`
- `surfacefx`
- `aitype`
- `mptype`
- `character`
- `xmodelalias`
- `rawfile`
- `scriptfile`
- `stringtable`
- `leaderboard`
- `structureddatadef`
- `tracer`
- `vehicle`
- `addonmapents`

#### T5: Call Of Duty: Black Ops

- `xmodelpieces`
- `physpreset`
- `physconstraints`
- `destructibledef`
- `xanim`
- `xmodel`
- `material`
- `techniqueset`
- `image`
- `soundbank`
- `soundpatch`
- `clipmap`
- `comworld`
- `gameworldsp`
- `gameworldmp`
- `mapents`
- `gfxworld`
- `gfxlightdef`
- `uimap`
- `font`
- `menulist`
- `menu`
- `localize`
- `weapon`
- `weapondef`
- `weaponvariant`
- `snddriverglobals`
- `fx`
- `fximpacttable`
- `aitype`
- `mptype`
- `mpbody`
- `mphead`
- `character`
- `xmodelalias`
- `rawfile`
- `stringtable`
- `packindex`
- `xglobals`
- `ddl`
- `glasses`
- `emblemset`

#### T6: Call Of Duty: Black Ops 2

- `xmodelpieces`
- `physpreset`
- `physconstraints`
- `destructibledef`
- `xanim`
- `xmodel`
- `material`
- `techniqueset`
- `image`
- `soundbank`
- `soundpatch`
- `clipmap`
- `comworld`
- `gameworldsp`
- `gameworldmp`
- `mapents`
- `gfxworld`
- `gfxlightdef`
- `uimap`
- `font`
- `fonticon`
- `menulist`
- `menu`
- `localize`
- `weapon`
- `weapondef`
- `weaponvariant`
- `weaponfull`
- `attachment`
- `attachmentunique`
- `camo`
- `snddriverglobals`
- `fx`
- `fximpacttable`
- `aitype`
- `mptype`
- `mpbody`
- `mphead`
- `character`
- `xmodelalias`
- `rawfile`
- `stringtable`
- `leaderboard`
- `xglobals`
- `ddl`
- `glasses`
- `emblemset`
- `script`
- `keyvaluepairs`
- `vehicle`
- `memoryblock`
- `addonmapents`
- `tracer`
- `skinnedverts`
- `qdb`
- `slug`
- `footsteptable`
- `footstepfxtable`
- `zbarrier`
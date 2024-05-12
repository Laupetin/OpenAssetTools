# Supported Asset Types

All asset types are supported to be loaded from other fastfiles in memory.
The following section specify which assets are supported to be dumped to disk (using `Unlinker`) and loaded from disk (using `Linker`):

- ❌ = Currently unsupported
- ✅ = Supported
- ⁉️ = Partial support

## IW3 (Call of Duty 4: Modern Warfare)

| Asset Type           | Dumping Support | Loading Support | Notes                                                             |
| -------------------- | --------------- | --------------- | ----------------------------------------------------------------- |
| PhysPreset           | ❌              | ❌              |                                                                   |
| XAnimParts           | ❌              | ❌              |                                                                   |
| XModel               | ⁉️              | ❌              | Model data can be exported to `XMODEL_EXPORT`, `OBJ`, `GLB/GLTF`. |
| Material             | ❌              | ❌              |                                                                   |
| MaterialTechniqueSet | ❌              | ❌              |                                                                   |
| GfxImage             | ✅              | ✅              |                                                                   |
| snd_alias_list_t     | ❌              | ❌              |                                                                   |
| SndCurve             | ❌              | ❌              |                                                                   |
| LoadedSound          | ✅              | ❌              |                                                                   |
| clipMap_t            | ❌              | ❌              |                                                                   |
| ComWorld             | ❌              | ❌              |                                                                   |
| GameWorldSp          | ❌              | ❌              |                                                                   |
| GameWorldMp          | ❌              | ❌              |                                                                   |
| MapEnts              | ✅              | ❌              |                                                                   |
| GfxWorld             | ❌              | ❌              |                                                                   |
| GfxLightDef          | ❌              | ❌              |                                                                   |
| Font_s               | ❌              | ❌              |                                                                   |
| MenuList             | ❌              | ❌              |                                                                   |
| menuDef_t            | ❌              | ❌              |                                                                   |
| LocalizeEntry        | ✅              | ✅              |                                                                   |
| WeaponDef            | ❌              | ❌              |                                                                   |
| FxEffectDef          | ❌              | ❌              |                                                                   |
| FxImpactTable        | ❌              | ❌              |                                                                   |
| RawFile              | ✅              | ✅              |                                                                   |
| StringTable          | ✅              | ✅              |                                                                   |

## IW4 (Call of Duty: Modern Warfare 2)

| Asset Type                | Dumping Support | Loading Support | Notes                                                                   |
| ------------------------- | --------------- | --------------- | ----------------------------------------------------------------------- |
| PhysPreset                | ✅              | ✅              |                                                                         |
| PhysCollmap               | ❌              | ❌              |                                                                         |
| XAnimParts                | ❌              | ❌              |                                                                         |
| XModel                    | ⁉️              | ❌              | Model data can be exported to `XMODEL_EXPORT`, `OBJ`, `GLB/GLTF`.       |
| Material                  | ❌              | ❌              |                                                                         |
| MaterialPixelShader       | ✅              | ✅              | Shaders are compiled. Only dumps/loads shader bytecode.                 |
| MaterialVertexShader      | ✅              | ✅              | Shaders are compiled. Only dumps/loads shader bytecode.                 |
| MaterialVertexDeclaration | ✅              | ✅              | Vertex declarations are dumped/loaded as part of a technique.           |
| MaterialTechniqueSet      | ⁉️              | ⁉️              | Statemaps are currently not exported.                                   |
| GfxImage                  | ✅              | ❌              | A few special image encodings are not yet supported.                    |
| snd_alias_list_t          | ❌              | ❌              |                                                                         |
| SndCurve                  | ✅              | ✅              |                                                                         |
| LoadedSound               | ✅              | ❌              |                                                                         |
| clipMap_t                 | ❌              | ❌              |                                                                         |
| ComWorld                  | ❌              | ❌              |                                                                         |
| GameWorldSp               | ❌              | ❌              |                                                                         |
| GameWorldMp               | ❌              | ❌              |                                                                         |
| MapEnts                   | ❌              | ❌              |                                                                         |
| FxWorld                   | ❌              | ❌              |                                                                         |
| GfxWorld                  | ❌              | ❌              |                                                                         |
| GfxLightDef               | ✅              | ✅              |                                                                         |
| Font_s                    | ❌              | ❌              |                                                                         |
| MenuList                  | ✅              | ✅              | The output is decompiled. The result will not be the same as the input. |
| menuDef_t                 | ✅              | ✅              | See menulist.                                                           |
| LocalizeEntry             | ✅              | ✅              |                                                                         |
| WeaponCompleteDef         | ✅              | ✅              |                                                                         |
| FxEffectDef               | ❌              | ❌              |                                                                         |
| FxImpactTable             | ❌              | ❌              |                                                                         |
| RawFile                   | ✅              | ✅              |                                                                         |
| StringTable               | ✅              | ✅              |                                                                         |
| LeaderboardDef            | ❌              | ❌              |                                                                         |
| StructuredDataDefSet      | ✅              | ✅              | The format is custom due to lacking information about original format.  |
| TracerDef                 | ✅              | ❌              |                                                                         |
| VehicleDef                | ✅              | ❌              |                                                                         |
| AddonMapEnts              | ⁉️              | ❌              | MapEnts String can be exported. Binary data currently not.              |

## IW5 (Call of Duty: Modern Warfare 3)

| Asset Type                | Dumping Support | Loading Support | Notes                                                                                                         |
| ------------------------- | --------------- | --------------- | ------------------------------------------------------------------------------------------------------------- |
| PhysPreset                | ❌              | ❌              |                                                                                                               |
| PhysCollmap               | ❌              | ❌              |                                                                                                               |
| XAnimParts                | ❌              | ❌              |                                                                                                               |
| XModelSurfs               | ❌              | ❌              |                                                                                                               |
| XModel                    | ⁉️              | ❌              | Model data can be exported to `XMODEL_EXPORT`, `OBJ`, `GLB/GLTF`.                                             |
| Material                  | ❌              | ❌              |                                                                                                               |
| MaterialPixelShader       | ❌              | ❌              |                                                                                                               |
| MaterialVertexShader      | ❌              | ❌              |                                                                                                               |
| MaterialVertexDeclaration | ❌              | ❌              |                                                                                                               |
| MaterialTechniqueSet      | ❌              | ❌              |                                                                                                               |
| GfxImage                  | ✅              | ❌              | A few special image encodings are not yet supported.                                                          |
| snd_alias_list_t          | ❌              | ❌              |                                                                                                               |
| SndCurve                  | ❌              | ❌              |                                                                                                               |
| LoadedSound               | ✅              | ❌              |                                                                                                               |
| clipMap_t                 | ❌              | ❌              |                                                                                                               |
| ComWorld                  | ❌              | ❌              |                                                                                                               |
| GlassWorld                | ❌              | ❌              |                                                                                                               |
| PathData                  | ❌              | ❌              |                                                                                                               |
| VehicleTrack              | ❌              | ❌              |                                                                                                               |
| MapEnts                   | ❌              | ❌              |                                                                                                               |
| FxWorld                   | ❌              | ❌              |                                                                                                               |
| GfxWorld                  | ❌              | ❌              |                                                                                                               |
| GfxLightDef               | ❌              | ❌              |                                                                                                               |
| Font_s                    | ❌              | ❌              |                                                                                                               |
| MenuList                  | ✅              | ✅              | The output is decompiled. The result will not be the same as the input.                                       |
| menuDef_t                 | ✅              | ✅              | See menulist.                                                                                                 |
| LocalizeEntry             | ✅              | ✅              |                                                                                                               |
| WeaponAttachment          | ✅              | ✅              |                                                                                                               |
| WeaponCompleteDef         | ✅              | ✅              |                                                                                                               |
| FxEffectDef               | ❌              | ❌              |                                                                                                               |
| FxImpactTable             | ❌              | ❌              |                                                                                                               |
| SurfaceFxTable            | ❌              | ❌              |                                                                                                               |
| RawFile                   | ✅              | ✅              |                                                                                                               |
| ScriptFile                | ⁉️              | ⁉️              | Can only be dumped/loaded as binary. Editing is possible with [GSC-Tool](https://github.com/xensik/gsc-tool). |
| StringTable               | ✅              | ✅              |                                                                                                               |
| LeaderboardDef            | ❌              | ❌              |                                                                                                               |
| StructuredDataDefSet      | ❌              | ❌              |                                                                                                               |
| TracerDef                 | ❌              | ❌              |                                                                                                               |
| VehicleDef                | ❌              | ❌              |                                                                                                               |
| AddonMapEnts              | ⁉️              | ❌              | MapEnts String can be exported. Binary data currently not.                                                    |

## T5 (Call of Duty: Black Ops)

| Asset Type           | Dumping Support | Loading Support | Notes                                                             |
| -------------------- | --------------- | --------------- | ----------------------------------------------------------------- |
| PhysPreset           | ❌              | ❌              |                                                                   |
| PhysConstraints      | ❌              | ❌              |                                                                   |
| DestructibleDef      | ❌              | ❌              |                                                                   |
| XAnimParts           | ❌              | ❌              |                                                                   |
| XModel               | ⁉️              | ❌              | Model data can be exported to `XMODEL_EXPORT`, `OBJ`, `GLB/GLTF`. |
| Material             | ❌              | ❌              |                                                                   |
| MaterialTechniqueSet | ❌              | ❌              |                                                                   |
| GfxImage             | ✅              | ❌              | A few special image encodings are not yet supported.              |
| SndBank              | ❌              | ❌              |                                                                   |
| SndPatch             | ❌              | ❌              |                                                                   |
| clipMap_t            | ❌              | ❌              |                                                                   |
| ComWorld             | ❌              | ❌              |                                                                   |
| GameWorldSp          | ❌              | ❌              |                                                                   |
| GameWorldMp          | ❌              | ❌              |                                                                   |
| MapEnts              | ❌              | ❌              |                                                                   |
| GfxWorld             | ❌              | ❌              |                                                                   |
| GfxLightDef          | ❌              | ❌              |                                                                   |
| Font_s               | ❌              | ❌              |                                                                   |
| MenuList             | ❌              | ❌              |                                                                   |
| menuDef_t            | ❌              | ❌              |                                                                   |
| LocalizeEntry        | ✅              | ✅              |                                                                   |
| WeaponVariantDef     | ❌              | ❌              |                                                                   |
| SndDriverGlobals     | ❌              | ❌              |                                                                   |
| FxEffectDef          | ❌              | ❌              |                                                                   |
| FxImpactTable        | ❌              | ❌              |                                                                   |
| RawFile              | ✅              | ✅              |                                                                   |
| StringTable          | ✅              | ✅              |                                                                   |
| PackIndex            | ❌              | ❌              |                                                                   |
| XGlobals             | ❌              | ❌              |                                                                   |
| ddlRoot_t            | ❌              | ❌              |                                                                   |
| Glasses              | ❌              | ❌              |                                                                   |
| EmblemSet            | ❌              | ❌              |                                                                   |

## T6 (Call of Duty: Black Ops II)

| Asset Type             | Dumping Support | Loading Support | Notes                                                                                                                                                          |
| ---------------------- | --------------- | --------------- | -------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| PhysPreset             | ✅              | ✅              |                                                                                                                                                                |
| PhysConstraints        | ✅              | ✅              |                                                                                                                                                                |
| DestructibleDef        | ❌              | ❌              |                                                                                                                                                                |
| XAnimParts             | ❌              | ❌              |                                                                                                                                                                |
| XModel                 | ⁉️              | ❌              | Model data can be exported to `XMODEL_EXPORT`, `OBJ`, `GLB/GLTF`.                                                                                              |
| Material               | ⁉️              | ⁉️              | Dumping/Loading is currently possible for materials in their compiled form. There is currently no material pipeline.                                           |
| MaterialTechniqueSet   | ⁉️              | ❌              | Only dumps compiled shaders.                                                                                                                                   |
| GfxImage               | ✅              | ✅              | A few special image encodings are not yet supported.                                                                                                           |
| SndBank                | ✅              | ✅              | The current implementation is subject to change.                                                                                                               |
| SndPatch               | ❌              | ❌              |                                                                                                                                                                |
| clipMap_t              | ❌              | ❌              |                                                                                                                                                                |
| ComWorld               | ❌              | ❌              |                                                                                                                                                                |
| GameWorldSp            | ❌              | ❌              |                                                                                                                                                                |
| GameWorldMp            | ❌              | ❌              |                                                                                                                                                                |
| MapEnts                | ❌              | ❌              |                                                                                                                                                                |
| GfxWorld               | ❌              | ❌              |                                                                                                                                                                |
| GfxLightDef            | ❌              | ❌              |                                                                                                                                                                |
| Font_s                 | ❌              | ❌              |                                                                                                                                                                |
| FontIcon               | ✅              | ✅              |                                                                                                                                                                |
| MenuList               | ❌              | ❌              |                                                                                                                                                                |
| menuDef_t              | ❌              | ❌              |                                                                                                                                                                |
| LocalizeEntry          | ✅              | ✅              |                                                                                                                                                                |
| WeaponVariantDef       | ✅              | ✅              |                                                                                                                                                                |
| WeaponAttachment       | ✅              | ✅              |                                                                                                                                                                |
| WeaponAttachmentUnique | ✅              | ✅              |                                                                                                                                                                |
| WeaponCamo             | ✅              | ✅              |                                                                                                                                                                |
| SndDriverGlobals       | ✅              | ❌              |                                                                                                                                                                |
| FxEffectDef            | ❌              | ❌              |                                                                                                                                                                |
| FxImpactTable          | ❌              | ❌              |                                                                                                                                                                |
| RawFile                | ✅              | ✅              |                                                                                                                                                                |
| StringTable            | ✅              | ✅              |                                                                                                                                                                |
| LeaderboardDef         | ❌              | ❌              |                                                                                                                                                                |
| XGlobals               | ❌              | ❌              |                                                                                                                                                                |
| ddlRoot_t              | ❌              | ❌              |                                                                                                                                                                |
| Glasses                | ❌              | ❌              |                                                                                                                                                                |
| EmblemSet              | ❌              | ❌              |                                                                                                                                                                |
| ScriptParseTree        | ⁉️              | ⁉️              | Can only be dumped/loaded as binary. Editing is possible with [GSC-Tool](https://github.com/xensik/gsc-tool). Plutonium supports loading source files however. |
| KeyValuePairs          | ✅              | ✅              | Is dumped/loaded as part of the `.zone` file.                                                                                                                  |
| VehicleDef             | ✅              | ✅              |                                                                                                                                                                |
| MemoryBlock            | ❌              | ❌              |                                                                                                                                                                |
| AddonMapEnts           | ❌              | ❌              |                                                                                                                                                                |
| TracerDef              | ✅              | ✅              |                                                                                                                                                                |
| SkinnedVertsDef        | ❌              | ❌              |                                                                                                                                                                |
| Qdb                    | ⁉️              | ⁉️              | Dumping/Loading is implemented as rawfiles. Their use is currently unknown.                                                                                    |
| Slug                   | ⁉️              | ⁉️              | Dumping/Loading is implemented as rawfiles. Their use is currently unknown.                                                                                    |
| FootstepTableDef       | ❌              | ❌              |                                                                                                                                                                |
| FootstepFXTableDef     | ❌              | ❌              |                                                                                                                                                                |
| ZBarrierDef            | ✅              | ✅              |                                                                                                                                                                |

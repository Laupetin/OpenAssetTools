import { getBinds } from "@webwindowed/web-api";

export type CommonAssetType =
  | "phys_preset"
  | "xanim"
  | "xmodel"
  | "material"
  | "technique_set"
  | "image"
  | "sound"
  | "sound_curve"
  | "loaded_sound"
  | "clip_map"
  | "com_world"
  | "game_world_sp"
  | "game_world_mp"
  | "map_ents"
  | "gfx_world"
  | "light_def"
  | "ui_map"
  | "font"
  | "menu_list"
  | "menu"
  | "localize_entry"
  | "weapon"
  | "sound_driver_globals"
  | "fx"
  | "impact_fx"
  | "ai_type"
  | "mp_type"
  | "character"
  | "xmodel_alias"
  | "raw_file"
  | "string_table"
  | "xmodel_pieces"
  | "phys_coll_map"
  | "xmodel_surfs"
  | "pixel_shader"
  | "vertex_shader"
  | "vertex_decl"
  | "fx_world"
  | "leaderboard"
  | "structured_data_def"
  | "tracer"
  | "vehicle"
  | "addon_map_ents"
  | "glass_world"
  | "path_data"
  | "vehicle_track"
  | "attachment"
  | "surface_fx"
  | "script"
  | "phys_constraints"
  | "destructible_def"
  | "sound_patch"
  | "weapon_def"
  | "weapon_variant"
  | "mp_body"
  | "mp_head"
  | "pack_index"
  | "xglobals"
  | "ddl"
  | "glasses"
  | "emblem_set"
  | "font_icon"
  | "weapon_full"
  | "attachment_unique"
  | "weapon_camo"
  | "key_value_pairs"
  | "memory_block"
  | "skinned_verts"
  | "qdb"
  | "slug"
  | "footstep_table"
  | "footstep_fx_table"
  | "zbarrier"
  | "xml_tree"
  | "scene_animation"
  | "cutscene"
  | "custom_camera";

export interface AssetDto {
  type: CommonAssetType;
  name: string;
}

export interface ZoneAssetsDto {
  assets: AssetDto[];
  references: AssetDto[];
}

type AssetBinds = {
  getAssetsForZone(zoneName: string): Promise<ZoneAssetsDto | null>;
};

export const { getAssetsForZone } = getBinds<AssetBinds>();

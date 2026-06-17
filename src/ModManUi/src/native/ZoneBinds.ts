import { getBinds } from "@webwindowed/web-api";

export type GameId = "iw3" | "iw4" | "iw5" | "t4" | "t5" | "t6";

export type GamePlatform = "pc" | "xbox" | "ps3";

export interface ZoneDto {
  name: string;
  filePath: string;
  game: GameId;
  platform: GamePlatform;
}

export interface ZoneLoadProgressDto {
  zoneName: string;
  /**
   * Between 0-100
   */
  percentage: number;
}

export interface ZoneLoadedDto {
  zone: ZoneDto;
}

export interface ZoneUnloadedDto {
  zoneName: string;
}

type ZoneBinds = {
  getZones(): Promise<ZoneDto[]>;
  loadFastFile(path: string): Promise<ZoneLoadedDto>;
  unloadZone(zoneName: string): Promise<void>;
};

export type ZoneEventMap = {
  zoneLoadProgress: ZoneLoadProgressDto;
  zoneLoaded: ZoneLoadedDto;
  zoneUnloaded: ZoneUnloadedDto;
};

export const { getZones, loadFastFile, unloadZone } = getBinds<ZoneBinds>();

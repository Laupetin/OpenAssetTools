export enum GameId {
  IW3 = "IW3",
  IW4 = "IW4",
  IW5 = "IW5",
  T5 = "T5",
  T6 = "T6",
}

export enum GamePlatform {
  PC = "PC",
  XBOX = "XBOX",
  PS3 = "PS3",
}

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

export interface ZoneBinds {
  getZones(): Promise<ZoneDto[]>;
  loadFastFile(path: string): Promise<ZoneLoadedDto>;
  unloadZone(zoneName: string): Promise<void>;
}

export interface ZoneEventMap {
  zoneLoadProgress: ZoneLoadProgressDto;
  zoneLoaded: ZoneLoadedDto;
  zoneUnloaded: ZoneUnloadedDto;
}

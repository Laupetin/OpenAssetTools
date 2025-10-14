export interface ZoneLoadProgressDto {
  zoneName: string;
  percentage: number;
}

export interface ZoneLoadedDto {
  zoneName: string;
  filePath: string;
}

export interface ZoneUnloadedDto {
  zoneName: string;
}

export interface ZoneBinds {
  loadFastFile(path: string): Promise<ZoneLoadedDto>;
  unloadZone(zoneName: string): Promise<void>;
}

export interface ZoneEventMap {
  zoneLoadProgress: ZoneLoadProgressDto;
  zoneLoaded: ZoneLoadedDto;
  zoneUnloaded: ZoneUnloadedDto;
}

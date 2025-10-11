export interface ZoneLoadedDto {
  zoneName: string;
  filePath: string;
}

export interface ZoneUnloadedDto {
  zoneName: string;
}

export interface ZoneBinds {
  loadFastFile(path: string): Promise<void>;
  unloadZone(zoneName: string): Promise<void>;
}

export interface ZoneEventMap {
  zoneLoaded: ZoneLoadedDto;
  zoneUnloaded: ZoneUnloadedDto;
}

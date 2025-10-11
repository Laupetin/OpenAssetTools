export interface ZoneLoadedDto {
  zoneName: string;
  filePath: string;
}

export interface ZoneUnloadedDto {
  zoneName: string;
}

export interface FastFileBinds {
  loadFastFile(path: string): Promise<void>;
  unloadZone(zoneName: string): Promise<void>;
}

export interface FastFileEventMap {
  zoneLoaded: ZoneLoadedDto;
  zoneUnloaded: ZoneUnloadedDto;
}

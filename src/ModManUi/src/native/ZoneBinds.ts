export interface ZoneDto {
  name: string;
  filePath: string;
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

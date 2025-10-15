export interface ZoneUnlinkProgressDto {
  zoneName: string;
  percentage: number;
}

export interface UnlinkingBinds {
  unlinkZone(zoneName: string): Promise<void>;
}

export interface UnlinkingEventMap {
  zoneUnlinkProgress: ZoneUnlinkProgressDto;
}

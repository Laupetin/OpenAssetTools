export interface ZoneUnlinkProgressDto {
  zoneName: string;
  /**
   * Between 0-100
   */
  percentage: number;
}

export interface UnlinkingBinds {
  unlinkZone(zoneName: string): Promise<void>;
}

export interface UnlinkingEventMap {
  zoneUnlinkProgress: ZoneUnlinkProgressDto;
}

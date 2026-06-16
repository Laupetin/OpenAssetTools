import { getBinds } from "@webwindowed/web-api";

export interface ZoneUnlinkProgressDto {
  zoneName: string;
  /**
   * Between 0-100
   */
  percentage: number;
}

type UnlinkingBinds = {
  unlinkZone(zoneName: string): Promise<void>;
};

export type UnlinkingEventMap = {
  zoneUnlinkProgress: ZoneUnlinkProgressDto;
};

export const { unlinkZone } = getBinds<UnlinkingBinds>();

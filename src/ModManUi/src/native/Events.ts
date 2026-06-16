import { getEvents } from "@webwindowed/web-api";
import type { UnlinkingEventMap } from "@/native/UnlinkingBinds.ts";
import type { ZoneEventMap } from "@/native/ZoneBinds.ts";

type NativeEventMap = UnlinkingEventMap & ZoneEventMap;
export const { addEventListener, removeEventListener } = getEvents<NativeEventMap>();

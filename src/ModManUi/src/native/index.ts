import type { AssetBinds } from "./AssetBinds";
import type { DialogBinds } from "./DialogBinds";
import type { UnlinkingBinds, UnlinkingEventMap } from "./UnlinkingBinds";
import type { ZoneBinds, ZoneEventMap } from "./ZoneBinds";

export type NativeMethods = AssetBinds & DialogBinds & UnlinkingBinds & ZoneBinds;

type NativeEventMap = UnlinkingEventMap & ZoneEventMap;

type WebWindowedExtensions = {
  webwindowedBinds: NativeMethods;
  webwindowedAddEventListener<K extends keyof NativeEventMap>(
    eventKey: K,
    callback: (payload: NativeEventMap[K]) => void,
  ): void;
  webwindowedRemoveEventListener<K extends keyof NativeEventMap>(
    eventKey: K,
    callback: (payload: NativeEventMap[K]) => void,
  ): boolean;
};

const windowWithWebWindowedExtensions = window as typeof window & WebWindowedExtensions;

export const webwindowedBinds = windowWithWebWindowedExtensions.webwindowedBinds;
export const webwindowedAddEventListener =
  windowWithWebWindowedExtensions.webwindowedAddEventListener;
export const webwindowedRemoveEventListener =
  windowWithWebWindowedExtensions.webwindowedRemoveEventListener;

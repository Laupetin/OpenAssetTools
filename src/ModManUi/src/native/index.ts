import type { AssetBinds } from "./AssetBinds";
import type { DialogBinds } from "./DialogBinds";
import type { UnlinkingBinds, UnlinkingEventMap } from "./UnlinkingBinds";
import type { ZoneBinds, ZoneEventMap } from "./ZoneBinds";

export type NativeMethods = AssetBinds & DialogBinds & UnlinkingBinds & ZoneBinds;

type NativeEventMap = UnlinkingEventMap & ZoneEventMap;

type WebViewExtensions = {
  webviewBinds: NativeMethods;
  webviewAddEventListener<K extends keyof NativeEventMap>(
    eventKey: K,
    callback: (payload: NativeEventMap[K]) => void,
  ): void;
  webviewRemoveEventListener<K extends keyof NativeEventMap>(
    eventKey: K,
    callback: (payload: NativeEventMap[K]) => void,
  ): boolean;
};

const windowWithWebViewExtensions = window as typeof window & WebViewExtensions;

export const webviewBinds = windowWithWebViewExtensions.webviewBinds;
export const webviewAddEventListener = windowWithWebViewExtensions.webviewAddEventListener;
export const webviewRemoveEventListener = windowWithWebViewExtensions.webviewRemoveEventListener;

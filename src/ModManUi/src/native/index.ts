import type { DialogBinds } from "./DialogBinds";


export type NativeMethods = DialogBinds;

interface NativeEventMap {
  
}

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

export interface NativeMethods {
  greet: (name: string) => Promise<string>;
}

interface NativeEventMap {
  greeting: string;
}

type WebViewExtensions = {
  webviewBinds: NativeMethods;
  webViewAddEventListener<K extends keyof NativeEventMap>(
    eventKey: K,
    callback: (payload: NativeEventMap[K]) => void,
  ): void;
  webViewRemoveEventListener<K extends keyof NativeEventMap>(
    eventKey: K,
    callback: (payload: NativeEventMap[K]) => void,
  ): boolean;
};

const windowWithWebViewExtensions = window as typeof window & WebViewExtensions;

export const webviewBinds = windowWithWebViewExtensions.webviewBinds;
export const webViewAddEventListener = windowWithWebViewExtensions.webViewAddEventListener;
export const webViewRemoveEventListener = windowWithWebViewExtensions.webViewRemoveEventListener;

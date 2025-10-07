export interface NativeMethods {
  greet: (name: string) => Promise<string>;
}

const windowWithWebViewExtensions = window as typeof window & {webview_binds: NativeMethods};

export const nativeMethods: NativeMethods = windowWithWebViewExtensions.webview_binds;

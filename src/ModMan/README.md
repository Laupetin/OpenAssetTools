# ModMan

ModMan is the experimental GUI for OpenAssetTools.

## How do I test it

Currently ModMan is not compiled by default.
To enable it, you have to generate with the appropriate premake5 flag:

```shell
# On Windows
./generate.bat --modman

# On Linux
./generate.sh --modman
```

**Before** building the C++ solution, the ui has to be built.
This will require NodeJS to be installed on your machine.

```shell
# Download dependencies
npm --prefix src/ModManUi install

# Build frontend
npm --prefix src/ModManUi run build

# Optional: Dev Server for UI development
npm --prefix src/ModManUi run dev
```

## How does it work

ModMan uses [`webview`](https://github.com/Laupetin/webview) for providing a web frontend as a native application.
Unlike frameworks like Electron this does not ship a browser engine alongside it, but instead relies on browser APIs of your OS.
On Windows, this makes use of [WebView2](https://learn.microsoft.com/en-us/microsoft-edge/webview2), on Linux it uses [WebKitGTK](https://webkitgtk.org).

This adds the following dependencies:
* **Windows**: An up-to-date OS with at the very least Windows10. The WebView2 library for development is downloaded by premake.
* **Linux**: Developing and using ModMan requires the following dependencies to be installed: `gtk4 webkitgtk-6.0`


import { fileURLToPath, URL } from "node:url";

import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import vueDevTools from "vite-plugin-vue-devtools";
import PluginCppHeader from "@webwindowed/vite-plugin-cpp-header";

// https://vite.dev/config/
export default defineConfig({
  build: {
    emptyOutDir: true,
    rolldownOptions: {
      output: {
        assetFileNames: "[name][extname]",
        entryFileNames: "[name].js",
        chunkFileNames: "[name].js",
      },
    },
  },
  publicDir: fileURLToPath(new URL("./public", import.meta.url)),
  plugins: [
    vue(),
    vueDevTools(),
    PluginCppHeader({
      outputPath: fileURLToPath(
        new URL("../../build/src/ModMan/Web/ViteAssets.h", import.meta.url),
      ),
    }),
  ],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./src", import.meta.url)),
      "@style": fileURLToPath(new URL("./src/style", import.meta.url)),
    },
  },
});

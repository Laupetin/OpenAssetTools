import { fileURLToPath, URL } from "node:url";

import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import vueDevTools from "vite-plugin-vue-devtools";
import headerTransformationPlugin from "./build/HeaderTransformationPlugin";

// https://vite.dev/config/
export default defineConfig({
  build: {
    copyPublicDir: false,
    emptyOutDir: true,
    rollupOptions: {
      output: {
        assetFileNames: "[name][extname]",
        entryFileNames: "[name].js",
        chunkFileNames: "[name].js",
      },
    },
  },
  plugins: [
    vue(),
    vueDevTools(),
    headerTransformationPlugin({
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

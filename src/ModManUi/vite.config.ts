import { fileURLToPath, URL } from "node:url";

import { defineConfig } from "vite";
import vue from "@vitejs/plugin-vue";
import vueDevTools from "vite-plugin-vue-devtools";
import { headerTransformationPlugin } from "./build/HeaderTransformationPlugin";

// https://vite.dev/config/
export default defineConfig({
  build: {
    outDir: "../../build/src/ModMan/ui",
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
  plugins: [vue(), vueDevTools(), headerTransformationPlugin()],
  resolve: {
    alias: {
      "@": fileURLToPath(new URL("./src", import.meta.url)),
    },
  },
  server: {
    port: 1420,
    strictPort: true,
  },
});

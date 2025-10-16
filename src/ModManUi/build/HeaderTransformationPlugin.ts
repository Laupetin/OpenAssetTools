import type { Plugin } from "vite";
import type { OutputAsset, OutputChunk } from "rollup";
import path from "node:path";
import fs from "node:fs";

type MinimalOutputAsset = Pick<OutputAsset, "type" | "fileName" | "source">;
type MinimalOutputChunk = Pick<OutputChunk, "type" | "fileName" | "code">;
type MinimalOutputBundle = Record<string, MinimalOutputAsset | MinimalOutputChunk>;

function createVarName(fileName: string) {
  return fileName.replaceAll(/[\.-]/g, "_").toUpperCase();
}

function transformAsset(asset: MinimalOutputAsset) {
  const varName = createVarName(asset.fileName);

  let bytes: string;
  if (typeof asset.source === "string") {
    bytes = [...asset.source].map((v) => String(v.charCodeAt(0))).join(",");
  } else {
    bytes = [...asset.source].map((v) => String(v)).join(",");
  }

  return `constexpr const unsigned char ${varName}[] {${bytes}};
`;
}

function transformChunk(chunk: MinimalOutputChunk) {
  const varName = createVarName(chunk.fileName);
  const bytes = [...chunk.code].map((v) => String(v.charCodeAt(0))).join(",");

  return `constexpr const unsigned char ${varName}[] {${bytes}};
`;
}

function writeHeader(
  bundle: MinimalOutputBundle,
  outputDir?: string,
  options?: HeaderTransformationPluginOptions,
  devServerPort?: number,
) {
  const outputPath = options?.outputPath ?? path.join(outputDir ?? "dist", "ViteAssets.h");
  const outputPathParentDir = path.dirname(outputPath);

  fs.mkdirSync(outputPathParentDir, { recursive: true });

  const fd = fs.openSync(outputPath, "w");
  const includeFileEnumeration = options?.includeFileEnumeration ?? true;

  fs.writeSync(
    fd,
    `#pragma once

`,
  );

  if (includeFileEnumeration) {
    fs.writeSync(
      fd,
      `#include <cstdlib>
#include <type_traits>

`,
    );
  }

  fs.writeSync(
    fd,
    `constexpr auto VITE_DEV_SERVER = ${devServerPort ? "true" : "false"};
constexpr auto VITE_DEV_SERVER_PORT = ${devServerPort ? String(devServerPort) : "-1"};
`,
  );

  for (const curBundle of Object.values(bundle)) {
    if (curBundle.type === "asset") {
      fs.writeSync(fd, transformAsset(curBundle));
    } else {
      fs.writeSync(fd, transformChunk(curBundle));
    }
  }

  if (includeFileEnumeration) {
    fs.writeSync(
      fd,
      `
struct UiFile
{
    const char* filename;
    const void* data;
    const size_t dataSize;
};

static inline const UiFile MOD_MAN_UI_FILES[] {
`,
    );

    let index = 0;
    for (const curBundle of Object.values(bundle)) {
      const fileName = curBundle.fileName;
      const varName = createVarName(fileName);

      let prefix = "  ";
      if (index > 0) {
        prefix = `,
  `;
      }

      fs.writeSync(
        fd,
        `${prefix}{ "${fileName}", ${varName}, std::extent_v<decltype(${varName})> }`,
      );
      index++;
    }

    fs.writeSync(
      fd,
      `
};
`,
    );

    fs.closeSync(fd);
  }
}

export interface HeaderTransformationPluginOptions {
  outputPath?: string;
  includeFileEnumeration?: boolean;
}

export default function headerTransformationPlugin(
  options?: HeaderTransformationPluginOptions,
): Plugin {
  let writeServerActive = false;
  let writeBundleActive = false;

  return {
    name: "vite-plugin-header-transformation",
    enforce: "post",
    config(_userOptions, env) {
      if (env.command === "serve") {
        writeServerActive = true;
      } else {
        writeBundleActive = true;
      }
    },
    configureServer(server) {
      if (!writeServerActive) {
        return;
      }
      server.httpServer?.once("listening", () => {
        writeHeader(
          {
            // We need at least one array entry for MSVC
            dummyfile: { type: "chunk", fileName: "dummyfile", code: "dummy" },
          },
          server.config.build.outDir,
          options,
          server.config.server.port,
        );
      });
    },
    writeBundle(outputOptions, bundle) {
      if (!writeBundleActive) {
        return;
      }

      writeHeader(bundle, outputOptions.dir, options);
    },
  };
}

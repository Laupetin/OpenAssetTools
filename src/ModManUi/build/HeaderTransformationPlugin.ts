import type { Plugin, ViteDevServer } from "vite";
import type { OutputOptions, OutputBundle, OutputAsset, OutputChunk } from "rollup";
import { createHmac } from "node:crypto";
import path from "node:path";
import fs from "node:fs";

function createTransformedTextSource2(varName: string, previousSource: string) {
  const hash = createHmac("sha256", previousSource);
  const digest = hash.digest("hex").substring(0, 16);

  return `#pragma once
  
static inline const char* ${varName} = R"${digest}(${previousSource})${digest}";
`;
}

function createTransformedTextSource(varName: string, previousSource: string) {
  const str = [...previousSource].map((v) => `0x${v.charCodeAt(0).toString(16).padStart(2, "0")}`).join(", ");
  return `#pragma once
  
static inline const unsigned char ${varName}[] {
${str}
};
`;
}

function createVarName(fileName: string) {
  return fileName.replaceAll(".", "_").toUpperCase();
}

function transformAsset(asset: OutputAsset) {
  const varName = createVarName(asset.names[0]);

  if (typeof asset.source === "string") {
    asset.source = createTransformedTextSource(varName, asset.source);
    return `${varName}, std::extent_v<decltype(${varName})>`;
    // return `${varName}, std::char_traits<char>::length(${varName})`;
  } else {
    const str = [...asset.source].map((v) => `0x${v.toString(16).padStart(2, "0")}`).join(", ");
    asset.source = `#pragma once
  
static inline const unsigned char ${varName}[] {
${str}
};
`;
    return `${varName}, std::extent_v<decltype(${varName})>`;
  }
}

function transformChunk(chunk: OutputChunk) {
  const varName = createVarName(chunk.fileName);
  chunk.code = createTransformedTextSource(varName, chunk.code);
    return `${varName}, std::extent_v<decltype(${varName})>`;
//   return `${varName}, std::char_traits<char>::length(${varName})`;
}

export function headerTransformationPlugin(): Plugin {
  return {
    name: "header-transformation",
    apply: "build",
    config(config) {
      config.base = "http://modman-resource/";
    },
    generateBundle(options: OutputOptions, bundle: OutputBundle, isWrite: boolean) {
      const includesStr: string[] = [];
      const uiFilesStr: string[] = [];
      for (const curBundle of Object.values(bundle)) {
        let varStr: string;
        if (curBundle.type === "asset") {
          varStr = transformAsset(curBundle);
        } else {
          varStr = transformChunk(curBundle);
        }

        includesStr.push(`#include "${curBundle.fileName}.h"`);
        uiFilesStr.push(`{ "${curBundle.fileName}", { "${curBundle.fileName}", ${varStr} } }`);

        curBundle.fileName = `${curBundle.fileName}.h`;
      }
      this.emitFile({
        type: "asset",
        fileName: "modmanui.h",
        source: `#pragma once

#include "index.html.h"
${includesStr.join("\n")}

#include <string>
#include <unordered_map>

struct UiFile
{
    const char* filename;
    const void* data;
    const size_t dataSize;
};

static inline const std::unordered_map<std::string, UiFile> MOD_MAN_UI_FILES({
${uiFilesStr.join(",\n")}
});
`,
      });
    },
    transformIndexHtml(
      html: string,
      ctx: {
        path: string;
        filename: string;
        server?: ViteDevServer;
        bundle?: OutputBundle;
        chunk?: OutputChunk;
      },
    ) {
      html = html
        .replaceAll("index.js.h", "index.js")
        .replaceAll("http://modman-resource/", "modman-resource://");

      html = createTransformedTextSource(createVarName("index.html"), html);
      ctx.filename = `${ctx.filename}.h`;

      return html;
    },
    writeBundle(options, bundle) {
      for (const curBundle of Object.values(bundle)) {
        if (curBundle.fileName === "index.html" && curBundle.type === "asset") {
          const outputFilePath = path.join(options.dir!, curBundle.fileName);
          fs.renameSync(outputFilePath, outputFilePath + ".h");
          curBundle.fileName += ".h";
        }
      }

      this.emitFile({
        type: "asset",
        fileName: "index.h",
        source: `
        <!DOCTYPE html>
        <html>
        <head>
          <meta charset="UTF-8">
          <title>Title</title>
         </head>
        <body>
          <script src="" type="module"></script>
          <script src="" type="module"></script>
          <script src="" type="module"></script>
        </body>
        </html>`,
      });
    },
  };
}

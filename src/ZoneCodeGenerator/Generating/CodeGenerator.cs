using System;
using System.Collections.Generic;
using System.IO;
using ZoneCodeGenerator.Domain.StructureInformation;
using ZoneCodeGenerator.Interface;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Generating
{
    static class CodeGenerator
    {
        private class GeneratorPreset
        {
            public string FileName { get; }
            public string TemplateName { get; }

            public GeneratorPreset(string fileName, string templateName)
            {
                FileName = fileName;
                TemplateName = templateName;
            }
        }

        private static readonly Dictionary<string, GeneratorPreset> presets =
            new Dictionary<string, GeneratorPreset>(StringComparer.CurrentCultureIgnoreCase)
            {
                {
                    "ZoneLoad", new GeneratorPreset("$asset/$asset_load_db", "ZoneLoad.stg")
                },
                {
                    "ZoneWrite", new GeneratorPreset("$asset/$asset_write_db", "ZoneWrite.stg")
                },
                {
                    "AssetStructTests", new GeneratorPreset("$asset_struct_test", "AssetStructTests.stg")
                }
            };

        public static IEnumerable<string> Presets => presets.Keys;

        public static bool GenerateCodeForPreset(string presetName, StructureInformation asset, CUISession session)
        {
            if (!presets.ContainsKey(presetName))
                return false;

            var preset = presets[presetName];

            try
            {
                if (!Directory.Exists(session.GeneratorOutputPath))
                    Directory.CreateDirectory(session.GeneratorOutputPath);

                var codeTemplate = CodeTemplate.FromResources(preset.TemplateName);

                if (codeTemplate == null)
                {
                    Console.WriteLine($"Could not find template '{preset.TemplateName}'");
                    return false;
                }

                var resolvedFileName = preset.FileName.Replace("$asset", asset.Type.Name.ToLower());
                var fullPath = Path.Combine(session.GeneratorOutputPath, resolvedFileName);
                var folderOfFile = Path.GetDirectoryName(fullPath);

                if (!Directory.Exists(folderOfFile))
                    Directory.CreateDirectory(folderOfFile);

                var renderingContext = RenderingContext.BuildContext(session, asset);

                if (renderingContext == null)
                    return false;

                var generatedCode = false;
                if (codeTemplate.HasHeaderTemplate)
                {
                    generatedCode = true;
                    using (var fileStream = new FileStream(fullPath + ".h", FileMode.Create))
                    {
                        codeTemplate.RenderHeaderFile(fileStream, renderingContext);
                        fileStream.Close();
                    }
                }

                if (codeTemplate.HasSourceTemplate)
                {
                    generatedCode = true;
                    using (var fileStream = new FileStream(fullPath + ".cpp", FileMode.Create))
                    {
                        codeTemplate.RenderSourceFile(fileStream, renderingContext);
                        fileStream.Close();
                    }
                }

                return generatedCode;
            }
            catch (Exception)
            {
                return false;
            }
        }
    }
}
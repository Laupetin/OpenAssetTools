using System;
using System.IO;
using System.Reflection;
using Antlr.Runtime;
using Antlr4.StringTemplate;
using Antlr4.StringTemplate.Compiler;
using Antlr4.StringTemplate.Misc;

namespace ZoneCodeGenerator.Generating
{
    public class TemplateGroupResources : TemplateGroup
    {
        private const string ResourceBasePath = "ZoneCodeGenerator.Generating.Templates";
        private const char DefaultDelimiterStart = '<';
        private const char DefaultDelimiterStop = '>';

        private bool alreadyLoaded;
        private readonly string fileName;

        public TemplateGroupResources(string fileName, char delimiterStartChar, char delimiterStopChar) : base(delimiterStartChar, delimiterStopChar)
        {
            this.fileName = fileName;
            alreadyLoaded = false;
        }

        public TemplateGroupResources(string fileName) : this(fileName, DefaultDelimiterStart, DefaultDelimiterStop)
        {

        }

        public override void ImportTemplates(IToken fileNameToken)
        {
            var text = Utility.Strip(fileNameToken.Text, 1);

            TemplateGroup importingGroup = null;
            if (text.EndsWith(GroupFileExtension))
            {
                importingGroup = new TemplateGroupResources(text, DelimiterStartChar, DelimiterStopChar);
            }
            else if (text.EndsWith(TemplateFileExtension))
            {
                importingGroup = new TemplateGroup(DelimiterStartChar, DelimiterStopChar);

                using (var resourceStream = StreamFromResources(text))
                {
                    if (resourceStream != null)
                    {
                        using (var textStream = new StreamReader(resourceStream))
                        {
                            var inputStream = new ANTLRReaderStream(textStream)
                            {
                                name = fileName
                            };

                            importingGroup.LoadTemplateFile("/", fileName, inputStream);
                        }
                    }
                }
            }

            if (importingGroup != null)
            {
                importingGroup.Load();
                ImportTemplates(importingGroup);
            }
            else
            {
                ErrorManager.CompiletimeError(ErrorType.CANT_IMPORT, null, fileNameToken, text);
            }
        }

        public override void Load()
        {
            if (alreadyLoaded)
                return;
            alreadyLoaded = true;

            if (Verbose)
                Console.WriteLine($"loading group file '{fileName}'");
            
            using (var resourceStream = StreamFromResources(fileName))
            {
                if (resourceStream == null)
                {
                    Console.WriteLine($"Resource '{fileName}' doesn't exist");
                    Console.WriteLine("The following files do exist: " + string.Join(", ", Assembly.GetExecutingAssembly().GetManifestResourceNames()));

                    return;
                }

                using (var textStream = new StreamReader(resourceStream))
                {
                    var inputStream = new ANTLRReaderStream(textStream)
                    {
                        name = fileName
                    };

                    try
                    {
                        var lexer = new GroupLexer(inputStream);
                        var tokenStream = new CommonTokenStream(lexer);
                        var parser = new GroupParser(tokenStream);

                        parser.group(this, "/");
                    }
                    catch (Exception ex)
                    {
                        ErrorManager.IOError(null, ErrorType.CANT_LOAD_GROUP_FILE, ex, FileName);
                    }
                }
            }
        }

        private static Stream StreamFromResources(string fileName)
        {
            var resourceName = $"{ResourceBasePath}.{fileName}";
            var assembly = Assembly.GetExecutingAssembly();

            return assembly.GetManifestResourceStream(resourceName);
        }

        public override string Name => "TemplateGroupResources";

        public override string FileName => Name;
    }
}

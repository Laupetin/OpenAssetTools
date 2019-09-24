using System;
using System.IO;
using System.Linq;
using ZoneCodeGenerator.Interface;
using ZoneCodeGenerator.Parsing.CommandFile.Impl;
using ZoneCodeGenerator.Parsing.CommandFile.PostProcessor;
using ZoneCodeGenerator.Parsing.Impl;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile
{
    static class CommandFileReader
    {
        private static readonly IDataPostProcessor[] postProcessors =
        {
            new PostProcessorDefaultBlock(),
        };

        public static bool ReadFile(string path, CUISession session)
        {
            try
            {
                using (IIncludingParsingStream streamFileSystem = new IncludingStreamFileSystem(path))
                {
                    var state = new CommandParserState(session.Repository);

                    using (IParsingFileStream preprocessorStream = new CommandFilePreprocessor(streamFileSystem))
                    {
                        var lexer = new Lexer(preprocessorStream);
                        var parser = new Parser<ICommandParserState>(state, lexer);

                        if (!parser.Parse())
                            return false;

                        preprocessorStream.Close();
                        streamFileSystem.Close();

                        if (!state.Apply(session))
                        {
                            Console.WriteLine("Finalizing from command file failed");
                            return false;
                        }

                        if (!PostProcessRepository(session.Repository))
                        {
                            Console.WriteLine("Postprocessing commands failed.");
                            return false;
                        }
                    }
                    
                    return true;
                }
            }
            catch (IOException)
            {

            }

            return false;
        }
        
        private static bool PostProcessRepository(IDataRepository repository)
        {
            return postProcessors.All(postProcessor => postProcessor.PostProcess(repository));
        }
    }
}

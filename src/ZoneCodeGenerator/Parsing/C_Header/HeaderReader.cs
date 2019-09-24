using System;
using System.IO;
using System.Linq;
using ZoneCodeGenerator.Parsing.C_Header.Impl;
using ZoneCodeGenerator.Parsing.C_Header.PostProcessor;
using ZoneCodeGenerator.Parsing.Impl;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.C_Header
{
    static class HeaderReader
    {
        private static readonly IDataPostProcessor[] postProcessors =
        {
            new PostProcessorUsages(), 
        };

        public static IDataRepository ReadFile(string path)
        {
            try
            {
                using (IIncludingParsingStream streamFileSystem = new IncludingStreamFileSystem(path))
                {
                    var state = new HeaderParserState();
                    IDataRepository dataRepository;

                    using (IParsingFileStream preprocessorStream = new Preprocessor(streamFileSystem, state))
                    {
                        var lexer = new Lexer(preprocessorStream);
                        var parser = new Parser<HeaderParserState>(state, lexer);

                        if (!parser.Parse())
                            return null;
                    
                        dataRepository = new InMemoryDataRepository();
                        try
                        {
                            state.FinishAndSaveTo(dataRepository);
                        }
                        catch (LoadingException e)
                        {
                            PrintFinishingError(e);
                            return null;
                        }

                        preprocessorStream.Close();
                        streamFileSystem.Close();

                    }

                    if (!PostProcessRepository(dataRepository))
                    {
                        Console.WriteLine("Postprocessing data failed.");
                        return null;
                    }
                    
                    return dataRepository;
                }
            }
            catch (IOException)
            {
                return null;
            }
        }

        private static bool PostProcessRepository(IDataRepository repository)
        {
            return postProcessors.All(postProcessor => postProcessor.PostProcess(repository));
        }

        private static void PrintFinishingError(LoadingException e)
        {
            Console.WriteLine("Parsing failed.");
            
            Console.WriteLine(e.Message);
            Console.WriteLine();
        }
    }
}

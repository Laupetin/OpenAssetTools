using System.Collections.Generic;
using ZoneCodeGenerator.Parsing;

namespace ZoneCodeGeneratorTests.Parsing.Mock
{
    class IncludingParsingStreamTest : IIncludingParsingStream
    {
        public string Filename { get; set; }
        public int Line { get; set; }
        public List<string> Lines { get; }
        public bool EndOfStream => Line >= Lines.Count;
        public string LastInclude { get; private set; }
        public int IncludeCount { get; private set; }

        public IncludingParsingStreamTest(string filename)
        {
            Line = 0;
            Filename = filename;
            Lines = new List<string>();
            LastInclude = "";
            IncludeCount = 0;
        }

        public string ReadLine()
        {
            return EndOfStream ? "" : Lines[Line++];
        }

        public void IncludeFile(string filename)
        {
            LastInclude = filename;
            IncludeCount++;
        }

        public void Close()
        {
            
        }

        public void Dispose()
        {
            Close();
        }
    }
}

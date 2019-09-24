using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Interface
{
    class CUISession
    {
        public string Game { get; set; }
        public IDataRepository Repository { get; set; }
        public string GeneratorOutputPath { get; set; }
        public string SourceFilePath { get; set; }

        public CUISession()
        {
            Repository = null;
            GeneratorOutputPath = ".";
        }
    }
}

using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing
{
    interface IDataPostProcessor
    {
        bool PostProcess(IDataRepository repository);
    }
}

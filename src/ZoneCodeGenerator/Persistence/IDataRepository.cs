using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;

namespace ZoneCodeGenerator.Persistence
{
    interface IDataRepository : IReadOnlyDataRepository
    {
        void Add(DataTypeEnum data);
        void Add(DataTypeStruct data);
        void Add(DataTypeUnion data);
        void Add(DataTypeTypedef data);
        void Add(FastFileBlock block);
    }
}

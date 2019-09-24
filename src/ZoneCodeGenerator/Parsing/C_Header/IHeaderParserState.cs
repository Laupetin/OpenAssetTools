using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.C_Header
{
    interface IHeaderParserState : IParserState<IHeaderParserState>
    {
        Namespace CurrentNamespace { get; }
        int CurrentPack { get; }
        Block CurrentBlock { get; }
        Block PreviousBlock { get; }

        void PushPack(int pack);
        int PopPack();

        void PushBlock(Block block);
        Block PopBlock();

        void AddDataType(DataType dataType);
        void AddForwardDeclaration(ForwardDeclaration forwardDeclaration);

        DataType FindType(string typename);
        EnumMember FindEnumMember(string enumMemberName);

        void FinishAndSaveTo(IDataRepository dataRepository);
    }
}

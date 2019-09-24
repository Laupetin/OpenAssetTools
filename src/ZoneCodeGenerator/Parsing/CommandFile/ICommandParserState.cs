using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile
{
    interface ICommandParserState : IParserState<ICommandParserState>
    {
        string Game { get; set; }
        IReadOnlyDataRepository Repository { get; }
        List<FastFileBlock> FastFileBlocks { get; }
        DataTypeWithMembers DataTypeInUse { get; set; }
    }
}

using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile
{
    interface ICommandParserState : IParserState<ICommandParserState>
    {
        string Game { get; set; }
        IReadOnlyDataRepository Repository { get; }
        List<FastFileBlock> FastFileBlocks { get; }
        StructureInformation DataTypeInUse { get; set; }

        bool GetMembersFromParts(string[] parts, StructureInformation baseType, out List<MemberInformation> members);
        bool GetTypenameAndMembersFromParts(string[] parts, out StructureInformation typeInformation,
            out List<MemberInformation> members);
    }
}

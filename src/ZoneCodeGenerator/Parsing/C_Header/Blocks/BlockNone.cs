using System.Collections.Generic;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    class BlockNone : Block
    {
        private readonly ITokenTest<IHeaderParserState>[] tests =
        {
            new TestNamespace(),
            new TestEnum(),
            new TestStruct(),
            new TestUnion(),
            new TestTypedef(),
            new TestForwardDecl()
        };

        public BlockNone(IHeaderParserState headerParserState) : base(headerParserState, BlockType.None)
        {

        }

        public override IEnumerable<ITokenTest<IHeaderParserState>> GetAvailableTests()
        {
            return tests;
        }

        public override void OnOpen()
        {
            // Do nothing
        }

        public override void OnClose() 
        {
            // Do nothing
        }

        public override void OnChildBlockClose(Block childBlock)
        {
            // Do nothing
        }
    }
}

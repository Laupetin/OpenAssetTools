using System;
using System.Collections.Generic;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    class BlockNamespace : Block
    {
        private readonly ITokenTest<IHeaderParserState>[] tests =
        {
            new TestNamespace(),
            new TestEnum(),
            new TestStruct(),
            new TestUnion(),
            new TestTypedef(),
            new TestForwardDecl(),
            new TestCloseBlock()
        };

        public string Name { get; }

        public BlockNamespace(IHeaderParserState headerParserState, string namespaceName) : base(headerParserState, BlockType.Namespace)
        {
            Name = namespaceName;
        }

        public override IEnumerable<ITokenTest<IHeaderParserState>> GetAvailableTests()
        {
            return tests;
        }

        public override void OnOpen()
        {
            State.CurrentNamespace.Push(Name);
        }

        public override void OnClose()
        {
            var poppedNamespaceName = State.CurrentNamespace.Pop();

            if (!Name.Equals(poppedNamespaceName))
                throw new Exception($"Popped namespace '{poppedNamespaceName}' does not equal name of namespace block '{Name}'");
        }

        public override void OnChildBlockClose(Block childBlock)
        {
            // Do nothing
        }
    }
}

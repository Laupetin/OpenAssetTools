using System.Collections.Generic;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    abstract class Block
    {
        protected readonly IHeaderParserState State;
        public BlockType Type { get; }

        protected Block(IHeaderParserState headerParserState, BlockType type)
        {
            State = headerParserState;
            Type = type;
        }

        public abstract IEnumerable<ITokenTest<IHeaderParserState>> GetAvailableTests();

        public abstract void OnOpen();

        public abstract void OnClose();

        public abstract void OnChildBlockClose(Block childBlock);
    }
}

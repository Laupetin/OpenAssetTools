using System.Collections.Generic;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing
{
    interface IParserState<in TState> where TState : IParserState<TState>
    {
        IEnumerable<ITokenTest<TState>> GetTests();
    }
}

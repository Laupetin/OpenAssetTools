namespace ZoneCodeGenerator.Parsing.Testing
{
    interface ITokenTest<in TState> where TState : IParserState<TState>
    {
        int ConsumedTokenCount { get; }

        TokenTestResult PerformTest(TState state, ILexer lexer, bool verbose = false);
    }
}

namespace ZoneCodeGenerator.Parsing
{
    interface ILexer
    {
        int CurrentLine { get; }
        string CurrentFile { get; }
        bool IsEndOfStream { get; }
        int CachedTokenCount { get; }

        string NextToken();
        string PeekToken(int index = 0);
        void SkipTokens(int count);
    }
}

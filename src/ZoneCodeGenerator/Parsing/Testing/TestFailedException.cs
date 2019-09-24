namespace ZoneCodeGenerator.Parsing.Testing
{
    class TestFailedException : LoadingException
    {
        public TestFailedException(string message) : base(message)
        {
        }
    }
}

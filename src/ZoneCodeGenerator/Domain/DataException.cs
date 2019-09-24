namespace ZoneCodeGenerator.Domain
{
    class DataException : LoadingException
    {
        public DataException(string message) : base(message)
        {
        }
    }
}

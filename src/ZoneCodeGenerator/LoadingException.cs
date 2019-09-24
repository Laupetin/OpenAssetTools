using System;

namespace ZoneCodeGenerator
{
    class LoadingException : Exception
    {
        public LoadingException(string message) : base(message)
        {
        }
    }
}

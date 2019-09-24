using System.Collections.Generic;

namespace ZoneCodeGenerator.Utils
{
    public static class KeyValueExtension
    {
        public static void Deconstruct<T1, T2>(this KeyValuePair<T1, T2> keyValuePair, out T1 m1, out T2 m2)
        {
            m1 = keyValuePair.Key;
            m2 = keyValuePair.Value;
        }
    }
}

using System.Collections;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace ZoneCodeGeneratorTests
{
    public static class AssertionExtensions
    {
        public static void Contains(this Assert assert, object expected, ICollection collection)
        {
            if (collection.Cast<object>().Any(item => item == expected))
            {
                return;
            }

            throw new AssertFailedException();
        }

        public static void IsZero(this Assert assert, int number)
        {
            if(number != 0)
                throw new AssertFailedException();
        }
    }
}

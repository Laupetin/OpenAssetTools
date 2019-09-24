namespace ZoneCodeGenerator.Utils
{
    static class AlignmentExtension
    {
        public static int Align(this int valueToAlign, int alignment)
        {
            return (valueToAlign + alignment - 1) / alignment * alignment;
        }

        public static long Align(this long valueToAlign, long alignment)
        {
            return (valueToAlign + alignment - 1) / alignment * alignment;
        }
    }
}

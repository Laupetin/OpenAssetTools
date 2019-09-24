namespace ZoneCodeGenerator.Parsing
{
    interface IIncludingParsingStream : IParsingFileStream
    {
        /// <summary>
        /// Pushes back current file and the position in it to read from another file now instead.
        /// After completion returns back to the previous file.
        /// </summary>
        /// <param name="filename"></param>
        void IncludeFile(string filename);
    }
}

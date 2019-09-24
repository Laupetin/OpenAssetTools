using System;

namespace ZoneCodeGenerator.Parsing
{
    interface IParsingFileStream : IDisposable
    {
        /// <summary>
        /// Whether the end of the stream has been reached.
        /// </summary>
        bool EndOfStream { get; }

        /// <summary>
        /// The current filename.
        /// </summary>
        string Filename { get; }
        
        /// <summary>
        /// The current line in the current file.
        /// </summary>
        int Line { get; }

        /// <summary>
        /// Reads a line from the current header and returns it.
        /// </summary>
        /// <returns></returns>
        string ReadLine();

        /// <summary>
        /// Closes this and all underlying streams.
        /// </summary>
        void Close();
    }
}

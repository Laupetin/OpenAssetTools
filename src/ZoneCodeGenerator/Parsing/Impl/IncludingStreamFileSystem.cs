using System.Collections.Generic;
using System.IO;

namespace ZoneCodeGenerator.Parsing.Impl
{
    class IncludingStreamFileSystem : IIncludingParsingStream
    {
        private class HeaderStreamState
        {
            private readonly StreamReader reader;
            private readonly Stream stream;

            public string Cwd { get; }
            public string Filename { get; }
            public int Line { get; private set; }

            public HeaderStreamState(FileStream stream, string filepath)
            {
                this.stream = stream;
                reader = new StreamReader(stream);

                Cwd = Path.GetDirectoryName(filepath);
                Filename = Path.GetFileName(filepath);
                Line = 1;
            }

            public bool EndOfStream => reader.EndOfStream;

            public string ReadLine()
            {
                Line++;
                return reader.ReadLine() ?? "";
            }

            public void Close()
            {
                reader.Close();
                stream.Close();
            }
        }

        private readonly Stack<HeaderStreamState> states;

        public IncludingStreamFileSystem(string path)
        {
            states = new Stack<HeaderStreamState>();
            var initialState = new HeaderStreamState(new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read), path);

            states.Push(initialState);
        }

        public bool EndOfStream => states.Count == 0;

        public string Filename => states.Count == 0 ? "" : State.Filename;

        public int Line => states.Count == 0 ? -1 : State.Line;

        private HeaderStreamState State => states.Count == 0 ? null : states.Peek();

        public string ReadLine()
        {
            var state = State;

            if (state == null)
                return "";

            var line = state.ReadLine();

            if (state.EndOfStream)
            {
                state.Close();
                states.Pop();
            }

            return line;
        }

        public void IncludeFile(string filename)
        {
            var state = State;

            if (state == null)
                return;

            var path = Path.Combine(state.Cwd, filename);

            if (!File.Exists(path)) return;

            var newState = new HeaderStreamState(new FileStream(path, FileMode.Open, FileAccess.Read, FileShare.Read), path);

            states.Push(newState);
        }

        public void Close()
        {
            while (states.Count > 0)
            {
                states.Pop().Close();
            }
        }

        public void Dispose()
        {
            Close();
        }
    }
}

using System;

namespace ZoneCodeGenerator.Parsing.Impl
{
    class CommentProcessor : ICommentProcessor
    {
        private const string LineCommentStart = "//";
        private const string BlockCommentStart = "/*";
        private const string BlockCommentEnd = "*/";
        
        private bool inComment;

        public CommentProcessor()
        {
            inComment = false;
        }

        public string RemoveComments(string line)
        {
            if (inComment)
            {
                var idx = line.IndexOf(BlockCommentEnd, StringComparison.Ordinal);

                if(idx == -1)
                    return "";

                line = line.Substring(idx + BlockCommentEnd.Length);
                inComment = false;
            }

            var commentIndex = line.IndexOf(LineCommentStart, StringComparison.Ordinal);
            if (commentIndex != -1)
                line = line.Remove(commentIndex).Trim();

            while (true)
            {
                var indexBlockCommentBegin = line.IndexOf(BlockCommentStart, StringComparison.Ordinal);

                if (indexBlockCommentBegin == -1)
                    break;

                var indexBlockCommentEnd = line.IndexOf(BlockCommentEnd, indexBlockCommentBegin, StringComparison.Ordinal);

                if(indexBlockCommentEnd == -1)
                {
                    line = line.Remove(indexBlockCommentBegin).Trim();
                    inComment = true;
                    break;
                }

                line = line.Remove(indexBlockCommentBegin, indexBlockCommentEnd + BlockCommentEnd.Length - indexBlockCommentBegin).Trim();
            }

            return line;
        }
    }
}

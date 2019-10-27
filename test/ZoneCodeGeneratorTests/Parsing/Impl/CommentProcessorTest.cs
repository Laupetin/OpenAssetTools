using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.Impl;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGeneratorTests.Parsing.Impl
{
    [TestClass]
    public class CommentPreprocessorTest
    {
        private ICommentProcessor commentProcessor;

        [TestInitialize]
        public void Setup()
        {
            commentProcessor = new CommentProcessor();
        }

        [TestMethod]
        public void EnsureReturnsUnmodifiedText()
        {
            string[] stringsThatShouldNotBeModified =
            {
                "This is a normal string",
                "There is nothing to be preprocessed!",
                "0124124124 # 124124124",
                "...",
                "<?php><html>asdf</html>",
                ""
            };

            foreach (var stringThatShouldNotBeModified in stringsThatShouldNotBeModified)
            {
                Assert.AreEqual(stringThatShouldNotBeModified, commentProcessor.RemoveComments(stringThatShouldNotBeModified));
            }
        }

        [TestMethod]
        public void EnsureDoesRemoveLineComments()
        {
            var commentStrings = new Dictionary<string, string>()
            {
                {"// This is a comment at the beginning of the line", ""},
                {"Text in front of a comment // Comment", "Text in front of a comment"},
                {"/ / Not a comment", "/ / Not a comment"},
                {"asdf /// Triple slash is a comment", "asdf"},
                {"b2h3 /////////////// In fact after the first two slashes it should always be considered a comment", "b2h3"},
            };

            foreach (var (input, expectedResult) in commentStrings)
            {
                Assert.AreEqual(expectedResult, commentProcessor.RemoveComments(input));
            }
        }

        [TestMethod]
        public void EnsureDoesRemoveBlockComments()
        {
            var commentStrings = new Dictionary<string, string>()
            {
                {"/* This is a block comment */", ""},
                {"Text in front of a comment /** Comment ***/", "Text in front of a comment"},
                {"/ * Not a comment */", "/ * Not a comment */"},
                {"Text in front of comment /* Comment */ Text after the comment", "Text in front of comment  Text after the comment"},
                {"Hello/*Hell*/World", "HelloWorld"},
            };

            foreach (var (input, expectedResult) in commentStrings)
            {
                Assert.AreEqual(expectedResult, commentProcessor.RemoveComments(input));
            }
        }

        [TestMethod]
        public void EnsureBlockCommentsWorkOverMultipleLines()
        {
            var commentStrings = new Dictionary<string, string>()
            {
                {"The start of the comment /* Is now", "The start of the comment"},
                {"Nothing to be seen here", ""},
                {"* / /* Still nothing", ""},
                {"The comment ends */ now", " now"},
                {"This line should not cause any issues", "This line should not cause any issues"},
            };

            foreach (var (input, expectedResult) in commentStrings)
            {
                Assert.AreEqual(expectedResult, commentProcessor.RemoveComments(input));
            }
        }
    }
}

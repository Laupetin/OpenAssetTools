using Microsoft.VisualStudio.TestTools.UnitTesting;
using ZoneCodeGenerator.Parsing.CommandFile.Impl;
using ZoneCodeGeneratorTests.Parsing.Mock;

namespace ZoneCodeGeneratorTests.Parsing.CommandFile.Impl
{
    [TestClass]
    public class PreprocessorTest
    {
        private IncludingParsingStreamTest headerStreamTest;

        private CommandFilePreprocessor preprocessor;

        [TestInitialize]
        public void Setup()
        {
            headerStreamTest = new IncludingParsingStreamTest("file.h");
            preprocessor = new CommandFilePreprocessor(headerStreamTest);
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

            headerStreamTest.Lines.AddRange(stringsThatShouldNotBeModified);

            foreach (var stringThatShouldNotBeModified in stringsThatShouldNotBeModified)
            {
                Assert.AreEqual(stringThatShouldNotBeModified, preprocessor.ReadLine());
            }
        }

        [TestMethod]
        public void EnsureIncludeChangesFileWhenUsingQuotationMarks()
        {
            headerStreamTest.Lines.AddRange(new[]
            {
                "include \"asdf.h\"",
                "include \"file/path/to/header.h\""
            });
            preprocessor.ReadLine();
            Assert.AreEqual(1, headerStreamTest.IncludeCount);
            Assert.AreEqual("asdf.h", headerStreamTest.LastInclude);

            preprocessor.ReadLine();
            Assert.AreEqual(2, headerStreamTest.IncludeCount);
            Assert.AreEqual("file/path/to/header.h", headerStreamTest.LastInclude);
        }

        [TestMethod]
        public void EnsureIncludeChangesFileWhenUsingCarets()
        {
            headerStreamTest.Lines.AddRange(new[]
            {
                "include <asdf.h>",
                "include <file/path/to/header.h>"
            });
            preprocessor.ReadLine();
            Assert.AreEqual(1, headerStreamTest.IncludeCount);
            Assert.AreEqual("asdf.h", headerStreamTest.LastInclude);

            preprocessor.ReadLine();
            Assert.AreEqual(2, headerStreamTest.IncludeCount);
            Assert.AreEqual("file/path/to/header.h", headerStreamTest.LastInclude);
        }
    }
}

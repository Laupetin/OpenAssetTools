using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Impl;
using ZoneCodeGenerator.Utils;
using ZoneCodeGeneratorTests.Parsing.Mock;

namespace ZoneCodeGeneratorTests.Parsing.C_Header.Impl
{
    [TestClass]
    public class PreprocessorTest
    {
        private IncludingParsingStreamTest headerStreamTest;
        private IHeaderParserState headerParserState;

        private Preprocessor preprocessor;

        [TestInitialize]
        public void Setup()
        {
            headerStreamTest = new IncludingParsingStreamTest("file.h");
            headerParserState = new HeaderParserState();
            preprocessor = new Preprocessor(headerStreamTest, headerParserState);
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
        public void EnsureDefinesArePlacedCorrectly()
        {
            var defineStrings = new Dictionary<string, string>()
            {
                {"#define World Hell", ""},
                {"Hello World!", "Hello Hell!"},
                {"World-wide Teamwork", "Hell-wide Teamwork"},
                {"#define great pretty bad", ""},
                {"Defines are great, right?", "Defines are pretty bad, right?"},
                {"Great world", "Great world"}, // Capitalization should matter
            };
            headerStreamTest.Lines.AddRange(defineStrings.Keys);

            foreach (var (_, expectedResult) in defineStrings)
            {
                Assert.AreEqual(expectedResult, preprocessor.ReadLine().Trim());
            }
        }

        [TestMethod]
        public void EnsureDefinesCanBeOverwritten()
        {
            var defineStrings = new Dictionary<string, string>()
            {
                {"#define World Hell", ""},
                {"#define World Mars", ""},
                {"Hello World!", "Hello Mars!"}
            };
            headerStreamTest.Lines.AddRange(defineStrings.Keys);

            foreach (var (_, expectedResult) in defineStrings)
            {
                Assert.AreEqual(expectedResult, preprocessor.ReadLine().Trim());
            }
        }

        [TestMethod]
        public void EnsureEmptyDefinesResolveToEmpty()
        {
            var defineStrings = new Dictionary<string, string>
            {
                {"#define World", ""},
                {"Hello World!", "Hello !"}
            };
            headerStreamTest.Lines.AddRange(defineStrings.Keys);

            foreach (var (_, expectedResult) in defineStrings)
            {
                Assert.AreEqual(expectedResult, preprocessor.ReadLine().Trim());
            }
        }

        [TestMethod]
        public void EnsureUndefRemovesDefines()
        {
            var defineStrings = new Dictionary<string, string>()
            {
                {"#define World Hell", ""},
                {"Hello World!", "Hello Hell!"},
                {"#undef World", ""},
                {"Hello World 2!", "Hello World 2!"},
                {"#define Hell Jupiter", ""},
                {"#define Hell Mars", ""},
                {"#undef Hell", ""},
                {"Hell no", "Hell no"},
            };
            headerStreamTest.Lines.AddRange(defineStrings.Keys);

            foreach (var (_, expectedResult) in defineStrings)
            {
                Assert.AreEqual(expectedResult, preprocessor.ReadLine().Trim());
            }
        }

        [TestMethod]
        public void EnsurePackIsParsedCorrectly()
        {
            var defaultPack = headerParserState.CurrentPack;

            var packs = new List<Tuple<string, int>>
            {
                new Tuple<string, int>("Test", defaultPack),
                new Tuple<string, int>("#pragma pack(push, 16)", 16),
                new Tuple<string, int>("Test2", 16),
                new Tuple<string, int>("#pragma pack(push, 64)", 64),
                new Tuple<string, int>("Test3", 64),
                new Tuple<string, int>("Test4", 64),
                new Tuple<string, int>("#pragma pack(pop)", 16),
                new Tuple<string, int>("Test5", 16),
                new Tuple<string, int>("#pragma pack(pop)", defaultPack),
                new Tuple<string, int>("Test6", defaultPack)
            };
            headerStreamTest.Lines.AddRange(packs.Select(tuple => tuple.Item1));
            
            foreach (var (_, expectedPack) in packs)
            {
                preprocessor.ReadLine();

                Assert.AreEqual(expectedPack, headerParserState.CurrentPack);
            }
        }

        [TestMethod]
        public void EnsureIncludeChangesFileWhenUsingQuotationMarks()
        {
            headerStreamTest.Lines.AddRange(new []
            {
                "#include \"asdf.h\"",
                "#include \"file/path/to/header.h\""
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
            headerStreamTest.Lines.AddRange(new []
            {
                "#include <asdf.h>",
                "#include <file/path/to/header.h>"
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

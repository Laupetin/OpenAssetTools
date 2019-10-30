using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.CommandFile;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.CommandFile.Tests
{
    [TestClass]
    public class TestAlignTest
    {
        private Mock<ICommandParserState> parserStateMock;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<ICommandParserState>();

            tokenOffset = 0;
            tokens = new List<string>();
            lexerMock = new Mock<ILexer>();

            lexerMock.Setup(lexer => lexer.PeekToken(It.IsAny<int>()))
                .Returns((int index) => tokens.ElementAtOrDefault(index + tokenOffset));
            lexerMock.Setup(lexer => lexer.NextToken())
                .Returns(() => tokens.ElementAtOrDefault(tokenOffset++));
            lexerMock.Setup(lexer => lexer.SkipTokens(It.IsAny<int>()))
                .Callback((int count) => tokenOffset += count);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleAlignStatement()
        {
            tokens.AddRange(new List<string>
            {
                "set", "align", "test", ":", ":", "type", "16", ";"
            });

            var test = new TestAlign();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(8, test.ConsumedTokenCount);
        }
    }
}

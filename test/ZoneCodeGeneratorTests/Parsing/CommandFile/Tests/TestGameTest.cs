using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.CommandFile;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGeneratorTests.Parsing.CommandFile.Tests
{
    [TestClass]
    public class TestGameTest
    {
        private Mock<IReadOnlyDataRepository> repositoryMock;
        private Mock<ICommandParserState> parserStateMock;

        private string game;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<ICommandParserState>();

            game = "";

            tokenOffset = 0;
            tokens = new List<string>();
            lexerMock = new Mock<ILexer>();
            repositoryMock = new Mock<IReadOnlyDataRepository>();

            parserStateMock.SetupGet(state => state.Repository)
                .Returns(() => repositoryMock.Object);

            lexerMock.Setup(lexer => lexer.PeekToken(It.IsAny<int>()))
                .Returns((int index) => tokens.ElementAtOrDefault(index + tokenOffset));
            lexerMock.Setup(lexer => lexer.NextToken())
                .Returns(() => tokens.ElementAtOrDefault(tokenOffset++));
            lexerMock.Setup(lexer => lexer.SkipTokens(It.IsAny<int>()))
                .Callback((int count) => tokenOffset += count);

            parserStateMock.SetupGet(state => state.Game)
                .Returns(() => game);
            parserStateMock.SetupSet(state => state.Game = It.IsAny<string>())
                .Callback((string s) => game = s);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleGameStatement()
        {
            tokens.AddRange(new List<string>
            {
                "game", "asdf", ";"
            });

            var test = new TestGame();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureDoesNotAcceptMultipleGameStatements()
        {
            tokens.AddRange(new List<string>
            {
                "game", "asdf", ";"
            });

            game = "alreadyAssignment";

            var test = new TestGame();

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}
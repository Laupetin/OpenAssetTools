using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.Testing.Tests
{
    [TestClass]
    public class TestCloseBlockTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private Mock<Block> topBlock;
        private Mock<INameAssignable> topBlockNameAssignable;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<IHeaderParserState>();
            topBlock = new Mock<Block>(parserStateMock.Object, BlockType.None);
            topBlockNameAssignable = topBlock.As<INameAssignable>();

            parserStateMock.SetupGet(state => state.CurrentBlock).Returns(() => topBlock.Object);

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
        public void EnsureRecognizesSimpleCurlyBrackets()
        {
            tokens.AddRange(new List<string>
            {
                "}", "struct", "test", "{"
            });

            var test = new TestCloseBlock(false);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object, true));

            // Be sure there was no name assigned
            topBlockNameAssignable.Verify(assignable => assignable.AssignName(It.IsAny<string>()), Times.Never());

            // Be sure the top block was popped
            parserStateMock.Verify(state => state.PopBlock());

            Assert.AreEqual(1, test.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureRecognizesCurlyBracketsWithSemicolon()
        {
            tokens.AddRange(new List<string>
            {
                "}", ";", "struct", "test", "{"
            });

            var test = new TestCloseBlock(true);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));

            // Be sure there was no name assigned
            topBlockNameAssignable.Verify(assignable => assignable.AssignName(It.IsAny<string>()), Times.Never());

            // Be sure the top block was popped
            parserStateMock.Verify(state => state.PopBlock());

            Assert.AreEqual(2, test.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureFailsWithoutSemicolonIfRequired()
        {
            tokens.AddRange(new List<string>
            {
                "}", "struct", "test", "{"
            });

            var test = new TestCloseBlock(true);

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            // Be sure there was no name assigned
            topBlockNameAssignable.Verify(assignable => assignable.AssignName(It.IsAny<string>()), Times.Never());
            
            parserStateMock.Verify(state => state.PopBlock(), Times.Never());
        }

        [TestMethod]
        public void EnsureAssignsName()
        {
            tokens.AddRange(new List<string>
            {
                "}", "closing_name", ";", "struct", "test", "{"
            });

            var test = new TestCloseBlock(true);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            
            topBlockNameAssignable.Verify(assignable => assignable.AssignName("closing_name"));
            topBlockNameAssignable.VerifyNoOtherCalls();

            // Be sure the top block was popped
            parserStateMock.Verify(state => state.PopBlock());

            Assert.AreEqual(3, test.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureFailsIfCannotAssignName()
        {
            tokens.AddRange(new List<string>
            {
                "}", "closing_name", ";", "struct", "test", "{"
            });

            // No INameAssignable interface implemented
            topBlock = new Mock<Block>(parserStateMock.Object, BlockType.None);

            var test = new TestCloseBlock(true);
            
            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
            
            parserStateMock.Verify(state => state.PopBlock(), Times.Never());
        }

        [TestMethod]
        public void EnsureDoesNotMatchIfNoCurlyBracket()
        {
            tokens.AddRange(new List<string>
            {
                "{", "}", "struct", "test", "{"
            });

            var test = new TestCloseBlock(false);

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            
            parserStateMock.Verify(state => state.PopBlock(), Times.Never());

            Assert.AreEqual(0, test.ConsumedTokenCount);
        }
    }
}

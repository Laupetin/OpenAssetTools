using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.C_Header.Tests
{
    [TestClass]
    public class TestNamespaceTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private Block pushedBlock;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<IHeaderParserState>();
            pushedBlock = null;

            parserStateMock.Setup(state => state.PushBlock(It.IsAny<Block>()))
                .Callback((Block block) => pushedBlock = block);

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
        public void EnsureAcceptsNamespaceDeclaration()
        {
            tokens.AddRange(new List<string>
            {
                "namespace", "some_namespace", "{"
            });

            var test = new TestNamespace();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()));
            parserStateMock.VerifyNoOtherCalls();

            if (pushedBlock is BlockNamespace namespaceBlock)
            {
                Assert.AreEqual("some_namespace", namespaceBlock.Name);
            }
            else
                Assert.Fail("Pushed block is supposed to be a namespace");
        }

        [TestMethod]
        public void EnsureAcceptsNamespacesWithSpecialCharacters()
        {
            tokens.AddRange(new List<string>
            {
                "namespace", "$omeW3irdN4m3sp4c3", "{"
            });

            var test = new TestNamespace();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()));
            parserStateMock.VerifyNoOtherCalls();

            if (pushedBlock is BlockNamespace namespaceBlock)
            {
                Assert.AreEqual("$omeW3irdN4m3sp4c3", namespaceBlock.Name);
            }
            else
                Assert.Fail("Pushed block is supposed to be a namespace");
        }

        [TestMethod]
        public void EnsureCanOnlyHaveOneNamespaceName()
        {
            tokens.AddRange(new List<string>
            {
                "namespace", "namespace_name", "another_namespace_name", "{"
            });

            var test = new TestNamespace();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);
            
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureNeedsOpeningCurlyBrackets()
        {
            tokens.AddRange(new List<string>
            {
                "namespace", "namespace_name", ""
            });

            var test = new TestNamespace();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);
            
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureRejectsNameWithDigitAsFirstCharacter()
        {
            tokens.AddRange(new List<string>
            {
                "namespace", "1ns", "{"
            });

            var test = new TestNamespace();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);
            
            parserStateMock.VerifyNoOtherCalls();
        }
    }
}

using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.C_Header.Tests
{
    [TestClass]
    public class TestForwardDeclTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private ForwardDeclaration addedForwardDeclaration;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        private Namespace currentNamespace;

        [TestInitialize]
        public void Setup()
        {
            currentNamespace = new Namespace();
            parserStateMock = new Mock<IHeaderParserState>();

            parserStateMock.SetupGet(state => state.CurrentNamespace).Returns(() => currentNamespace);
            parserStateMock.Setup(state => state.AddForwardDeclaration(It.IsAny<ForwardDeclaration>()))
                .Callback((ForwardDeclaration forwardDeclaration) => addedForwardDeclaration = forwardDeclaration);

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
        public void EnsureAcceptsForwardDeclarationForEnums()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ";"
            });

            var test = new TestForwardDecl();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddForwardDeclaration(It.IsAny<ForwardDeclaration>()), Times.Once());

            Assert.AreEqual("", addedForwardDeclaration.Namespace);
            Assert.AreEqual("test_enum", addedForwardDeclaration.Name);
            Assert.AreEqual(DataTypeType.Enum, addedForwardDeclaration.Type);
        }

        [TestMethod]
        public void EnsureAcceptsForwardDeclarationForStructs()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "test_struct", ";"
            });

            var test = new TestForwardDecl();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddForwardDeclaration(It.IsAny<ForwardDeclaration>()), Times.Once());

            Assert.AreEqual("", addedForwardDeclaration.Namespace);
            Assert.AreEqual("test_struct", addedForwardDeclaration.Name);
            Assert.AreEqual(DataTypeType.Struct, addedForwardDeclaration.Type);
        }

        [TestMethod]
        public void EnsureAcceptsForwardDeclarationForUnions()
        {
            tokens.AddRange(new List<string>
            {
                "union", "test_union", ";"
            });

            var test = new TestForwardDecl();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddForwardDeclaration(It.IsAny<ForwardDeclaration>()), Times.Once());

            Assert.AreEqual("", addedForwardDeclaration.Namespace);
            Assert.AreEqual("test_union", addedForwardDeclaration.Name);
            Assert.AreEqual(DataTypeType.Union, addedForwardDeclaration.Type);
        }

        [TestMethod]
        public void EnsureRejectsForwardDeclarationForNamespaces()
        {
            tokens.AddRange(new List<string>
            {
                "namespace", "test_union", ";"
            });

            var test = new TestForwardDecl();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);
        }
    }
}

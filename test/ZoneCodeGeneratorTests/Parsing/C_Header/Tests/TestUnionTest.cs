using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.C_Header.Tests
{
    [TestClass]
    public class TestUnionTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private List<DataType> typesOfState;
        private Namespace currentNamespace;
        private BlockUnion pushedUnion;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            currentNamespace = new Namespace();

            parserStateMock = new Mock<IHeaderParserState>();
            typesOfState = new List<DataType>();
            parserStateMock.SetupGet(state => state.CurrentNamespace)
                .Returns(() => currentNamespace);
            parserStateMock.Setup(state => state.PushBlock(It.IsAny<Block>()))
                .Callback((Block block) => pushedUnion = block as BlockUnion);
            parserStateMock.Setup(state => state.FindType(It.IsAny<string>()))
                .Returns((string query) => typesOfState.FirstOrDefault(type => type.FullName.Equals(query)));

            pushedUnion = null;
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
        public void EnsureAcceptsSimpleUnionStatement()
        {
            tokens.AddRange(new List<string>
            {
                "union", "{"
            });

            var test = new TestUnion();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(2, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedUnion);
            Assert.IsFalse(pushedUnion.IsTypedef);
        }

        [TestMethod]
        public void EnsureAcceptsNamedUnionStatement()
        {
            tokens.AddRange(new List<string>
            {
                "union", "test_union", "{"
            });

            var test = new TestUnion();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedUnion);
            Assert.IsFalse(pushedUnion.IsTypedef);
            Assert.AreEqual("test_union", pushedUnion.Name);
        }

        [TestMethod]
        public void EnsureCanBeTypedef()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "union", "test_union", "{"
            });

            var test = new TestUnion();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedUnion);
            Assert.IsTrue(pushedUnion.IsTypedef);
            Assert.AreEqual("test_union", pushedUnion.Name);
        }

        [TestMethod]
        public void EnsureAcceptsCustomSpecifiedAlignment()
        {
            tokens.AddRange(new List<string>
            {
                "union", "__declspec", "(", "align", "(", "64", ")", ")", "test_union", "{"
            });

            var test = new TestUnion();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(10, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedUnion);
            Assert.IsFalse(pushedUnion.IsTypedef);
            Assert.AreEqual("test_union", pushedUnion.Name);
            Assert.AreEqual(64, pushedUnion.CustomAlignment);
        }

        [TestMethod]
        public void EnsureCanSpecifyParent()
        {
            tokens.AddRange(new List<string>
            {
                "union", "test_union", ":", "test", ":", ":", "parent", "{"
            });

            var test = new TestUnion();

            var parent = new DataTypeUnion("test", "parent", 4);
            var parentEntry = new Variable("test_int", new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>()));
            parent.Members.Add(parentEntry);
            typesOfState.Add(parent);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(8, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedUnion);
            Assert.IsFalse(pushedUnion.IsTypedef);
            Assert.AreEqual("test_union", pushedUnion.Name);
            Assert.AreEqual(1, pushedUnion.Variables.Count);
            Assert.AreEqual(parentEntry, pushedUnion.Variables[0]);
        }

        [TestMethod]
        public void EnsureFailsWhenParentTypeCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "union", "test_union", ":", "test", ":", ":", "parent", "{"
            });

            var test = new TestUnion();

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureFailsWhenParentTypeIsNotDataTypeWithMembers()
        {
            tokens.AddRange(new List<string>
            {
                "union", "test_union", ":", "test", ":", ":", "parent", "{"
            });

            var test = new TestUnion();

            var testEnum = new DataTypeEnum("test", "parent", DataTypeBaseType.INT);
            typesOfState.Add(testEnum);

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}

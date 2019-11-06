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
    public class TestStructTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private List<DataType> typesOfState;
        private Namespace currentNamespace;
        private BlockStruct pushedStruct;

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
                .Callback((Block block) => pushedStruct = block as BlockStruct);
            parserStateMock.Setup(state => state.FindType(It.IsAny<string>()))
                .Returns((string query) => typesOfState.FirstOrDefault(type => type.FullName.Equals(query)));

            pushedStruct = null;
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
        public void EnsureAcceptsSimpleStructStatement()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "{"
            });

            var test = new TestStruct();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(2, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedStruct);
            Assert.IsFalse(pushedStruct.IsTypedef);
        }

        [TestMethod]
        public void EnsureAcceptsNamedStructStatement()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "test_struct", "{"
            });

            var test = new TestStruct();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedStruct);
            Assert.IsFalse(pushedStruct.IsTypedef);
            Assert.AreEqual("test_struct", pushedStruct.Name);
        }

        [TestMethod]
        public void EnsureCanBeTypedef()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "struct", "test_struct", "{"
            });

            var test = new TestStruct();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedStruct);
            Assert.IsTrue(pushedStruct.IsTypedef);
            Assert.AreEqual("test_struct", pushedStruct.Name);
        }

        [TestMethod]
        public void EnsureAcceptsCustomSpecifiedAlignment()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "__declspec", "(", "align", "(", "64", ")", ")", "test_struct", "{"
            });

            var test = new TestStruct();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(10, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedStruct);
            Assert.IsFalse(pushedStruct.IsTypedef);
            Assert.AreEqual("test_struct", pushedStruct.Name);
            Assert.AreEqual(64, pushedStruct.CustomAlignment);
        }

        [TestMethod]
        public void EnsureCanSpecifyParent()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "test_struct", ":", "test", ":", ":", "parent", "{"
            });

            var test = new TestStruct();

            var parent = new DataTypeStruct("test", "parent", 4);
            var parentEntry = new Variable("test_int", new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>()));
            parent.Members.Add(parentEntry);
            typesOfState.Add(parent);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(8, test.ConsumedTokenCount);

            Assert.IsNotNull(pushedStruct);
            Assert.IsFalse(pushedStruct.IsTypedef);
            Assert.AreEqual("test_struct", pushedStruct.Name);
            Assert.AreEqual(1, pushedStruct.Variables.Count);
            Assert.AreEqual(parentEntry, pushedStruct.Variables[0]);
        }

        [TestMethod]
        public void EnsureFailsWhenParentTypeCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "test_struct", ":", "test", ":", ":", "parent", "{"
            });

            var test = new TestStruct();

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureFailsWhenParentTypeIsNotDataTypeWithMembers()
        {
            tokens.AddRange(new List<string>
            {
                "struct", "test_struct", ":", "test", ":", ":", "parent", "{"
            });

            var test = new TestStruct();

            var testEnum = new DataTypeEnum("test", "parent", DataTypeBaseType.INT);
            typesOfState.Add(testEnum);

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}

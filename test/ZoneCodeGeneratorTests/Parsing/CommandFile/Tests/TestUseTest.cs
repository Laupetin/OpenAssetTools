using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.CommandFile;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGeneratorTests.Parsing.CommandFile.Tests
{
    [TestClass]
    public class TestUseTest
    {
        private Mock<IReadOnlyDataRepository> repositoryMock;
        private Mock<ICommandParserState> parserStateMock;

        private DataTypeWithMembers dataTypeWithMembers;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<ICommandParserState>();

            dataTypeWithMembers = null;

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

            parserStateMock.SetupGet(state => state.DataTypeInUse)
                .Returns(() => dataTypeWithMembers);
            parserStateMock.SetupSet(state => state.DataTypeInUse = It.IsAny<DataTypeWithMembers>())
                .Callback((DataTypeWithMembers type) => dataTypeWithMembers = type);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleUseStatement()
        {
            tokens.AddRange(new List<string>
            {
                "use", "test", ":", ":", "type", ";"
            });

            var test = new TestUse();

            var assetTypeToUse = new DataTypeStruct("test", "type", 4);

            repositoryMock.Setup(repository => repository.GetDataTypeByName("test::type"))
                .Returns(() => assetTypeToUse);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(6, test.ConsumedTokenCount);

            Assert.AreEqual(assetTypeToUse, dataTypeWithMembers);
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenTypeIsNotFound()
        {
            tokens.AddRange(new List<string>
            {
                "use", "test", ":", ":", "type", ";"
            });

            var test = new TestUse();

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenTypeIsNotDataTypeWithMembers()
        {
            tokens.AddRange(new List<string>
            {
                "use", "test", ":", ":", "type", ";"
            });

            var test = new TestUse();

            var _enum = new DataTypeEnum("test", "type", DataTypeBaseType.INT);
            var enumEntry1 = new EnumMember("ENTRY_ONE", 1);
            var enumEntry2 = new EnumMember("ENTRY_TWO", 2);
            _enum.Members.Add(enumEntry1);
            _enum.Members.Add(enumEntry2);

            repositoryMock.Setup(repository => repository.GetDataTypeByName("test::type"))
                .Returns(() => _enum);

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}
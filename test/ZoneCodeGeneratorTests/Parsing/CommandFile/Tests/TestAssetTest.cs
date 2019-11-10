using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.CommandFile;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGeneratorTests.Parsing.CommandFile.Tests
{
    [TestClass]
    public class TestAssetTest
    {
        private Mock<IReadOnlyDataRepository> repositoryMock;
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
            repositoryMock = new Mock<IReadOnlyDataRepository>();

            parserStateMock.SetupGet(state => state.Repository)
                .Returns(() => repositoryMock.Object);

            lexerMock.Setup(lexer => lexer.PeekToken(It.IsAny<int>()))
                .Returns((int index) => tokens.ElementAtOrDefault(index + tokenOffset));
            lexerMock.Setup(lexer => lexer.NextToken())
                .Returns(() => tokens.ElementAtOrDefault(tokenOffset++));
            lexerMock.Setup(lexer => lexer.SkipTokens(It.IsAny<int>()))
                .Callback((int count) => tokenOffset += count);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleAssetStatement()
        {
            tokens.AddRange(new List<string>
            {
                "asset", "test", ":", ":", "type", "TEST_TYPE_NUMBER", ";"
            });

            var test = new TestAsset();
            var assetStruct = new DataTypeStruct("test", "type", 4);
            var assetInformation = new StructureInformation(assetStruct);

            var assetEnum = new DataTypeEnum("", "testEnum", DataTypeBaseType.INT);
            var assetEnumEntry = new EnumMember("TEST_TYPE_NUMBER", 1337);
            assetEnum.Members.Add(assetEnumEntry);

            repositoryMock.Setup(repository => repository.GetDataTypeByName("test::type"))
                .Returns(assetStruct);
            repositoryMock.Setup(repository => repository.GetInformationFor(assetStruct))
                .Returns(() => assetInformation);
            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[]{assetEnum});

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(7, test.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenTypeCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "asset", "test", ":", ":", "type", "TEST_TYPE_NUMBER", ";"
            });

            var test = new TestAsset();
            var assetStruct = new DataTypeStruct("test", "type", 4);
            var assetInformation = new StructureInformation(assetStruct);

            var assetEnum = new DataTypeEnum("", "testEnum", DataTypeBaseType.INT);
            var assetEnumEntry = new EnumMember("TEST_TYPE_NUMBER", 1337);
            assetEnum.Members.Add(assetEnumEntry);

            repositoryMock.Setup(repository => repository.GetInformationFor(It.IsAny<DataTypeWithMembers>()))
                .Returns(() => assetInformation);
            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[]{assetEnum});

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenNoInformationForAssetCanBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "asset", "test", ":", ":", "type", "TEST_TYPE_NUMBER", ";"
            });

            var test = new TestAsset();
            var assetStruct = new DataTypeStruct("test", "type", 4);

            var assetEnum = new DataTypeEnum("", "testEnum", DataTypeBaseType.INT);
            var assetEnumEntry = new EnumMember("TEST_TYPE_NUMBER", 1337);
            assetEnum.Members.Add(assetEnumEntry);

            repositoryMock.Setup(repository => repository.GetDataTypeByName("test::type"))
                .Returns(assetStruct);
            repositoryMock.Setup(repository => repository.GetInformationFor(assetStruct))
                .Returns(() => null);
            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetEnum });

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenEnumEntryCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "asset", "test", ":", ":", "type", "TEST_TYPE_NUMBER", ";"
            });

            var test = new TestAsset();
            var assetStruct = new DataTypeStruct("test", "type", 4);
            var assetInformation = new StructureInformation(assetStruct);

            var assetEnum = new DataTypeEnum("", "testEnum", DataTypeBaseType.INT);
            var assetEnumEntry = new EnumMember("NOT_THE_RIGHT_ENTRY", 420);
            assetEnum.Members.Add(assetEnumEntry);

            repositoryMock.Setup(repository => repository.GetDataTypeByName("test::type"))
                .Returns(assetStruct);
            repositoryMock.Setup(repository => repository.GetInformationFor(assetStruct))
                .Returns(() => assetInformation);
            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[]{assetEnum});

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenAssetIsNotADataTypeWithMembers()
        {
            tokens.AddRange(new List<string>
            {
                "asset", "test", ":", ":", "type", "TEST_TYPE_NUMBER", ";"
            });

            var test = new TestAsset();
            var asset = new DataTypeEnum("test", "type", DataTypeBaseType.INT);
            var assetStruct = new DataTypeStruct("test", "type", 4);
            var assetInformation = new StructureInformation(assetStruct);

            var assetEnum = new DataTypeEnum("", "testEnum", DataTypeBaseType.INT);
            var assetEnumEntry = new EnumMember("TEST_TYPE_NUMBER", 1337);
            assetEnum.Members.Add(assetEnumEntry);

            repositoryMock.Setup(repository => repository.GetDataTypeByName("test::type"))
                .Returns(asset);
            repositoryMock.Setup(repository => repository.GetInformationFor(It.IsAny<DataTypeWithMembers>()))
                .Returns(() => assetInformation);
            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetEnum });

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}
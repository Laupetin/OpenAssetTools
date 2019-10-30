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
    public class TestBlockTest
    {
        private Mock<IReadOnlyDataRepository> repositoryMock;
        private Mock<ICommandParserState> parserStateMock;

        private List<FastFileBlock> fastFileBlocks;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<ICommandParserState>();

            fastFileBlocks = new List<FastFileBlock>();

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

            parserStateMock.SetupGet(state => state.FastFileBlocks)
                .Returns(() => fastFileBlocks);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleTempBlockStatement()
        {
            tokens.AddRange(new List<string>
            {
                "block", "temp", "SomeThing", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry1 = new EnumMember("whatever", 1);
            var enumEntry2 = new EnumMember("SomeThing", 513);
            assetTypeEnum.Members.Add(enumEntry1);
            assetTypeEnum.Members.Add(enumEntry2);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, fastFileBlocks.Count);

            var block = fastFileBlocks[0];

            Assert.AreEqual("SomeThing", block.Name);
            Assert.AreEqual(513, block.Index);
            Assert.AreEqual(FastFileBlock.Type.Temp, block.BlockType);
            Assert.IsTrue(block.IsTemp);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleRuntimeBlockStatement()
        {
            tokens.AddRange(new List<string>
            {
                "block", "runtime", "ASSET_TYPE_RUNTIME", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry1 = new EnumMember("whatever", 1);
            var enumEntry2 = new EnumMember("ASSET_TYPE_RUNTIME", 11);
            assetTypeEnum.Members.Add(enumEntry1);
            assetTypeEnum.Members.Add(enumEntry2);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, fastFileBlocks.Count);

            var block = fastFileBlocks[0];

            Assert.AreEqual("ASSET_TYPE_RUNTIME", block.Name);
            Assert.AreEqual(11, block.Index);
            Assert.AreEqual(FastFileBlock.Type.Runtime, block.BlockType);
            Assert.IsTrue(block.IsRuntime);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleDelayBlockStatement()
        {
            tokens.AddRange(new List<string>
            {
                "block", "delay", "ASSET_TYPE_DELAY", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry1 = new EnumMember("whatever", 1);
            var enumEntry2 = new EnumMember("ASSET_TYPE_DELAY", 133);
            assetTypeEnum.Members.Add(enumEntry1);
            assetTypeEnum.Members.Add(enumEntry2);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, fastFileBlocks.Count);

            var block = fastFileBlocks[0];

            Assert.AreEqual("ASSET_TYPE_DELAY", block.Name);
            Assert.AreEqual(133, block.Index);
            Assert.AreEqual(FastFileBlock.Type.Delay, block.BlockType);
            Assert.IsTrue(block.IsDelay);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleNormalBlockStatement()
        {
            tokens.AddRange(new List<string>
            {
                "block", "normal", "BLOCK_TYPE_LEET", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntryBlaze = new EnumMember("BLOCK_TYPE_BLAZE", 420);
            var enumEntryLeet = new EnumMember("BLOCK_TYPE_LEET", 1337);
            assetTypeEnum.Members.Add(enumEntryBlaze);
            assetTypeEnum.Members.Add(enumEntryLeet);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, fastFileBlocks.Count);

            var block = fastFileBlocks[0];

            Assert.AreEqual("BLOCK_TYPE_LEET", block.Name);
            Assert.AreEqual(1337, block.Index);
            Assert.AreEqual(FastFileBlock.Type.Normal, block.BlockType);
            Assert.IsTrue(block.IsNormal);
        }

        [TestMethod]
        public void EnsureDoesNotExceptUnknownTypes()
        {
            tokens.AddRange(new List<string>
            {
                "block", "unknowntype", "ASSET_TYPE_WHATEVER", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry1 = new EnumMember("whatever", 1);
            var enumEntry2 = new EnumMember("ASSET_TYPE_WHATEVER", 2);
            assetTypeEnum.Members.Add(enumEntry1);
            assetTypeEnum.Members.Add(enumEntry2);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureDoesNotSetDefaultBlockWhenNotSpecified()
        {
            tokens.AddRange(new List<string>
            {
                "block", "normal", "ASSET_TYPE_BLOCK", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry = new EnumMember("ASSET_TYPE_BLOCK", 5);
            assetTypeEnum.Members.Add(enumEntry);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, fastFileBlocks.Count);

            var block = fastFileBlocks[0];
            Assert.IsFalse(block.IsDefault);
        }

        [TestMethod]
        public void EnsureDoesSetDefaultBlockWhenSpecified()
        {
            tokens.AddRange(new List<string>
            {
                "block", "normal", "ASSET_TYPE_BLOCK", "default", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry = new EnumMember("ASSET_TYPE_BLOCK", 5);
            assetTypeEnum.Members.Add(enumEntry);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(5, test.ConsumedTokenCount);

            Assert.AreEqual(1, fastFileBlocks.Count);

            var block = fastFileBlocks[0];
            Assert.IsTrue(block.IsDefault);
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenEnumEntryCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "block", "normal", "ASSET_TYPE_BLOCK", ";"
            });

            var test = new TestBlock();

            var assetTypeEnum = new DataTypeEnum("", "AssetTypeEnum", DataTypeBaseType.INT);
            var enumEntry1 = new EnumMember("whatever", 1);
            var enumEntry2 = new EnumMember("ASSET_TYPE_SOMETHING_ELSE", 6);
            assetTypeEnum.Members.Add(enumEntry1);
            assetTypeEnum.Members.Add(enumEntry2);

            repositoryMock.Setup(repository => repository.GetAllEnums())
                .Returns(() => new[] { assetTypeEnum });

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}
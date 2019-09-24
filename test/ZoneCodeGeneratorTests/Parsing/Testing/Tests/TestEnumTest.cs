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

namespace ZoneCodeGeneratorTests.Parsing.Testing.Tests
{
    [TestClass]
    public class TestEnumTest
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
        public void EnsureAcceptsSimpleEnumOpening()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Once());
            parserStateMock.VerifyNoOtherCalls();

            if (pushedBlock is BlockEnum blockEnum)
            {
                Assert.AreEqual("test_enum", blockEnum.Name);
                Assert.IsFalse(blockEnum.IsTypedef);
                Assert.AreEqual(DataTypeBaseType.INT, blockEnum.ParentType);
            }
            else
                Assert.Fail("Pushed block is supposed to be an enum");
        }

        [TestMethod]
        public void EnsureAcceptsEnumAsTypedef()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "enum", "test_enum", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Once());
            parserStateMock.VerifyNoOtherCalls();

            if (pushedBlock is BlockEnum blockEnum)
            {
                Assert.AreEqual("test_enum", blockEnum.Name);
                Assert.IsTrue(blockEnum.IsTypedef);
                Assert.AreEqual(DataTypeBaseType.INT, blockEnum.ParentType);
            }
            else
                Assert.Fail("Pushed block is supposed to be an enum");
        }

        [TestMethod]
        public void EnsureAcceptsEnumWithParentTypeSpecified()
        {
            parserStateMock.Setup(state => state.FindType("unsigned char")).Returns(DataTypeBaseType.UNSIGNED_CHAR);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "unsigned", "char", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(6, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Once());

            if (pushedBlock is BlockEnum blockEnum)
            {
                Assert.AreEqual("test_enum", blockEnum.Name);
                Assert.IsFalse(blockEnum.IsTypedef);
                Assert.AreEqual(DataTypeBaseType.UNSIGNED_CHAR, blockEnum.ParentType);
            }
            else
                Assert.Fail("Pushed block is supposed to be an enum");
        }

        [TestMethod]
        public void EnsureAcceptsEnumWithParentTypeAsTypedef()
        {
            var typeDeclaration = new TypeDeclaration(DataTypeBaseType.SHORT, new List<ReferenceType>());
            var typedef = new DataTypeTypedef("", "bla_blub", typeDeclaration);
            parserStateMock.Setup(state => state.FindType("bla_blub")).Returns(typedef);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "bla_blub", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(5, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Once());

            if (pushedBlock is BlockEnum blockEnum)
            {
                Assert.AreEqual("test_enum", blockEnum.Name);
                Assert.IsFalse(blockEnum.IsTypedef);
                Assert.AreEqual(DataTypeBaseType.SHORT, blockEnum.ParentType);
            }
            else
                Assert.Fail("Pushed block is supposed to be an enum");
        }

        [TestMethod]
        public void EnsureAcceptsEnumWithParentTypeAsTypedefWithNamespace()
        {
            var typeDeclaration = new TypeDeclaration(DataTypeBaseType.SHORT, new List<ReferenceType>());
            var typedef = new DataTypeTypedef("std", "bla_blub", typeDeclaration);
            parserStateMock.Setup(state => state.FindType("std::bla_blub")).Returns(typedef);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "std", ":", ":", "bla_blub", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(8, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Once());

            if (pushedBlock is BlockEnum blockEnum)
            {
                Assert.AreEqual("test_enum", blockEnum.Name);
                Assert.IsFalse(blockEnum.IsTypedef);
                Assert.AreEqual(DataTypeBaseType.SHORT, blockEnum.ParentType);
            }
            else
                Assert.Fail("Pushed block is supposed to be an enum");
        }

        [TestMethod]
        public void EnsureRejectsEnumWithInvalidParentType()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "non_existant", "{", "test"
            });

            var test = new TestEnum();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.Verify(state => state.FindType("non_existant"), Times.Once());
        }

        [TestMethod]
        public void EnsureRejectsEnumWithParentTypeAsTypedefWithArray()
        {
            var typeDeclaration = new TypeDeclaration(DataTypeBaseType.SHORT, new List<ReferenceType>
            {
                new ReferenceTypeArray(3)
            });
            var typedef = new DataTypeTypedef("", "bla_blub", typeDeclaration);
            parserStateMock.Setup(state => state.FindType("bla_blub")).Returns(typedef);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "bla_blub", "{", "test"
            });

            var test = new TestEnum();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.Verify(state => state.FindType("bla_blub"), Times.Once());
        }

        [TestMethod]
        public void EnsureRejectsEnumWithParentTypeAsTypedefWithPointerDepth()
        {
            var typeDeclaration = new TypeDeclaration(DataTypeBaseType.SHORT, new List<ReferenceType>
            {
                new ReferenceTypePointer()
            });
            var typedef = new DataTypeTypedef("", "bla_blub", typeDeclaration);
            parserStateMock.Setup(state => state.FindType("bla_blub")).Returns(typedef);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "bla_blub", "{", "test"
            });

            var test = new TestEnum();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.Verify(state => state.FindType("bla_blub"), Times.Once());
        }

        [TestMethod]
        public void EnsureRejectsEnumWithParentTypeNotBaseType()
        {
            var _struct = new DataTypeStruct("", "bla_blub", 4);
            parserStateMock.Setup(state => state.FindType("bla_blub")).Returns(_struct);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "bla_blub", "{", "test"
            });

            var test = new TestEnum();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.Verify(state => state.FindType("bla_blub"), Times.Once());
        }

        [TestMethod]
        public void EnsureRejectsEnumWithParentTypeTypedefNotBaseType()
        {
            var _struct = new DataTypeStruct("", "bla_blub", 4);
            var typeDeclaration = new TypeDeclaration(_struct, new List<ReferenceType>());
            var typedef = new DataTypeTypedef("", "bla_blub", typeDeclaration);
            parserStateMock.Setup(state => state.FindType("bla_blub")).Returns(typedef);

            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "bla_blub", "{", "test"
            });

            var test = new TestEnum();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.Verify(state => state.FindType("bla_blub"), Times.Once());
        }

        [TestMethod]
        public void EnsureRejectsWhenKeywordIsWrong()
        {
            tokens.AddRange(new List<string>
            {
                "notenum", "test_enum", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureRejectsWhenNameIsWrong()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "3name", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureRejectsWhenNoCurlyBrackets()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", "test", "test2"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureRejectsWhenDoubleColonButNoType()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureRejectsWhenDoubleColonAndTypeButNoBracket()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "test_enum", ":", "short", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Never());
            parserStateMock.VerifyNoOtherCalls();
        }

        [TestMethod]
        public void EnsureNameIsGeneratedIfNoneExisting()
        {
            tokens.AddRange(new List<string>
            {
                "enum", "{", "test"
            });

            var test = new TestEnum();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(2, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.PushBlock(It.IsAny<Block>()), Times.Once());
            parserStateMock.VerifyNoOtherCalls();

            if (pushedBlock is BlockEnum blockEnum)
            {
                Assert.IsFalse(string.IsNullOrEmpty(blockEnum.Name));
                Assert.IsFalse(blockEnum.IsTypedef);
                Assert.AreEqual(DataTypeBaseType.INT, blockEnum.ParentType);
            }
            else
                Assert.Fail("Pushed block is supposed to be an enum");
        }
    }
}

using System;
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
    public class TestEnumMemberTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private Mock<BlockEnum> enumMock;
        private EnumMember addedEnumMember;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            parserStateMock = new Mock<IHeaderParserState>();
            enumMock = new Mock<BlockEnum>(parserStateMock.Object, "testenum", DataTypeBaseType.INT, false);
            addedEnumMember = null;

            parserStateMock.SetupGet(state => state.CurrentBlock).Returns(() => enumMock.Object);
            enumMock.Setup(_enum => _enum.AddMember(It.IsAny<EnumMember>()))
                .Callback((EnumMember member) => addedEnumMember = member);

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
        public void EnsureAcceptsSimpleEnumEntry()
        {
            tokens.AddRange(new List<string>
            {
                "asdf", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(2, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("asdf", addedEnumMember.Name);
            Assert.AreEqual(0L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureAcceptsEntryWithSpecialCharacters()
        {
            tokens.AddRange(new List<string>
            {
                "$ASgF123__", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(2, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("$ASgF123__", addedEnumMember.Name);
            Assert.AreEqual(0L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureRejectsWithDigitInTheBeginning()
        {
            tokens.AddRange(new List<string>
            {
                "123var", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }

        [TestMethod]
        public void EnsureAcceptsEntryWithDecimalNumber()
        {
            tokens.AddRange(new List<string>
            {
                "leet", "=", "1337", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("leet", addedEnumMember.Name);
            Assert.AreEqual(1337L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureAcceptsEntryWithHexadecimalNumber()
        {
            tokens.AddRange(new List<string>
            {
                "smoke", "=", "0x1a4", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("smoke", addedEnumMember.Name);
            Assert.AreEqual(420L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureRejectsEntryWithoutName()
        {
            tokens.AddRange(new List<string>
            {
                ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }

        [TestMethod]
        public void EnsureRejectsEntryWithoutNameButWithNumber()
        {
            tokens.AddRange(new List<string>
            {
                "=", "0", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }

        [TestMethod]
        public void EnsurePicksNextNumberWhenNoneSpecified()
        {
            enumMock.Setup(_enum => _enum.GetNextEnumValue()).Returns(1337L);
            tokens.AddRange(new List<string>
            {
                "asdf", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(2, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("asdf", addedEnumMember.Name);
            Assert.AreEqual(1337L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureNeedsTerminator()
        {
            tokens.AddRange(new List<string>
            {
                "asdf", "nextentry"
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }

        [TestMethod]
        public void EnsureNeedsTerminatorEvenWithValueSpecified()
        {
            tokens.AddRange(new List<string>
            {
                "asdf", "=", "0", "nextentry"
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.NoMatch, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(0, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }

        [TestMethod]
        public void EnsureAcceptsLastEntryWithoutComma()
        {
            tokens.AddRange(new List<string>
            {
                "asdf", "}"
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(1, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("asdf", addedEnumMember.Name);
            Assert.AreEqual(0L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureAcceptsLastEntryWithoutCommaWithValueSpecified()
        {
            tokens.AddRange(new List<string>
            {
                "asdf", "=", "1336", "}"
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("asdf", addedEnumMember.Name);
            Assert.AreEqual(1336L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureAcceptsOtherEnumValues()
        {
            var previousMember = new EnumMember("prev", 1335);

            parserStateMock.Setup(state => state.FindEnumMember("prev")).Returns(previousMember);

            tokens.AddRange(new List<string>
            {
                "asdf", "=", "prev", ","
            });

            var test = new TestEnumMember();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Once());

            Assert.AreEqual("asdf", addedEnumMember.Name);
            Assert.AreEqual(1335L, addedEnumMember.Value);
        }

        [TestMethod]
        public void EnsureThrowsErrorIfPreviousEnumValueNotDeclared()
        {
            tokens.AddRange(new List<string>
            {
                "asdf", "=", "prev", ","
            });

            var test = new TestEnumMember();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }

        [TestMethod]
        public void EnsureThrowsErrorIfTopBlockIsNotEnum()
        {
            var blockMock = new Mock<Block>(parserStateMock.Object, BlockType.None);
            parserStateMock.SetupGet(state => state.CurrentBlock).Returns(blockMock.Object);
            tokens.AddRange(new List<string>
            {
                "asdf", "=", "0", ","
            });

            var test = new TestEnumMember();

            Assert.ThrowsException<Exception>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));

            enumMock.Verify(_enum => _enum.AddMember(It.IsAny<EnumMember>()), Times.Never());
        }
    }
}

using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;

namespace ZoneCodeGeneratorTests.Parsing.Matching.Matchers
{
    [TestClass]
    public class MatcherNumberTest
    {
        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        private MatchingContext matchingContext;

        [TestInitialize]
        public void Setup()
        {
            tokenOffset = 0;
            tokens = new List<string>();

            lexerMock = new Mock<ILexer>();

            lexerMock.Setup(lexer => lexer.PeekToken(It.IsAny<int>()))
                .Returns((int index) => tokens.ElementAtOrDefault(index + tokenOffset));
            lexerMock.Setup(lexer => lexer.NextToken())
                .Returns(() => tokens.ElementAtOrDefault(tokenOffset++));
            lexerMock.Setup(lexer => lexer.SkipTokens(It.IsAny<int>()))
                .Callback((int count) => tokenOffset += count);

            matchingContext = new MatchingContext(lexerMock.Object, new Dictionary<string, TokenMatcher>());
        }

        [TestMethod]
        public void EnsureDecimalNumbersAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "1337"
            });

            var matcher = new MatcherNumber();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureHexNumbersAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "0x1A4"
            });

            var matcher = new MatcherNumber();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureHexNumbersWithInvalidLettersAreInvalid()
        {
            tokens.AddRange(new List<string>
            {
                "0xAFZF"
            });

            var matcher = new MatcherNumber();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureWordsAreInvalid()
        {
            tokens.AddRange(new List<string>
            {
                "Hello"
            });

            var matcher = new MatcherNumber();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureSymbolsAreInvalid()
        {
            tokens.AddRange(new List<string>
            {
                "%"
            });

            var matcher = new MatcherNumber();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureJustHexPrefixIsInvalid()
        {
            tokens.AddRange(new List<string>
            {
                "0x"
            });

            var matcher = new MatcherNumber();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureNamedOutputIsDecimalNumberForDecimalInput()
        {
            tokens.AddRange(new List<string>
            {
                "420"
            });

            var matcher = new MatcherNumber().WithName("number_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("420", result["number_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureNamedOutputIsDecimalNumberForHexInput()
        {
            tokens.AddRange(new List<string>
            {
                "0x1a5"
            });

            var matcher = new MatcherNumber().WithName("number_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("421", result["number_token"].ElementAtOrDefault(0));
        }
    }
}

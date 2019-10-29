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
    public class MatcherArrayTest
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
        public void EnsureDecimalArraysAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "[", "15", "]"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureHexArraysAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "[", "0x20", "]"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureEnumValueArraysAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "[", "ENUM_VALUE", "]"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureFirstSquareBracketIsRequiredToBeRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "13", "]"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureSecondSquareBracketIsRequiredToBeRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "[", "13"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureSquareBracketsAreRequiredToBeRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "13"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureSquareBracketsNeedToBeInTheCorrectOrder()
        {
            tokens.AddRange(new List<string>
            {
                "]", "13", "["
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureNamedOutputIsDecimalNumberForDecimalInput()
        {
            tokens.AddRange(new List<string>
            {
                "[", "13", "]"
            });

            var matcher = new MatcherArray().WithName("array_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
            Assert.AreEqual("13", result.NamedMatches["array_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureNamedOutputIsDecimalNumberForHexInput()
        {
            tokens.AddRange(new List<string>
            {
                "[", "0x133", "]"
            });

            var matcher = new MatcherArray().WithName("array_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
            Assert.AreEqual("307", result.NamedMatches["array_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureNamedOutputIsStringForEnumValueInput()
        {
            tokens.AddRange(new List<string>
            {
                "[", "HELLO_WORLD", "]"
            });

            var matcher = new MatcherArray().WithName("array_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
            Assert.AreEqual("HELLO_WORLD", result.NamedMatches["array_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureMakesCorrectUseOfTokenOffset()
        {
            tokens.AddRange(new List<string>
            {
                "random", "string", "[", "5", "]", ";"
            });

            var matcher = new MatcherArray();
            var result = matcher.Test(matchingContext, 2);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureAddsTagWhenMatched()
        {
            tokens.AddRange(new List<string>
            {
                "[", "42", "]", "randomString"
            });

            var matcher = new MatcherArray().WithTag("asdf");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
            Assert.AreEqual(1, result.MatchedTags.Count);
            Assert.AreEqual("asdf", result.MatchedTags[0]);
        }

        [TestMethod]
        public void EnsureDoesNotAddTagWhenNotMatched()
        {
            tokens.AddRange(new List<string>
            {
                "nope", "[", "42", "]", "randomString"
            });

            var matcher = new MatcherArray().WithTag("asdf");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(0, result.MatchedTags.Count);
        }
    }
}

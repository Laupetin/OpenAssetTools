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
    public class MatcherNameTest
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
        public void EnsureRecognizesNormalName()
        {
            tokens.AddRange(new List<string>
            {
                "variable_name"
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureRecognizesUnderscoreAtTheBeginning()
        {
            tokens.AddRange(new List<string>
            {
                "_variable_name"
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureAcceptsDigit()
        {
            tokens.AddRange(new List<string>
            {
                "v4ri4bl3_n4m3"
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureDoesNotAcceptDigitAtTheBeginning()
        {
            tokens.AddRange(new List<string>
            {
                "5variable_name"
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureDoesNotAcceptCKeyword()
        {
            tokens.AddRange(new List<string>
            {
                "float"
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureDoesNotAcceptSymbols()
        {
            tokens.AddRange(new List<string>
            {
                "%$§"
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureDoesNotAcceptEmptyString()
        {
            tokens.AddRange(new List<string>
            {
                ""
            });

            var matcher = new MatcherName();
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureNamedOutputIsMatchedName()
        {
            tokens.AddRange(new List<string>
            {
                "variable_n4me"
            });

            var matcher = new MatcherName().WithName("name_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("variable_n4me", result.NamedMatches["name_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureMakesCorrectUseOfTokenOffset()
        {
            tokens.AddRange(new List<string>
            {
                "static", "int", "variable_n4me", "=", "5", ";"
            });

            var matcher = new MatcherName().WithName("name_token");
            var result = matcher.Test(matchingContext, 2);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("variable_n4me", result.NamedMatches["name_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureAddsTagWhenMatched()
        {
            tokens.AddRange(new List<string>
            {
                "variable_n4me"
            });

            var matcher = new MatcherName().WithTag("very_cool_matcher");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual(1, result.MatchedTags.Count);
            Assert.AreEqual("very_cool_matcher", result.MatchedTags[0]);
        }

        [TestMethod]
        public void EnsureDoesNotAddTagWhenNotMatched()
        {
            tokens.AddRange(new List<string>
            {
                "1337"
            });

            var matcher = new MatcherName().WithTag("very_cool_matcher");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(0, result.MatchedTags.Count);
        }
    }
}

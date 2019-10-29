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
    public class MatcherLiteralTest
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
        public void EnsureMatchingSameStringReturnsSuccessful()
        {
            tokens.AddRange(new List<string>
            {
                "const"
            });

            var matcher = new MatcherLiteral("const");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureMatchingDifferentStringReturnsUnsuccessful()
        {
            tokens.AddRange(new List<string>
            {
                "long"
            });

            var matcher = new MatcherLiteral("const");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureMatchingIsCaseSensitive()
        {
            tokens.AddRange(new List<string>
            {
                "CONST"
            });

            var matcher = new MatcherLiteral("const");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureNamedMatchIsLiteralItself()
        {
            tokens.AddRange(new List<string>
            {
                "const"
            });

            var matcher = new MatcherLiteral("const").WithName("test_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("const", result.NamedMatches["test_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureMatcherWithMultipleLiteralTokensMatchesWhenAllAreFound()
        {
            tokens.AddRange(new List<string>
            {
                "const", "range", "of", "turtles"
            });

            var matcher = new MatcherLiteral("const", "range");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(2, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureMatcherWithMultipleLiteralTokensDoesNotMatchWhenFirstTokenIsDifferent()
        {
            tokens.AddRange(new List<string>
            {
                "static", "range", "of", "turtles"
            });

            var matcher = new MatcherLiteral("const", "range");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureMatcherWithMultipleLiteralTokensDoesNotMatchWhenSecondTokenIsDifferent()
        {
            tokens.AddRange(new List<string>
            {
                "const", "element", "of", "turtles"
            });

            var matcher = new MatcherLiteral("const", "const");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureMakesCorrectUseOfTokenOffset()
        {
            tokens.AddRange(new List<string>
            {
                "strawberry", "cake", "is", "yummy"
            });

            var matcher = new MatcherLiteral("cake");
            var result = matcher.Test(matchingContext, 1);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureAddsTagWhenMatched()
        {
            tokens.AddRange(new List<string>
            {
                "strawberry"
            });

            var matcher = new MatcherLiteral("strawberry").WithTag("strawberryMatcher");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual(1, result.MatchedTags.Count);
            Assert.AreEqual("strawberryMatcher", result.MatchedTags[0]);
        }

        [TestMethod]
        public void EnsureDoesNotAddTagWhenNotMatched()
        {
            tokens.AddRange(new List<string>
            {
                "apple"
            });

            var matcher = new MatcherLiteral("strawberry").WithTag("strawberryMatcher");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(0, result.MatchedTags.Count);
        }
    }
}

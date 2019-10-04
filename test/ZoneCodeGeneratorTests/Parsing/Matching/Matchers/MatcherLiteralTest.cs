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
    }
}

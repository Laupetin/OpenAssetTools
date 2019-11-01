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
    public class MatcherTypenameTest
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
        public void EnsureBaseTypeNamesAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "int"
            });
            
            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("int", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureNormalTypeNamesAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "GfxWorld"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("GfxWorld", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureUnsignedTypeNamesAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "unsigned", "int"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(2, result.ConsumedTokenCount);
            Assert.AreEqual("unsigned int", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureSignedTypeNamesAreRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "signed", "int"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(2, result.ConsumedTokenCount);
            Assert.AreEqual("signed int", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureDoubleLongIsRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "long", "long"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(2, result.ConsumedTokenCount);
            Assert.AreEqual("long long", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureDoubleLongWithPrefixesIsRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "unsigned", "long", "long"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
            Assert.AreEqual("unsigned long long", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureDoubleIntIsNotRecognized()
        {
            tokens.AddRange(new List<string>
            {
                "int", "int"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("int", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureNamespacesAreAllowed()
        {
            tokens.AddRange(new List<string>
            {
                "std", ":", ":", "string"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(4, result.ConsumedTokenCount);
            Assert.AreEqual("std::string", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureAcceptsTypeNamesWithNumbersAndUnderscores()
        {
            tokens.AddRange(new List<string>
            {
                "std", ":", ":", "int32_t"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(4, result.ConsumedTokenCount);
            Assert.AreEqual("std::int32_t", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureShortNamespacesAreAllowed()
        {
            tokens.AddRange(new List<string>
            {
                "a", ":", ":", "b"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(4, result.ConsumedTokenCount);
            Assert.AreEqual("a::b", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureDoubleColonPrefixIsNotAllowed()
        {
            tokens.AddRange(new List<string>
            {
                ":", ":", "string"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.IsFalse(result.NamedMatches.ContainsKey("type_token"));
        }

        [TestMethod]
        public void EnsureSingleDoubleColonNamespacesDoNotMatch()
        {
            tokens.AddRange(new List<string>
            {
                "std", ":", "string"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual("std", result.NamedMatches["type_token"].ElementAtOrDefault(0));
        }

        [TestMethod]
        public void EnsureSymbolsAreNotAllowed()
        {
            tokens.AddRange(new List<string>
            {
                "%"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.IsFalse(result.NamedMatches.ContainsKey("type_token"));
        }

        [TestMethod]
        public void EnsureSymbolsWithUnsignedAreNotAllowed()
        {
            tokens.AddRange(new List<string>
            {
                "unsigned", "%"
            });

            var matcher = new MatcherTypename().WithName("type_token");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.IsFalse(result.NamedMatches.ContainsKey("type_token"));
        }

        [TestMethod]
        public void EnsureMakesCorrectUseOfTokenOffset()
        {
            tokens.AddRange(new List<string>
            {
                "5", "4", "3", "2", "std", ":", ":", "string", "randomString", ";"
            });

            var matcher = new MatcherTypename();
            var result = matcher.Test(matchingContext, 4);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(4, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureAddsTagWhenMatched()
        {
            tokens.AddRange(new List<string>
            {
                "std", ":", ":", "something", "test"
            });

            var matcher = new MatcherTypename().WithTag("naisu");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(4, result.ConsumedTokenCount);
            Assert.AreEqual(1, result.MatchedTags.Count);
            Assert.AreEqual("naisu", result.MatchedTags[0]);
        }

        [TestMethod]
        public void EnsureDoesNotAddTagWhenNotMatched()
        {
            tokens.AddRange(new List<string>
            {
                "1337", "asdf", ":", ":", "something", "test"
            });

            var matcher = new MatcherTypename().WithTag("naisu");
            var result = matcher.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(0, result.MatchedTags.Count);
        }
    }
}

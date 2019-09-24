using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;

namespace ZoneCodeGeneratorTests.Parsing.Matching.Matchers
{
    [TestClass]
    public class MatcherGroupOptionalTest
    {
        private Mock<ILexer> lexerMock;
        private MatchingContext matchingContext;

        [TestInitialize]
        public void Setup()
        {
            lexerMock = new Mock<ILexer>();
            matchingContext = new MatchingContext(lexerMock.Object, new Dictionary<string, TokenMatcher>());
        }

        [TestMethod]
        public void EnsureIsSuccessfulWhenMatcherIsSuccessful()
        {
            var matcherGroup = new MatcherGroupOptional(new TestMatcher(true, 5));

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(5, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureIsSuccessfulWhenMatcherIsUnsuccessful()
        {
            var matcherGroup = new MatcherGroupOptional(new TestMatcher(false, 0));

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }
    }
}

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

        [TestMethod]
        public void EnsureMakesCorrectUseOfTokenOffset()
        {
            var testMatcher = new TestMatcher(true, 1);
            var groupOptional = new MatcherGroupOptional(testMatcher);

            var result = groupOptional.Test(matchingContext, 7);

            Assert.IsTrue(result.Successful);

            Assert.AreEqual(7, testMatcher.TestTokenOffset);
        }

        [TestMethod]
        public void EnsureAddsTagWhenMatched()
        {
            var testMatcher = new TestMatcher(true, 1).WithTag("testTag");
            var groupOptional = new MatcherGroupOptional(testMatcher).WithTag("optionalTag");

            var result = groupOptional.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(2, result.MatchedTags.Count);
            Assert.AreEqual("optionalTag", result.MatchedTags[0]);
            Assert.AreEqual("testTag", result.MatchedTags[1]);
        }

        [TestMethod]
        public void EnsureDoesStillAddItsOwnTagWhenMatcherDoesNotMatch()
        {
            var testMatcher = new TestMatcher(false, 0).WithTag("testTag");
            var groupOptional = new MatcherGroupOptional(testMatcher).WithTag("optionalTag");

            var result = groupOptional.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.MatchedTags.Count);
            Assert.AreEqual("optionalTag", result.MatchedTags[0]);
        }
    }
}

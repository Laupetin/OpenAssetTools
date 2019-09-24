using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;

namespace ZoneCodeGeneratorTests.Parsing.Matching.Matchers
{
    [TestClass]
    public class MatcherGroupLoopTest
    {
        private const string LoopToken = "loop_token";

        private int testsUntilUnsuccessful;
        private int timesTested;
        private Mock<ILexer> lexerMock;
        private Mock<TokenMatcher> tokenMatcherMock;
        private MatchingContext matchingContext;

        [TestInitialize]
        public void Setup()
        {
            timesTested = 0;
            testsUntilUnsuccessful = 0;
            lexerMock = new Mock<ILexer>();
            tokenMatcherMock = new Mock<TokenMatcher>();

            tokenMatcherMock.Setup(matcher => matcher.Test(It.IsAny<MatchingContext>(), It.IsAny<int>()))
                .Returns(() =>
                {
                    timesTested++;

                    if(testsUntilUnsuccessful == 0)
                        return new TokenMatchingResult(false, 0);

                    testsUntilUnsuccessful--;

                    var matchResult = new TokenMatchingResult(true, 1);

                    matchResult.AddNamedMatch(LoopToken, "test");

                    return matchResult;
                });

            matchingContext = new MatchingContext(lexerMock.Object, new Dictionary<string, TokenMatcher>());
        }

        [TestMethod]
        public void EnsureZeroOneMultipleAcceptsNoSuccessfulTests()
        {
            testsUntilUnsuccessful = 0;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(1, timesTested);
        }

        [TestMethod]
        public void EnsureZeroOneMultipleAcceptsOneSuccessfulTest()
        {
            testsUntilUnsuccessful = 1;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual(2, timesTested);
        }

        [TestMethod]
        public void EnsureZeroOneMultipleAcceptsMultipleSuccessfulTest()
        {
            testsUntilUnsuccessful = 5;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(5, result.ConsumedTokenCount);
            Assert.AreEqual(6, timesTested);
        }

        [TestMethod]
        public void EnsureOneMultipleRejectsNoSuccessfulTests()
        {
            testsUntilUnsuccessful = 0;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(1, timesTested);
        }

        [TestMethod]
        public void EnsureOneMultipleAcceptsOneSuccessfulTest()
        {
            testsUntilUnsuccessful = 1;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(1, result.ConsumedTokenCount);
            Assert.AreEqual(2, timesTested);
        }

        [TestMethod]
        public void EnsureOneMultipleAcceptsMultipleSuccessfulTests()
        {
            testsUntilUnsuccessful = 3;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(3, result.ConsumedTokenCount);
            Assert.AreEqual(4, timesTested);
        }

        [TestMethod]
        public void EnsureMultipleRejectsNoSuccessfulTests()
        {
            testsUntilUnsuccessful = 0;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.Multiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(1, timesTested);
        }

        [TestMethod]
        public void EnsureMultipleRejectsOneSuccessfulTest()
        {
            testsUntilUnsuccessful = 1;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.Multiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            Assert.AreEqual(2, timesTested);
        }

        [TestMethod]
        public void EnsureMultipleAcceptsMultipleSuccessfulTests()
        {
            testsUntilUnsuccessful = 2;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.Multiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(2, result.ConsumedTokenCount);
            Assert.AreEqual(3, timesTested);
        }

        [TestMethod]
        public void EnsureAllNamedMatchesAreIncluded()
        {
            testsUntilUnsuccessful = 10;
            var matcherGroup = new MatcherGroupLoop(MatcherGroupLoop.LoopMode.Multiple, tokenMatcherMock.Object);

            var result = matcherGroup.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(10, result.ConsumedTokenCount);
            Assert.AreEqual(11, timesTested);

            var namedMatches = result[LoopToken];

            Assert.AreEqual(10, namedMatches.Count);
            
            foreach(var namedMatch in namedMatches)
                Assert.AreEqual("test", namedMatch);
        }
    }
}

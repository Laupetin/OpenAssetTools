using System;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;

namespace ZoneCodeGeneratorTests.Parsing.Matching.Matchers
{
    [TestClass]
    public class MatcherGroupAndTest
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
        public void EnsureAllTestsSuccessfulMakesGroupSuccessful()
        {
            var groupAnd = new MatcherGroupAnd
            (
                new TestMatcher(true, 1),
                new TestMatcher(true, 1),
                new TestMatcher(true, 1),
                new TestMatcher(true, 1)
            );

            var result = groupAnd.Test(matchingContext, 0);

            Assert.IsTrue(result.Successful);
            Assert.AreEqual(4, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureLastTestFailingLetsGroupFail()
        {
            var groupAnd = new MatcherGroupAnd
            (
                new TestMatcher(true, 4),
                new TestMatcher(true, 2),
                new TestMatcher(true, 1),
                new TestMatcher(false, 0)
            );

            var result = groupAnd.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureFirstTestFailingLetsGroupFail()
        {
            var groupAnd = new MatcherGroupAnd
            (
                new TestMatcher(false, 0),
                new TestMatcher(true, 1),
                new TestMatcher(true, 1),
                new TestMatcher(true, 1)
            );

            var result = groupAnd.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
        }

        [TestMethod]
        public void EnsureTestsAreCalledWithCorrectTokenOffsets()
        {
            var testMatchers = new[]
            {
                new TestMatcher(true, 4),
                new TestMatcher(true, 2),
                new TestMatcher(true, 1),
                new TestMatcher(false, 0),
                new TestMatcher(true, 1)
            };
            var iTokenMatcherArray = (TokenMatcher[])testMatchers.Clone();
            var groupAnd = new MatcherGroupAnd(iTokenMatcherArray);

            var result = groupAnd.Test(matchingContext, 0);

            Assert.IsFalse(result.Successful);
            Assert.AreEqual(0, result.ConsumedTokenCount);
            
            Assert.IsTrue(testMatchers[0].WasTested);
            Assert.AreEqual(0, testMatchers[0].TestTokenOffset);
            
            Assert.IsTrue(testMatchers[1].WasTested);
            Assert.AreEqual(4, testMatchers[1].TestTokenOffset);
            
            Assert.IsTrue(testMatchers[2].WasTested);
            Assert.AreEqual(6, testMatchers[2].TestTokenOffset);
            
            Assert.IsTrue(testMatchers[3].WasTested);
            Assert.AreEqual(7, testMatchers[3].TestTokenOffset);
            
            Assert.IsFalse(testMatchers[4].WasTested);
        }

        [TestMethod]
        public void EnsureGroupWithNoTestsThrowsException()
        {
            MatcherGroupAnd matcherGroup = null;

            Assert.ThrowsException<ArgumentException>(
                () => matcherGroup = new MatcherGroupAnd()
                );

            Assert.IsNull(matcherGroup);
        }
    }
}

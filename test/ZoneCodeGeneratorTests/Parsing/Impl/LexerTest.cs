using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ZoneCodeGenerator.Parsing.Impl;
using ZoneCodeGeneratorTests.Parsing.Mock;

namespace ZoneCodeGeneratorTests.Parsing.Impl
{
    [TestClass]
    public class LexerTest
    {
        private IncludingParsingStreamTest headerStream;
        private Lexer lexer;

        [TestInitialize]
        public void Setup()
        {
            headerStream = new IncludingParsingStreamTest("file.h");
            lexer = new Lexer(headerStream);
        }

        private void AssertTokenOutput(params string[] expectedTokens)
        {
            foreach (var token in expectedTokens)
                Assert.AreEqual(token, lexer.NextToken());
        }

        [TestMethod]
        public void EnsureCanSplitWords()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });
            
            AssertTokenOutput("Some", "random", "tokens", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsureCanSplitSpecialCharactersFromWords()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "word1;word2; word3 ; word4 ;word5",
                "word1:word2: word3 : word4 :word5",
                "word1{word2{ word3 { word4 {word5",
                "word1}word2} word3 } word4 }word5",
                "word1,word2, word3 , word4 ,word5",
                "word1=word2= word3 = word4 =word5",
                "word1(word2( word3 ( word4 (word5",
                "word1)word2) word3 ) word4 )word5",
                "word1*word2* word3 * word4 *word5",
                "word1[word2[ word3 [ word4 [word5",
                "word1]word2] word3 ] word4 ]word5",
                "word1<word2< word3 < word4 <word5",
                "word1>word2> word3 > word4 >word5",
                "word1#word2# word3 # word4 #word5",
                "word1&word2& word3 & word4 &word5",
                "word1%word2% word3 % word4 %word5",
                "word1\"word2\" word3 \" word4 \"word5",
                "word1?word2? word3 ? word4 ?word5",
                "word1!word2! word3 ! word4 !word5",
                "word1+word2+ word3 + word4 +word5",
                "word1-word2- word3 - word4 -word5",
                "word1/word2/ word3 / word4 /word5",
                "word1\\word2\\ word3 \\ word4 \\word5",
            });
            
            AssertTokenOutput("word1", ";", "word2", ";", "word3", ";", "word4", ";", "word5");
            AssertTokenOutput("word1", ":", "word2", ":", "word3", ":", "word4", ":", "word5");
            AssertTokenOutput("word1", "{", "word2", "{", "word3", "{", "word4", "{", "word5");
            AssertTokenOutput("word1", "}", "word2", "}", "word3", "}", "word4", "}", "word5");
            AssertTokenOutput("word1", ",", "word2", ",", "word3", ",", "word4", ",", "word5");
            AssertTokenOutput("word1", "=", "word2", "=", "word3", "=", "word4", "=", "word5");
            AssertTokenOutput("word1", "(", "word2", "(", "word3", "(", "word4", "(", "word5");
            AssertTokenOutput("word1", ")", "word2", ")", "word3", ")", "word4", ")", "word5");
            AssertTokenOutput("word1", "*", "word2", "*", "word3", "*", "word4", "*", "word5");
            AssertTokenOutput("word1", "[", "word2", "[", "word3", "[", "word4", "[", "word5");
            AssertTokenOutput("word1", "]", "word2", "]", "word3", "]", "word4", "]", "word5");
            AssertTokenOutput("word1", "<", "word2", "<", "word3", "<", "word4", "<", "word5");
            AssertTokenOutput("word1", ">", "word2", ">", "word3", ">", "word4", ">", "word5");
            AssertTokenOutput("word1", "#", "word2", "#", "word3", "#", "word4", "#", "word5");
            AssertTokenOutput("word1", "&", "word2", "&", "word3", "&", "word4", "&", "word5");
            AssertTokenOutput("word1", "%", "word2", "%", "word3", "%", "word4", "%", "word5");
            AssertTokenOutput("word1", "\"", "word2", "\"", "word3", "\"", "word4", "\"", "word5");
            AssertTokenOutput("word1", "?", "word2", "?", "word3", "?", "word4", "?", "word5");
            AssertTokenOutput("word1", "!", "word2", "!", "word3", "!", "word4", "!", "word5");
            AssertTokenOutput("word1", "+", "word2", "+", "word3", "+", "word4", "+", "word5");
            AssertTokenOutput("word1", "-", "word2", "-", "word3", "-", "word4", "-", "word5");
            AssertTokenOutput("word1", "/", "word2", "/", "word3", "/", "word4", "/", "word5");
            AssertTokenOutput("word1", "\\", "word2", "\\", "word3", "\\", "word4", "\\", "word5");

            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsurePeekingDoesNotConsumeTokens()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });

            Assert.AreEqual("Some", lexer.PeekToken());
            Assert.AreEqual("tokens", lexer.PeekToken(2));
            Assert.AreEqual("here", lexer.PeekToken(4));
            Assert.AreEqual("right", lexer.PeekToken(3));


            Assert.AreEqual("Some", lexer.NextToken());
            Assert.AreEqual("right", lexer.PeekToken(2));

            AssertTokenOutput("random", "tokens", "right");
            
            Assert.AreEqual("here", lexer.PeekToken());
            Assert.AreEqual("here", lexer.PeekToken());
            Assert.AreEqual("here", lexer.PeekToken());
            
            Assert.AreEqual("here", lexer.NextToken());

            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsurePeekingOutOfBoundsDoesNotCauseIssues()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });

            Assert.AreEqual("", lexer.PeekToken(1337));
            Assert.AreEqual("", lexer.PeekToken(5));
            Assert.AreEqual("", lexer.PeekToken(-2));

            AssertTokenOutput("Some", "random", "tokens", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);
            
            Assert.AreEqual("", lexer.PeekToken());
            Assert.AreEqual("", lexer.PeekToken(2));
        }

        [TestMethod]
        public void EnsureGettingNextTokenAfterStreamEndDoesNotCauseIssues()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });

            AssertTokenOutput("Some", "random", "tokens", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);
            
            Assert.AreEqual("", lexer.NextToken());
            Assert.AreEqual("", lexer.NextToken());
        }

        [TestMethod]
        public void EnsureSkippingTokensWorksCorrectly()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });

            AssertTokenOutput("Some", "random");
            Assert.IsFalse(lexer.IsEndOfStream);

            lexer.SkipTokens(2);
            
            AssertTokenOutput("here");
            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsureSkippingNoTokensWorksCorrectly()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });

            AssertTokenOutput("Some", "random");
            Assert.IsFalse(lexer.IsEndOfStream);

            lexer.SkipTokens(0);
            
            AssertTokenOutput("tokens", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsureSkippingOutOfBoundsDoesNotCauseIssues()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });

            AssertTokenOutput("Some", "random");
            Assert.IsFalse(lexer.IsEndOfStream);

            lexer.SkipTokens(-5);
            
            AssertTokenOutput("tokens", "right");

            lexer.SkipTokens(1337);
            Assert.IsTrue(lexer.IsEndOfStream);
            AssertTokenOutput("");
        }

        [TestMethod]
        public void EnsureSkippingAfterStreamEndDoesNotCauseIssues()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some random tokens",
                "right here"
            });
            
            AssertTokenOutput("Some", "random", "tokens", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);

            lexer.SkipTokens(1);
            lexer.SkipTokens(2);
            
            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsureTheFilenameOfTheTokenIsCorrect()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some",
                "random",
                "tokens",
                "right",
                "here"
            });
            
            Assert.AreEqual("file.h", lexer.CurrentFile);
            AssertTokenOutput("Some");
            Assert.AreEqual("file.h", lexer.CurrentFile);
            headerStream.Filename = "file2.h";
            AssertTokenOutput("random");
            headerStream.Filename = "file3.h";
            Assert.AreEqual("file3.h", lexer.CurrentFile);
        }

        [TestMethod]
        public void EnsureFilenameOfTokenStaysConsistent()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some",
                "random",
                "tokens",
                "right",
                "here"
            });
            
            Assert.AreEqual("file.h", lexer.CurrentFile);
            Assert.AreEqual("Some", lexer.PeekToken());

            headerStream.Filename = "file2.h";

            Assert.AreEqual("file.h", lexer.CurrentFile);
            Assert.AreEqual("Some", lexer.PeekToken());
            Assert.AreEqual("file.h", lexer.CurrentFile);
            
            Assert.AreEqual("Some", lexer.NextToken());
            
            Assert.AreEqual("file2.h", lexer.CurrentFile);

            AssertTokenOutput("random", "tokens", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);
        }

        [TestMethod]
        public void EnsureTheLineOfTheTokenIsCorrect()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some",
                "random",
                "tokens",
                "right",
                "here"
            });
            
            Assert.AreEqual(0, lexer.CurrentLine);
            AssertTokenOutput("Some");
            Assert.AreEqual(1, lexer.CurrentLine);
            AssertTokenOutput("random");
            Assert.AreEqual(2, lexer.CurrentLine);

            lexer.SkipTokens(1);
            Assert.AreEqual(3, lexer.CurrentLine);
        }

        [TestMethod]
        public void EnsureLineOfTokenStaysConsistent()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "Some",
                "random",
                "tokens",
                "right",
                "here"
            });

            Assert.AreEqual(0, lexer.CurrentLine);
            Assert.AreEqual("Some", lexer.PeekToken());
            Assert.AreEqual("random", lexer.PeekToken(1));
            
            headerStream.Line = 3;
            
            Assert.AreEqual(0, lexer.CurrentLine);
            Assert.AreEqual("Some", lexer.PeekToken());
            Assert.AreEqual("Some", lexer.NextToken());
            
            Assert.AreEqual(1, lexer.CurrentLine);
            Assert.AreEqual("random", lexer.PeekToken());
            Assert.AreEqual("random", lexer.NextToken());
            
            Assert.AreEqual(3, lexer.CurrentLine);
            Assert.AreEqual("right", lexer.PeekToken());
            Assert.AreEqual("right", lexer.NextToken());
        }

        [TestMethod]
        public void EnsureTreatsUnusedSpecialCharactersAsWordCharacters()
        {
            headerStream.Lines.AddRange(new List<string>
            {
                "$ome _random token$",
                "right here"
            });
            
            AssertTokenOutput("$ome", "_random", "token$", "right", "here");
            Assert.IsTrue(lexer.IsEndOfStream);
        }
    }
}

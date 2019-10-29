using System;

namespace ZoneCodeGenerator.Parsing.Matching.Matchers
{
    class MatcherGroupLoop : GroupMatcher
    {
        public enum LoopMode
        {
            ZeroOneMultiple,
            OneMultiple,
            Multiple
        }

        private readonly TokenMatcher matcher;
        private readonly LoopMode mode;

        public MatcherGroupLoop(LoopMode mode, TokenMatcher matcher) : base(new []{matcher})
        {
            this.mode = mode;
            this.matcher = matcher;
        }

        protected override TokenMatchingResult PerformTest(MatchingContext context, int tokenOffset)
        {
            var successfulResult = new TokenMatchingResult(true, 0);
            successfulResult.AppendTag(Tag);

            var matchedTimes = 0;

            while (true)
            {
                var matcherResult = matcher.Test(context, tokenOffset + successfulResult.ConsumedTokenCount);

                if (!matcherResult.Successful)
                    break;

                matcherResult.CopyTo(successfulResult);
                matchedTimes++;
            }

            int minAmountOfMatches;
            switch (mode)
            {
                case LoopMode.ZeroOneMultiple:
                    minAmountOfMatches = 0;
                    break;

                case LoopMode.OneMultiple:
                    minAmountOfMatches = 1;
                    break;

                case LoopMode.Multiple:
                    minAmountOfMatches = 2;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            return matchedTimes < minAmountOfMatches 
                ? new TokenMatchingResult(false, 0) 
                : successfulResult;
        }

        protected override string GetIdentifier()
        {
            return "GroupLoop";
        }
    }
}

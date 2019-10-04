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
            var result = new TokenMatchingResult(false, 0);
            result.AppendTag(Tag);

            var matchedTimes = 0;

            while (true)
            {
                var matcherResult = matcher.Test(context, tokenOffset + result.ConsumedTokenCount);

                if (!matcherResult.Successful)
                    break;

                matcherResult.CopyTo(result);
                matchedTimes++;
            }

            switch (mode)
            {
                case LoopMode.ZeroOneMultiple:
                    result.Successful = true;
                    break;

                case LoopMode.OneMultiple:
                    result.Successful = matchedTimes > 0;
                    break;

                case LoopMode.Multiple:
                    result.Successful = matchedTimes > 1;
                    break;
                default:
                    throw new ArgumentOutOfRangeException();
            }

            if (!result.Successful)
                result.ConsumedTokenCount = 0;

            return result;
        }

        protected override string GetIdentifier()
        {
            return "GroupLoop";
        }
    }
}

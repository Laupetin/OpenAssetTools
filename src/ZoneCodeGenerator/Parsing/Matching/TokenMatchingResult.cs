using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Parsing.Matching
{
    class TokenMatchingResult
    {
        public bool Successful { get; set; }
        public int ConsumedTokenCount { get; set; }

        private readonly Dictionary<string, List<string>> namedMatches;
        public IReadOnlyDictionary<string, List<string>> NamedMatches => namedMatches;

        private readonly List<string> matchedTags;
        public IReadOnlyList<string> MatchedTags => matchedTags;

        public TokenMatchingResult(bool success, int consumedTokenCount)
        {
            if(!success && consumedTokenCount > 0)
                throw new ArgumentException("Token Match cannot be unsuccessful and consume tokens");

            if(consumedTokenCount < 0)
                throw new ArgumentException("Consumed token count cannot be negative");

            Successful = success;
            ConsumedTokenCount = consumedTokenCount;

            namedMatches = new Dictionary<string, List<string>>();
            matchedTags = new List<string>();
        }

        public void AddNamedMatch(string name, string value)
        {
            if(!namedMatches.ContainsKey(name))
                namedMatches.Add(name, new List<string>());

            namedMatches[name].Add(value);
        }

        public void PrependTag(string tag)
        {
            if (!string.IsNullOrEmpty(tag))
                matchedTags.Insert(0, tag);
        }

        public void AppendTag(string tag)
        {
            if(!string.IsNullOrEmpty(tag))
                matchedTags.Add(tag);
        }

        public void CopyTo(TokenMatchingResult target)
        {
            foreach(var (matchKey, matchValue) in namedMatches)
            {
                foreach (var namedMatch in matchValue)
                {
                    target.AddNamedMatch(matchKey, namedMatch);
                }
            }

            target.matchedTags.AddRange(matchedTags);

            target.ConsumedTokenCount += ConsumedTokenCount;
        }
    }
}

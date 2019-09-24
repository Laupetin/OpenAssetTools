using System;
using System.Collections;
using System.Collections.Generic;

namespace ZoneCodeGenerator.Parsing.Matching
{
    class TokenMatchingResult : IEnumerable<KeyValuePair<string, List<string>>>
    {
        public bool Successful { get; set; }
        public int ConsumedTokenCount { get; set; }

        private readonly Dictionary<string, List<string>> namedMatches;

        public TokenMatchingResult(bool success, int consumedTokenCount)
        {
            if(!success && consumedTokenCount > 0)
                throw new ArgumentException("Token Match cannot be unsuccessful and consume tokens");

            if(consumedTokenCount < 0)
                throw new ArgumentException("Consumed token count cannot be negative");

            Successful = success;
            ConsumedTokenCount = consumedTokenCount;

            namedMatches = new Dictionary<string, List<string>>();
        }

        public void AddNamedMatch(string name, string value)
        {
            if(!namedMatches.ContainsKey(name))
                namedMatches.Add(name, new List<string>());

            namedMatches[name].Add(value);
        }

        public IEnumerator<KeyValuePair<string, List<string>>> GetEnumerator()
        {
            return namedMatches.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return namedMatches.GetEnumerator();
        }

        public List<string> this[string key] => namedMatches.ContainsKey(key) ? namedMatches[key] : new List<string>();

        public void CopyNamedMatchesTo(TokenMatchingResult target)
        {
            foreach(var namedMatchKey in namedMatches)
            {
                foreach (var namedMatch in namedMatchKey.Value)
                {
                    target.AddNamedMatch(namedMatchKey.Key, namedMatch);
                }
            }
        }
    }
}

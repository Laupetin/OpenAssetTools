using System;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestEnumMember : AbstractTokenTest<IHeaderParserState>
    {
        private const string NameToken = "name";
        private const string NumberValueToken = "value_number";
        private const string EnumMemberValueToken = "value_string";

        private static readonly TokenMatcher[] matchers = {
            new MatcherName().WithName(NameToken),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherLiteral("="), 
                new MatcherGroupOr(
                    new MatcherNumber().WithName(NumberValueToken),
                    new MatcherName().WithName(EnumMemberValueToken)
            ))),
            new MatcherGroupOr(
                new MatcherLiteral(","), 
                new MatcherLiteral("}").NonConsuming()
                )
        };

        public TestEnumMember() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            var name = GetMatcherTokens(NameToken)[0];
            long value;
            
            if (!(state.CurrentBlock is BlockEnum _enum))
                throw new Exception("Excepted enum to be top block when parsing an enum member.");

            if (HasMatcherTokens(NumberValueToken))
            {
                value = long.Parse(GetMatcherTokens(NumberValueToken)[0]);
            }
            else if(HasMatcherTokens(EnumMemberValueToken))
            {
                var stringValue = GetMatcherTokens(EnumMemberValueToken)[0];
                var memberWithFittingName = state.FindEnumMember(stringValue);

                if(memberWithFittingName == null)
                    throw new TestFailedException($"Could not find value for '{stringValue}'.");

                value = memberWithFittingName.Value;
            }
            else
            {
                value = _enum.GetNextEnumValue();
            }

            var enumMember = new EnumMember(name, value);
            _enum.AddMember(enumMember);
        }
    }
}

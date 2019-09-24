using System;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestForwardDecl : AbstractTokenTest<IHeaderParserState>
    {
        private const string EnumToken = "enum";
        private const string StructToken = "struct";
        private const string UnionToken = "union";
        private const string NameToken = "name";

        private static readonly TokenMatcher[] matchers = {
            new MatcherGroupOr(
                new MatcherLiteral("enum").WithName(EnumToken), 
                new MatcherLiteral("struct").WithName(StructToken),
                new MatcherLiteral("union").WithName(UnionToken)
                ),
            new MatcherName().WithName(NameToken),
            new MatcherLiteral(";")
        };

        public TestForwardDecl() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            var name = GetMatcherTokens(NameToken)[0];
            var _namespace = state.CurrentNamespace.ToString();

            if (HasMatcherTokens(EnumToken))
            {
                state.AddForwardDeclaration(new ForwardDeclaration(_namespace, name, DataTypeType.Enum));
            }
            else if (HasMatcherTokens(StructToken))
            {
                state.AddForwardDeclaration(new ForwardDeclaration(_namespace, name, DataTypeType.Struct));
            }
            else if (HasMatcherTokens(UnionToken))
            {
                state.AddForwardDeclaration(new ForwardDeclaration(_namespace, name, DataTypeType.Union));
            }
            else
                throw new Exception("Unknown type for forward declaration.");
        }
    }
}

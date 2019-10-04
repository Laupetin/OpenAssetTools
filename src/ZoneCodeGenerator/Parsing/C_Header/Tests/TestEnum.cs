using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestEnum : AbstractTokenTest<IHeaderParserState>
    {
        private const string TypedefToken = "typedef";
        private const string NameToken = "name";
        private const string TypenameToken = "typename";

        private static readonly TokenMatcher[] matchers = {
            new MatcherGroupOptional(new MatcherLiteral("typedef").WithName(TypedefToken)),
            new MatcherLiteral("enum"),
            new MatcherGroupOptional(new MatcherName().WithName(NameToken)),
            new MatcherGroupOptional(new MatcherGroupAnd(new MatcherLiteral(":"), new MatcherTypename().WithName(TypenameToken))),
            new MatcherLiteral("{")
        };

        public TestEnum() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            var isTypedef = HasMatcherTokens(TypedefToken);
            var name = NextMatch(NameToken) ?? RandomName.GenerateName();
            var parentType = DataTypeBaseType.INT;

            if (HasMatcherTokens(TypenameToken))
            {
                var typeName = NextMatch(TypenameToken);
                var type = state.FindType(typeName);

                if (type == null)
                    throw new TestFailedException($"Parent type '{typeName}' of enum '{name}' not found.");

                while (type is DataTypeTypedef typedef)
                {
                    if (typedef.TypeDefinition.References.Count > 0)
                        throw new TestFailedException($"Parent type of enum '{name}' cannot be a typedef that is an array or a pointer.");

                    type = typedef.TypeDefinition.Type;
                }

                parentType = type as DataTypeBaseType ?? throw new TestFailedException($"Parent type of enum '{name}' must be a base type.");
            }
                
            state.PushBlock(new BlockEnum(state, name, parentType, isTypedef));
        }
    }
}

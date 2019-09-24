using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestStruct : AbstractTokenTest<IHeaderParserState>
    {
        private const string TypedefToken = "typedef";
        private const string NameToken = "name";
        private const string AlignToken = "align";
        private const string ParentToken = "parent";

        private static readonly TokenMatcher[] matchers = {
            new MatcherGroupOptional(new MatcherLiteral("typedef").WithName(TypedefToken)),
            new MatcherGroupOptional(new MatcherLiteral("const")), 
            new MatcherLiteral("struct"),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherLiteral("__declspec"),
                new MatcherLiteral("("),
                new MatcherLiteral("align"),
                new MatcherLiteral("("),
                new MatcherNumber().WithName(AlignToken),
                new MatcherLiteral(")"),
                new MatcherLiteral(")")
                )), 
            new MatcherGroupOptional(new MatcherName().WithName(NameToken)),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherLiteral(":"),
                new MatcherTypename().WithName(ParentToken)
                )),
            new MatcherLiteral("{")
        };

        public TestStruct() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            var isTypedef = HasMatcherTokens(TypedefToken);
            var name = HasMatcherTokens(NameToken) ? GetMatcherTokens(NameToken)[0] : RandomName.GenerateName();

            var block = new BlockStruct(state, name, isTypedef);

            if (HasMatcherTokens(AlignToken))
                block.CustomAlignment = int.Parse(GetMatcherTokens(AlignToken)[0]);

            if (HasMatcherTokens(ParentToken))
            {
                var parentDataTypeName = GetMatcherTokens(ParentToken)[0];
                var parentDataType = state.FindType(parentDataTypeName);

                if(parentDataType == null)
                    throw new TestFailedException($"Could not find parent data type '{parentDataTypeName}' for struct '{name}'");
                if(!(parentDataType is DataTypeStruct parentStruct))
                    throw new TestFailedException($"Parent data type '{parentDataTypeName}' for struct '{name}' must be a struct as well.");

                block.Inherit(parentStruct);
            }

            state.PushBlock(block);
        }
    }
}

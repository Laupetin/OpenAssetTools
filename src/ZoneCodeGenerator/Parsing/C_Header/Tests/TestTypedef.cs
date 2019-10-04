using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestTypedef : AbstractTokenTest<IHeaderParserState>
    {
        private const string PointerToArrayVariant = "pointertoarray";
        private const string ArrayOfPointersVariant = "arrayofpointers";
        private const string TypeNameTokens = "typename";
        private const string PointerTokens = "pointer";
        private const string NameToken = "name";
        private const string ArrayTokens = "array";

        private static readonly TokenMatcher[] arrayOfPointersMatchers = {
            new MatcherLiteral("typedef"),
            new MatcherTypename().WithName(TypeNameTokens),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherLiteral("*").WithName(PointerTokens)),
            new MatcherName().WithName(NameToken),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray().WithName(ArrayTokens)),
            new MatcherLiteral(";").WithName(ArrayOfPointersVariant)
        };

        private static readonly TokenMatcher[] pointerToArrayMatchers = {
            new MatcherLiteral("typedef"),
            new MatcherTypename().WithName(TypeNameTokens),
            new MatcherLiteral("("), 
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, new MatcherLiteral("*").WithName(PointerTokens)),
            new MatcherName().WithName(NameToken),
            new MatcherLiteral(")"), 
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, new MatcherArray().WithName(ArrayTokens)),
            new MatcherLiteral(";").WithName(PointerToArrayVariant)
        };

        private static readonly TokenMatcher[] matchers =
        {
            new MatcherGroupOr(
                new MatcherGroupAnd(arrayOfPointersMatchers),
                new MatcherGroupAnd(pointerToArrayMatchers)
                )
        };

        public TestTypedef() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            var name = NextMatch(NameToken);
            var pointerDepth = GetMatcherTokenCount(PointerTokens);

            var typeName = NextMatch(TypeNameTokens);
            var type = state.FindType(typeName);

            if (type == null)
                throw new TestFailedException($"Could not find type '{typeName}' of typedef '{name}'.");
            
            var arraySize = new int[GetMatcherTokenCount(ArrayTokens)];
            string arrayToken;
            var index = 0;
            while((arrayToken = NextMatch(ArrayTokens)) != null)
            {
                if (!int.TryParse(arrayToken, out arraySize[index++]))
                    throw new TestFailedException($"Array size '{arrayToken}' is not numeric.");
            }

            var references = new List<ReferenceType>();

            if (HasMatcherTokens(PointerToArrayVariant))
            {
                for (var i = 0; i < pointerDepth; i++)
                    references.Add(new ReferenceTypePointer());

                foreach(var array in arraySize)
                    references.Add(new ReferenceTypeArray(array));
            }
            else
            {
                foreach(var array in arraySize)
                    references.Add(new ReferenceTypeArray(array));
            
                for (var i = 0; i < pointerDepth; i++)
                    references.Add(new ReferenceTypePointer());
            }
            
            var typeDeclaration = new TypeDeclaration(type, references);
            var typedef = new DataTypeTypedef(state.CurrentNamespace.ToString(), name, typeDeclaration);
            
            state.AddDataType(typedef);
        }
    }
}

using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestTypedef : AbstractTokenTest<IHeaderParserState>
    {
        private const string TypeNameTokens = "typename";
        private const string PointerTokens = "pointer";
        private const string NameToken = "name";
        private const string ArrayTokens = "array";
        private const string AlignValueToken = "alignValue";
        private const string TokenConst = "constToken";

        private const string TagAlign = "alignTag";
        private const string TagArrayOfPointers = "arrayOfPointersTag";
        private const string TagPointerToArray = "pointerToArrayTag";

        private static readonly TokenMatcher align = new MatcherGroupAnd(
            new MatcherLiteral("__declspec", "("),
            new MatcherLiteral("align", "("),
            new MatcherNumber().WithName(AlignValueToken),
            new MatcherLiteral(")"),
            new MatcherLiteral(")")
        ).WithTag(TagAlign);

        private static readonly TokenMatcher arrayOfPointers = new MatcherGroupAnd(
            new MatcherGroupOptional(new MatcherLiteral("const").WithName(TokenConst)),
            new MatcherTypename().WithName(TypeNameTokens),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple,
                new MatcherLiteral("*").WithName(PointerTokens)),
            new MatcherName().WithName(NameToken),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray().WithName(ArrayTokens)),
            new MatcherLiteral(";")
        ).WithTag(TagArrayOfPointers);

        private static readonly TokenMatcher pointerToArray = new MatcherGroupAnd(
            new MatcherGroupOptional(new MatcherLiteral("const").WithName(TokenConst)),
            new MatcherTypename().WithName(TypeNameTokens),
            new MatcherLiteral("("),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple,
                new MatcherLiteral("*").WithName(PointerTokens)),
            new MatcherName().WithName(NameToken),
            new MatcherLiteral(")"),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple, new MatcherArray().WithName(ArrayTokens)),
            new MatcherLiteral(";")
        ).WithTag(TagPointerToArray);

        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("typedef"),
            new MatcherGroupOptional(new MatcherWithTag(TagAlign)),
            new MatcherGroupOr(
                new MatcherWithTag(TagArrayOfPointers),
                new MatcherWithTag(TagPointerToArray)
            )
        };

        public TestTypedef() : base(matchers)
        {
            AddTaggedMatcher(align);
            AddTaggedMatcher(arrayOfPointers);
            AddTaggedMatcher(pointerToArray);
        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            // Check if a custom alignment has been defined
            int? alignmentOverride = null;
            if (PeekTag().Equals(TagAlign))
            {
                NextTag();
                alignmentOverride = int.Parse(NextMatch(AlignValueToken));
            }

            // See which variant has been used
            var isArrayOfPointersVariant = NextTag().Equals(TagArrayOfPointers);

            // Get name of typedef and the amount of pointer depth it has
            var name = NextMatch(NameToken);
            var pointerDepth = GetMatcherTokenCount(PointerTokens);

            // Find referenced type
            var typeName = NextMatch(TypeNameTokens);
            var type = state.FindType(typeName);
            if (type == null)
                throw new TestFailedException($"Could not find type '{typeName}' of typedef '{name}'.");

            // Collect all array sizes that have been specified
            var arraySize = new int[GetMatcherTokenCount(ArrayTokens)];
            string arrayToken;
            var index = 0;
            while ((arrayToken = NextMatch(ArrayTokens)) != null)
            {
                if (!int.TryParse(arrayToken, out arraySize[index++]))
                    throw new TestFailedException($"Array size '{arrayToken}' is not numeric.");
            }

            var references = new List<ReferenceType>();
            if (isArrayOfPointersVariant)
            {
                foreach (var array in arraySize)
                    references.Add(new ReferenceTypeArray(array));

                for (var i = 0; i < pointerDepth; i++)
                    references.Add(new ReferenceTypePointer());
            }
            else
            {
                for (var i = 0; i < pointerDepth; i++)
                    references.Add(new ReferenceTypePointer());

                foreach (var array in arraySize)
                    references.Add(new ReferenceTypeArray(array));
            }

            var typeDeclaration = new TypeDeclaration(type, references)
            {
                IsConst = HasMatcherTokens(TokenConst)
            };
            var typedef = new DataTypeTypedef(state.CurrentNamespace.ToString(), name, typeDeclaration)
            {

                AlignmentOverride = alignmentOverride
            };

            state.AddDataType(typedef);
        }
    }
}
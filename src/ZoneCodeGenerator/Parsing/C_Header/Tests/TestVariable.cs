using System;
using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Tests
{
    class TestVariable : AbstractTokenTest<IHeaderParserState>
    {
        private const string PointerToArrayVariant = "pointertoarray";
        private const string ArrayOfPointersVariant = "arrayofpointers";
        private const string TypeNameToken = "typename";
        private const string PointerTokens = "pointer";
        private const string NameToken = "name";
        private const string ArrayTokens = "array";
        private const string BitSizeToken = "bitsize";
        private const string TokenConst = "constToken";

        private static readonly TokenMatcher[] arrayOfPointersMatchers = {
            new MatcherGroupOptional(new MatcherLiteral("const").WithName(TokenConst)),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple,  new MatcherLiteral("*").WithName(PointerTokens)),
            new MatcherName().WithName(NameToken),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherArray().WithName(ArrayTokens)),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherLiteral(":"),
                new MatcherNumber().WithName(BitSizeToken)
            )), 
            new MatcherLiteral(";").WithName(ArrayOfPointersVariant)
        };

        private static readonly TokenMatcher[] pointerToArrayMatchers = {
            new MatcherGroupOptional(new MatcherLiteral("const").WithName(TokenConst)),
            new MatcherTypename().WithName(TypeNameToken),
            new MatcherLiteral("("), 
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple,  new MatcherLiteral("*").WithName(PointerTokens)),
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

        public TestVariable() : base(matchers)
        {

        }

        protected override void ProcessMatch(IHeaderParserState state)
        {
            var name = NextMatch(NameToken);
            var typeName = NextMatch(TypeNameToken);
            var type = state.FindType(typeName);

            if (type == null)
                throw new TestFailedException($"Type '{typeName}' not found.");

            var pointerDepth = GetMatcherTokenCount(PointerTokens);

            int? bitSize = null;
            if (HasMatcherTokens(BitSizeToken))
                bitSize = int.Parse(NextMatch(BitSizeToken));

            string arrayToken;
            var index = 0;
            var arraySize = new int[GetMatcherTokenCount(ArrayTokens)];
            while ((arrayToken = NextMatch(ArrayTokens)) != null)
            {
                if (!int.TryParse(arrayToken, out arraySize[index++]))
                    throw new TestFailedException($"Array size '{arrayToken}' is not numeric.");
            }

            if (state.CurrentBlock is IVariableHolder variableHolder)
            {
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

                var typeDeclaration = new TypeDeclaration(type, references)
                {
                    CustomBitSize = bitSize,
                    IsConst = HasMatcherTokens(TokenConst)
                };

                var variable = new Variable(name, typeDeclaration);

                variableHolder.AddVariable(variable);
            }
            else
            {
                throw new Exception("Expected current block to be a variable holder when parsing variables.");
            }
        }
    }
}

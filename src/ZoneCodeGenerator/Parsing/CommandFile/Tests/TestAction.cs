using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestAction : AbstractTokenTest<ICommandParserState>
    {
        private const string TokenTypename = "typeName";
        private const string TagTypename = "typeName";

        private const string TokenActionName = "actionName";

        private const string TokenActionArgument = "actionArgument";
        private const string TagActionArguments = "actionArguments";

        private static readonly TokenMatcher actionArguments = new MatcherGroupAnd(
            new MatcherLiteral("("),
            new MatcherGroupOptional(new MatcherGroupAnd(
                new MatcherTypename().WithName(TokenActionArgument),
                new MatcherGroupLoop(MatcherGroupLoop.LoopMode.ZeroOneMultiple, new MatcherGroupAnd(
                    new MatcherLiteral(","),
                    new MatcherTypename().WithName(TokenActionArgument)
                ))
            )),
            new MatcherLiteral(")")
        ).WithTag(TagActionArguments);

        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("set"),
            new MatcherLiteral("action"),
            new MatcherGroupOr(
                new MatcherGroupAnd(
                    new MatcherName().WithName(TokenActionName),
                    new MatcherWithTag(TagActionArguments)
                ),
                new MatcherGroupAnd(
                    new MatcherTypename().WithName(TokenTypename).WithTag(TagTypename),
                    new MatcherName().WithName(TokenActionName),
                    new MatcherWithTag(TagActionArguments)
                )
            ),
            new MatcherLiteral(";")
        };

        public TestAction() : base(matchers)
        {
            AddTaggedMatcher(actionArguments);
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            StructureInformation selectedStructure;
            if (PeekTag().Equals(TagTypename))
            {
                NextTag();
                var typeName = NextMatch(TokenTypename);
                var typeNameParts = typeName.Split(new[] {"::"}, StringSplitOptions.None);

                if (state.DataTypeInUse != null &&
                    state.GetMembersFromParts(typeNameParts, state.DataTypeInUse, out var selectedMembers))
                {
                    selectedStructure = state.DataTypeInUse;
                }
                else if (state.GetTypenameAndMembersFromParts(typeNameParts, out selectedStructure, out selectedMembers))
                {
                    // Do nothing
                }
                else
                {
                    throw new TestFailedException($"Could not find type '{typeName}'.");
                }

                if (selectedMembers.Any())
                {
                    selectedStructure = selectedMembers.Last().StructureType ??
                                        throw new TestFailedException("Member is not a data type with members.");
                }
            }
            else
            {
                selectedStructure = state.DataTypeInUse ?? throw new TestFailedException("No type found for action.");
            }

            var actionName = NextMatch(TokenActionName);
            if (string.IsNullOrEmpty(actionName))
            {
                throw new TestFailedException("Action needs a valid name!");
            }

            var parameterTypes = new List<DataType>();
            string parameter;
            while ((parameter = NextMatch(TokenActionArgument)) != null)
            {
                var parameterDataType = state.Repository.GetDataTypeByName(parameter);

                if (parameterDataType == null)
                {
                    throw new TestFailedException($"Could not find type '{parameter}'.");
                }

                parameterTypes.Add(parameterDataType);
            }

            selectedStructure.PostLoadAction = new CustomAction(actionName, parameterTypes);
        }
    }
}
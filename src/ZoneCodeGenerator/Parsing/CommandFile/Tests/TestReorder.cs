using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing.Matching;
using ZoneCodeGenerator.Parsing.Matching.Matchers;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.CommandFile.Tests
{
    class TestReorder : AbstractTokenTest<ICommandParserState>
    {
        private const string TypeNameToken = "typeName";
        private const string ReorderMemberNameToken = "member";
        private const string TokenFindFirst = "findFirst";

        private static readonly TokenMatcher[] matchers =
        {
            new MatcherLiteral("reorder"),
            new MatcherGroupOptional(new MatcherTypename().WithName(TypeNameToken)),
            new MatcherLiteral(":"),
            new MatcherGroupOptional(new MatcherLiteral(".", ".", ".").WithName(TokenFindFirst)),
            new MatcherGroupLoop(MatcherGroupLoop.LoopMode.OneMultiple,
                new MatcherName().WithName(ReorderMemberNameToken)),
            new MatcherLiteral(";")
        };

        public TestReorder() : base(matchers)
        {
        }

        protected override void ProcessMatch(ICommandParserState state)
        {
            StructureInformation typeToReorder;

            if (HasMatcherTokens(TypeNameToken))
            {
                // If there was a type specified then use it to reorder.
                var typeName = NextMatch(TypeNameToken);
                var typeNameParts = typeName.Split(new[] {"::"}, StringSplitOptions.None);

                if (!state.GetTypenameAndMembersFromParts(typeNameParts, out typeToReorder, out var memberList))
                {
                    throw new TestFailedException($"Could not parse type with name '{typeName}'.");
                }

                // If there were members specified, use the type of the member.
                if (memberList.Any())
                {
                    var lastMember = memberList.Last();

                    typeToReorder = lastMember.StructureType
                                    ?? throw new TestFailedException(
                                        $"Cannot reorder type of '{lastMember.Member.Name}'. Type is not a data type with members.");
                }
            }
            else if (state.DataTypeInUse != null)
            {
                // If there was no type specified try to use the one currently in use.
                typeToReorder = state.DataTypeInUse;
            }
            else
            {
                // No type specified and no type in use. We don't know what to reorder.
                throw new TestFailedException("A type to reorder needs to be specified.");
            }

            // Create a pool of all members that have not been sorted yet.
            var memberPool = new List<MemberInformation>(typeToReorder.OrderedMembers);

            // Create a list that will be the sorted list at the end.
            var sortedMembers = new List<MemberInformation>(memberPool.Count);

            if (HasMatcherTokens(TokenFindFirst))
            {
                var firstMemberName = NextMatch(ReorderMemberNameToken);
                var firstMember =
                    memberPool.FirstOrDefault(information => information.Member.Name.Equals(firstMemberName));

                if (firstMember == null)
                {
                    throw new TestFailedException(
                        $"Cannot find member with name '{firstMemberName}' in type '{typeToReorder.Type.FullName}'.");
                }

                var firstMemberIndex = memberPool.IndexOf(firstMember);
                sortedMembers.AddRange(memberPool.GetRange(0, firstMemberIndex + 1));
                memberPool.RemoveRange(0, firstMemberIndex + 1);
            }

            string nextMemberName;
            while ((nextMemberName = NextMatch(ReorderMemberNameToken)) != null)
            {
                var nextMember =
                    memberPool.FirstOrDefault(information => information.Member.Name.Equals(nextMemberName));

                if (nextMember == null)
                {
                    throw new TestFailedException(
                        $"Cannot find member with name '{nextMemberName}' in type '{typeToReorder.Type.FullName}'.");
                }

                sortedMembers.Add(nextMember);
                memberPool.Remove(nextMember);
            }

            // Insert members that have not been mentioned at the end.
            sortedMembers.AddRange(memberPool);

            // Apply new members to the StructureInformation
            typeToReorder.OrderedMembers = sortedMembers;
        }
    }
}
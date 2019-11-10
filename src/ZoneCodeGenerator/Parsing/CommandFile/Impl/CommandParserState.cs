using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Interface;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.Impl
{
    class CommandParserState : ICommandParserState
    {
        private static readonly ITokenTest<ICommandParserState>[] tests = {
            new TestArraySize(),
            new TestAsset(),
            new TestBlock(),
            new TestCondition(),
            new TestCount(),
            new TestGame(),
            new TestReorder(),
            new TestScriptString(),
            new TestString(),
            new TestUse()
        };

        public string Game { get; set; }
        public IReadOnlyDataRepository Repository { get; }
        public List<FastFileBlock> FastFileBlocks { get; }
        public StructureInformation DataTypeInUse { get; set; }

        public CommandParserState(IReadOnlyDataRepository repository)
        {
            Repository = repository;
            FastFileBlocks = new List<FastFileBlock>();
            DataTypeInUse = null;
        }

        public IEnumerable<ITokenTest<ICommandParserState>> GetTests()
        {
            return tests;
        }

        public bool Apply(CUISession session)
        {
            session.Game = Game;

            foreach (var block in FastFileBlocks)
            {
                session.Repository.Add(block);
            }

            return true;
        }

        private bool FindTypenameParts(string[] parts, out DataTypeWithMembers dataTypeWithMembers, out int typeNamePartCount)
        {
            typeNamePartCount = 1;
            while (typeNamePartCount <= parts.Length)
            {
                var currentTypeName = string.Join("::", parts, 0, typeNamePartCount);
                var foundDataType = Repository.GetDataTypeByName(currentTypeName);

                if (foundDataType != null)
                {
                    if (!(foundDataType is DataTypeWithMembers foundDataTypeWithMembers))
                    {
                        throw new TestFailedException($"Referenced type '{currentTypeName}' needs to be a data type with members to be used in an evaluation.");
                    }

                    dataTypeWithMembers = foundDataTypeWithMembers;
                    return true;
                }

                typeNamePartCount++;
            }

            dataTypeWithMembers = null;
            return false;
        }

        public bool GetMembersFromParts(string[] parts, StructureInformation baseType, out List<MemberInformation> members)
        {
            members = new List<MemberInformation>();
            var currentStructure = baseType;
            foreach (var part in parts)
            {
                var member = currentStructure.OrderedMembers.FirstOrDefault(information =>
                    information.Member.Name.Equals(part));

                if (member == null)
                {
                    members = null;
                    return false;
                }

                members.Add(member);
                currentStructure = member.StructureType;
            }

            return true;
        }

        public bool GetTypenameAndMembersFromParts(string[] parts, out StructureInformation typeInformation,
            out List<MemberInformation> members)
        {
            if (!FindTypenameParts(parts, out var type, out var typeNamePartCount))
            {
                typeInformation = null;
                members = null;
                return false;
            }

            typeInformation = Repository.GetInformationFor(type);
            if (typeInformation == null)
            {
                members = null;
                return false;
            }

            var memberParts = new string[parts.Length - typeNamePartCount];
            Array.Copy(parts, typeNamePartCount, memberParts, 0, memberParts.Length);

            return GetMembersFromParts(memberParts, typeInformation, out members);
        }
    }
}

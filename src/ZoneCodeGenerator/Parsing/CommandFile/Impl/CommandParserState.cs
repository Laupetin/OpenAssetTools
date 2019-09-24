using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.FastFileStructure;
using ZoneCodeGenerator.Interface;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.Impl
{
    class CommandParserState : ICommandParserState
    {
        private static readonly ITokenTest<ICommandParserState>[] tests = {
            new TestAsset(),
            new TestBlock(),
            new TestCondition(),
            new TestCount(),
            new TestGame(),
            new TestReorder(),
            new TestScriptString(),
            new TestUse()
        };

        public string Game { get; set; }
        public IReadOnlyDataRepository Repository { get; }
        public List<FastFileBlock> FastFileBlocks { get; }
        public DataTypeWithMembers DataTypeInUse { get; set; }

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
    }
}

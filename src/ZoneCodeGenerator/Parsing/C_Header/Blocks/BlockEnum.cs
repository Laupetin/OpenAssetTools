using System;
using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    class BlockEnum : Block, INameAssignable, IVariableBlock
    {
        private readonly ITokenTest<IHeaderParserState>[] tests =
        {
            new TestEnumMember(),
            new TestCloseBlock(true)
        };

        public string Name { get; }
        public string AssignedName { get; private set; }
        public string Namespace { get; private set; }
        public DataTypeBaseType ParentType { get; }
        public bool IsTypedef { get; }
        public List<EnumMember> Members { get; }

        private DataTypeEnum _enum;

        public BlockEnum(IHeaderParserState headerParserState, string name, DataTypeBaseType parentType, bool isTypedef) : base(headerParserState, BlockType.Enum)
        {
            Name = name;
            ParentType = parentType;
            IsTypedef = isTypedef;
            Members = new List<EnumMember>();
        }

        public override IEnumerable<ITokenTest<IHeaderParserState>> GetAvailableTests()
        {
            return tests;
        }

        public override void OnOpen()
        {
            Namespace = State.CurrentNamespace.ToString();

            State.CurrentNamespace.Push(Name);
        }

        public override void OnClose()
        {
            var poppedNamespaceName = State.CurrentNamespace.Pop();

            if (!Name.Equals(poppedNamespaceName))
                throw new Exception($"Popped namespace '{poppedNamespaceName}' does not equal name of union block '{Name}'");

            _enum = new DataTypeEnum(Namespace, Name, ParentType);

            _enum.Members.AddRange(Members);

            State.AddDataType(_enum);

            if (IsTypedef)
            {
                var typeDeclaration = new TypeDeclaration(_enum, new List<ReferenceType>());
                var typedef = new DataTypeTypedef(Namespace, AssignedName, typeDeclaration);

                State.AddDataType(typedef);
            }
        }

        public override void OnChildBlockClose(Block childBlock)
        {
            
        }

        public void AssignName(string nameAssignment)
        {
            AssignedName = nameAssignment;
        }

        public virtual void AddMember(EnumMember enumMember)
        {
            Members.Add(enumMember);
        }

        public virtual EnumMember FindMember(string enumMemberName)
        {
            return Members.Find(member => member.Name.Equals(enumMemberName));
        }

        public virtual long GetNextEnumValue()
        {
            var nextValue = 0L;

            if (Members.Count > 0)
            {
                nextValue = Members[Members.Count - 1].Value + 1;
            }

            return nextValue;
        }

        public bool DefinesVariable()
        {
            return !IsTypedef && !string.IsNullOrEmpty(AssignedName) && _enum != null;
        }

        public Variable GetDefinedVariable()
        {
            if (!DefinesVariable())
                return null;

            var typeDeclaration = new TypeDeclaration(_enum, new List<ReferenceType>());
            return new Variable(AssignedName, typeDeclaration);
        }
    }
}

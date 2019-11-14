using System;
using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    class BlockStruct : Block, INameAssignable, IVariableHolder, IVariableBlock
    {
        private readonly ITokenTest<IHeaderParserState>[] tests =
        {
            new TestStruct(),
            new TestEnum(),
            new TestUnion(),
            new TestVariable(),
            new TestCloseBlock(true)
        };

        public string Name { get; }
        public bool IsAnonymous { get; }

        public string AssignedName { get; private set; }
        public string Namespace { get; private set; }
        public bool IsTypedef { get; }
        public int? CustomAlignment { get; set; }
        public int Pack { get; private set; }
        public List<Variable> Variables { get; }

        private DataTypeStruct _struct;
        private DataTypeTypedef typedef;

        public BlockStruct(IHeaderParserState headerParserState, string name, bool isTypedef) : base(headerParserState, BlockType.Struct)
        {
            if (!string.IsNullOrEmpty(name))
            {
                Name = name;
                IsAnonymous = false;
            }
            else
            {
                Name = RandomName.GenerateName();
                IsAnonymous = true;
            }

            IsTypedef = isTypedef;
            Variables = new List<Variable>();
            AssignedName = "";
            _struct = null;
            typedef = null;
        }

        public override IEnumerable<ITokenTest<IHeaderParserState>> GetAvailableTests()
        {
            return tests;
        }

        public override void OnOpen()
        {
            Pack = State.CurrentPack;

            Namespace = State.CurrentNamespace.ToString();

            State.CurrentNamespace.Push(Name);

            State.AddForwardDeclaration(new ForwardDeclaration(Namespace, Name, DataTypeType.Struct));
        }

        public override void OnClose()
        {
            var poppedNamespaceName = State.CurrentNamespace.Pop();

            if (!Name.Equals(poppedNamespaceName))
                throw new Exception($"Popped namespace '{poppedNamespaceName}' does not equal name of struct block '{Name}'");

            _struct = new DataTypeStruct(Namespace, Name, Pack)
            {
                AlignmentOverride = CustomAlignment,
                IsAnonymous = IsAnonymous
            };

            _struct.Members.AddRange(Variables);

            State.AddDataType(_struct);

            if (IsTypedef)
            {
                var typeDeclaration = new TypeDeclaration(_struct, new List<ReferenceType>());
                typedef = new DataTypeTypedef(Namespace, AssignedName, typeDeclaration);

                State.AddDataType(typedef);
            }
        }

        public override void OnChildBlockClose(Block childBlock)
        {
            if (childBlock is IVariableBlock variableBlock)
            {
                AddVariable(variableBlock.GetDefinedVariable());
            }
        }

        public void AssignName(string nameAssignment)
        {
            AssignedName = nameAssignment;
        }

        public void AddVariable(Variable variable)
        {
            Variables.Add(variable);
        }

        public Variable GetDefinedVariable()
        {
            var name = "";
            DataType dataType = _struct;

            if (!IsTypedef)
                name = AssignedName;
            else
                dataType = typedef;

            var typeDeclaration = new TypeDeclaration(dataType, new List<ReferenceType>());
            return new Variable(name, typeDeclaration);
        }

        public void Inherit(DataTypeStruct parent)
        {
            foreach(var variable in parent.Members)
                AddVariable(variable);
        }
    }
}

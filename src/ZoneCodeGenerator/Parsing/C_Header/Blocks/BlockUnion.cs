using System;
using System.Collections.Generic;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    class BlockUnion : Block, INameAssignable, IVariableHolder, IVariableBlock
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
        public string AssignedName { get; private set; }
        public string Namespace { get; private set; }
        public bool IsTypedef { get; }
        public int? CustomAlignment { get; set; }
        public int Pack { get; private set; }
        public List<Variable> Variables { get; }

        private DataTypeUnion union;
        private DataTypeTypedef typedef;

        public BlockUnion(IHeaderParserState headerParserState, string name, bool isTypedef) : base(headerParserState, BlockType.Union)
        {
            Name = name;
            IsTypedef = isTypedef;
            Variables = new List<Variable>();
            CustomAlignment = null;
            AssignedName = "";
            union = null;
            typedef = null;
        }

        public override IEnumerable<ITokenTest<IHeaderParserState>> GetAvailableTests()
        {
            return tests;
        }

        public override void OnOpen()
        {
            Namespace = State.CurrentNamespace.ToString();
            Pack = State.CurrentPack;

            State.CurrentNamespace.Push(Name);

            State.AddForwardDeclaration(new ForwardDeclaration(Namespace, Name, DataTypeType.Union));
        }

        public override void OnClose()
        {
            var poppedNamespaceName = State.CurrentNamespace.Pop();

            if (!Name.Equals(poppedNamespaceName))
                throw new Exception($"Popped namespace '{poppedNamespaceName}' does not equal name of union block '{Name}'");

            union = new DataTypeUnion(Namespace, Name, Pack)
            {
                AlignmentOverride = CustomAlignment
            };

            union.Members.AddRange(Variables);

            State.AddDataType(union);

            if (IsTypedef)
            {
                var typeDeclaration = new TypeDeclaration(union, new List<ReferenceType>());
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
            DataType dataType = union;

            if (!IsTypedef)
                name = AssignedName;
            else
                dataType = typedef;

            var typeDeclaration = new TypeDeclaration(dataType, new List<ReferenceType>());
            return new Variable(name, typeDeclaration);
        }

        public void Inherit(DataTypeUnion parent)
        {
            foreach(var variable in parent.Members)
                AddVariable(variable);
        }   
    }
}

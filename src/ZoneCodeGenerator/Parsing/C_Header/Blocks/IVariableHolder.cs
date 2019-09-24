using ZoneCodeGenerator.Domain;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    interface IVariableHolder
    {
        void AddVariable(Variable variable);
    }
}

using ZoneCodeGenerator.Domain;

namespace ZoneCodeGenerator.Parsing.C_Header.Blocks
{
    interface IVariableBlock
    {
        Variable GetDefinedVariable();
    }
}

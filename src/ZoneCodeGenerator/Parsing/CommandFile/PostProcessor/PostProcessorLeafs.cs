using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorLeafs : IDataPostProcessor
    {
        public static bool IsLeaf(StructureInformation structureInformation)
        {
            foreach (var member in structureInformation.OrderedMembers)
            {
                // If there is a condition to this member and it always evaluates to false: Skip this member
                if (member.Condition != null && member.Condition.IsStatic && member.Condition.EvaluateNumeric() == 0)
                    continue;

                // Any ScriptStrings or Strings need to be processed.
                if (member.IsScriptString
                    || member.IsString)
                    return false;

                // If there are any Pointer members that are not always count 0 it needs to be processed.
                var hasNoPointerMembers = member.Member.VariableType.References.OfType<ReferenceTypePointer>().All(pointer =>
                {
                    if (!pointer.Count.IsStatic)
                        return false;

                    return pointer.Count.EvaluateNumeric() == 0;
                });

                if (!hasNoPointerMembers)
                    return false;

                // If the member has an embedded type with dynamic size
                if (member.Computations.HasDynamicArraySize)
                    return false;

                if (member.StructureType != null
                    && member.StructureType != structureInformation
                    && !IsLeaf(member.StructureType))
                    return false;
            }

            return true;
        }

        public bool PostProcess(IDataRepository repository)
        {
            foreach (var dataTypeWithMembers in repository.GetAllStructs()
                .AsEnumerable<DataTypeWithMembers>()
                .Concat(repository.GetAllUnions()))
            {
                var information = repository.GetInformationFor(dataTypeWithMembers);

                information.IsLeaf = IsLeaf(information);
            }

            return true;
        }
    }
}
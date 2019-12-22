using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorMemberLeafs : IDataPostProcessor
    {
        private static bool MemberIsLeaf(MemberInformation member)
        {
            if (member.IsString || member.IsScriptString)
                return false;

            if (member.StructureType != null && !member.StructureType.IsLeaf)
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

            if (member.Computations.HasDynamicArraySize)
                return false;

            return true;
        }

        private static void ProcessStructureInformation(StructureInformation structure)
        {
            foreach (var member in structure.OrderedMembers)
            {
                member.IsLeaf = MemberIsLeaf(member);
            }
        }

        public bool PostProcess(IDataRepository repository)
        {
            foreach (var structure in repository.GetAllStructs()
                .Cast<DataTypeWithMembers>()
                .Concat(repository.GetAllUnions())
                .Select(repository.GetInformationFor))
            {
                ProcessStructureInformation(structure);
            }

            return true;
        }
    }
}
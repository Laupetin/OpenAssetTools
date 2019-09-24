using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.C_Header.PostProcessor
{
    class PostProcessorUsages : IDataPostProcessor
    {
        public bool PostProcess(IDataRepository repository)
        {
            foreach (var dataTypeWithMembers in repository.GetAllStructs()
                .AsEnumerable<DataTypeWithMembers>()
                .Concat(repository.GetAllUnions()))
            {
                var information = repository.GetInformationFor(dataTypeWithMembers);

                foreach (var memberInformation in information.OrderedMembers)
                {
                    if (memberInformation.StructureType == null) continue;

                    memberInformation.StructureType.Usages.Add(information);

                    if (IsNonEmbeddedReference(memberInformation.Member))
                        memberInformation.StructureType.NonEmbeddedReferenceExists = true;

                    if (IsPointerReference(memberInformation.Member))
                        memberInformation.StructureType.PointerReferenceExists = true;

                    if (IsArrayReference(memberInformation.Member))
                        memberInformation.StructureType.PointerReferenceExists = true;
                }
            }

            return true;
        }

        private static bool IsNonEmbeddedReference(Variable var)
        {
            return var.VariableType.References.Any();
        }

        private static bool IsPointerReference(Variable var)
        {
            return var.VariableType.References.Any() 
                   && var.VariableType.References.Last() is ReferenceTypePointer;
        }

        private static bool IsArrayReference(Variable var)
        {
            return var.VariableType.References.Any() 
                   && var.VariableType.References.Last() is ReferenceTypeArray;
        }
    }
}
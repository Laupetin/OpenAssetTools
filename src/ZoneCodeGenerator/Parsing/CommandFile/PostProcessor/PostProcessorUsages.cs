using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
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

                    if (IsSinglePointerReference(memberInformation.Member))
                        memberInformation.StructureType.SinglePointerReferenceExists = true;

                    if (IsArrayPointerReference(memberInformation.Member))
                        memberInformation.StructureType.ArrayPointerReferenceExists = true;

                    if (IsArrayReference(memberInformation.Member))
                        memberInformation.StructureType.ArrayReferenceExists = true;
                }
            }

            return true;
        }

        private static bool IsNonEmbeddedReference(Variable var)
        {
            return var.VariableType.References.Any();
        }

        private static bool IsSinglePointerReference(Variable var)
        {
            return var.VariableType.References.Any() 
                    && var.VariableType.References.Last() is ReferenceTypePointer pointerReference
                    && !pointerReference.IsArray;
        }

        private static bool IsArrayPointerReference(Variable var)
        {
            return var.VariableType.References.Any()
                   && var.VariableType.References.Last() is ReferenceTypePointer pointerReference
                   && pointerReference.IsArray;
        }

        private static bool IsArrayReference(Variable var)
        {
            return var.VariableType.References.Any() 
                   && var.VariableType.References.Last() is ReferenceTypeArray;
        }
    }
}
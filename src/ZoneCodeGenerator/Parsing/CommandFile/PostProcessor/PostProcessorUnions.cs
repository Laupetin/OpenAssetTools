using System;
using System.Collections.Generic;
using System.Linq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Parsing.CommandFile.PostProcessor
{
    class PostProcessorUnions : IDataPostProcessor
    {
        private static bool ProcessUnion(StructureInformation unionInformation)
        {
            var entriesWithoutConditionCount =
                unionInformation.OrderedMembers.Count(member => member.Condition == null && !member.IsLeaf);

            if (entriesWithoutConditionCount > 1 && unionInformation.Usages.Any() && !unionInformation.IsLeaf)
            {
                Console.WriteLine($"Union '{unionInformation.Type.FullName}' has more than one entry without a condition!");
                return false;
            }
            else if (entriesWithoutConditionCount == 1)
            {
                // If there is only one entry without condition make it the last of the ordered members
                var entryWithoutCondition = unionInformation.OrderedMembers.First(information => information.Condition == null);

                unionInformation.OrderedMembers.Remove(entryWithoutCondition);
                unionInformation.OrderedMembers.Insert(unionInformation.OrderedMembers.Count, entryWithoutCondition);
            }

            return true;
        }

        public bool PostProcess(IDataRepository repository)
        {
            return repository.GetAllUnions()
                .Select(repository.GetInformationFor)
                .All(ProcessUnion);
        }
    }
}
using System;
using ZoneCodeGenerator.Utils;

namespace ZoneCodeGenerator.Domain
{
    class DataTypeStruct : DataTypeWithMembers
    {
        public DataTypeStruct(string _namespace, string name, int pack) : base(_namespace, name, pack, DataTypeType.Struct)
        {
        }

        protected override int CalculateSize()
        {
            var currentSize = 0;
            var currentBitOffset = 0;

            foreach (var member in Members)
            {
                if (member.VariableType.HasCustomBitSize)
                {
                    currentBitOffset += member.VariableType.CustomBitSize.GetValueOrDefault(0);
                }
                else
                {
                    if (currentBitOffset > 0)
                    {
                        currentBitOffset = currentBitOffset.Align(8);
                        currentSize += currentBitOffset / 8;
                        currentBitOffset = 0;
                    }

                    currentSize = currentSize.Align(Math.Min(member.Alignment, Pack));
                    currentSize += member.VariableType.Size;
                }
            }

            if (currentBitOffset > 0)
            {
                currentBitOffset = currentBitOffset.Align(8);
                currentSize += currentBitOffset / 8;
            }
            
            return currentSize.Align(Alignment);
        }

        public override string ToString()
        {
            return $"struct {FullName}";
        }
    }
}

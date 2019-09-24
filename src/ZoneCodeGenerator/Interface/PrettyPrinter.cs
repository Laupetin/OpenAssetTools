using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGenerator.Interface
{
    class PrettyPrinter
    {
        private readonly IDataRepository dataRepository;

        public PrettyPrinter(IDataRepository dataRepository)
        {
            this.dataRepository = dataRepository;
        }

        public void PrintEnums()
        {
            var allEnums = dataRepository.GetAllEnums();
            var dataTypeEnums = allEnums.ToList();

            Console.WriteLine($"{dataTypeEnums.Count} enums:");

            foreach(var _enum in dataTypeEnums)
            {
                Console.WriteLine($" Name: {_enum.FullName}");
                Console.WriteLine($" Alignment: {_enum.Alignment}");
                Console.WriteLine($" Size: {_enum.Size}");

                foreach(var member in _enum.Members)
                {
                    Console.WriteLine($"  {member.Name} = {member.Value}");
                }

                Console.WriteLine();
            }
        }

        private static void PrintVariablePointerToArray(Variable variable)
        {
            var arraySize = new List<int>();
            var pointerDepth = 0;
            var referenceOffset = 0;

            for (; referenceOffset < variable.VariableType.References.Count; referenceOffset++)
            {
                if (!(variable.VariableType.References[referenceOffset] is ReferenceTypePointer))
                    break;
                
                pointerDepth++;
            }

            for (; referenceOffset < variable.VariableType.References.Count; referenceOffset++)
            {
                if (!(variable.VariableType.References[referenceOffset] is ReferenceTypeArray array))
                    throw new Exception("Expected the rest of the references to be array references");
                
                arraySize.Add(array.ArraySize);
            }

            var memberBuilder = new StringBuilder();
            memberBuilder.Append("  ");
            memberBuilder.Append(variable.VariableType.Type.FullName);
            memberBuilder.Append('(');
            memberBuilder.Append('*', pointerDepth);
            memberBuilder.Append(variable.Name);
            memberBuilder.Append(')');

            foreach (var array in arraySize)
            {
                memberBuilder.Append('[');
                memberBuilder.Append(array);
                memberBuilder.Append(']');
            }

            Console.WriteLine(memberBuilder.ToString());
        }

        private static void PrintVariableArrayOfPointers(Variable variable)
        {
            var arraySize = new List<int>();
            var pointerDepth = 0;
            var referenceOffset = 0;

            for (; referenceOffset < variable.VariableType.References.Count; referenceOffset++)
            {
                if (!(variable.VariableType.References[referenceOffset] is ReferenceTypeArray array))
                    break;

                arraySize.Add(array.ArraySize);
            }

            for (; referenceOffset < variable.VariableType.References.Count; referenceOffset++)
            {
                if (!(variable.VariableType.References[referenceOffset] is ReferenceTypePointer))
                    throw new Exception("Expected the rest of the references to be pointer references");

                pointerDepth++;
            }

            var memberBuilder = new StringBuilder();
            memberBuilder.Append("  ");
            memberBuilder.Append(variable.VariableType.Type.FullName);
            memberBuilder.Append('*', pointerDepth);
            memberBuilder.Append(" ");
            memberBuilder.Append(variable.Name);

            foreach (var array in arraySize)
            {
                memberBuilder.Append('[');
                memberBuilder.Append(array);
                memberBuilder.Append(']');
            }

            Console.WriteLine(memberBuilder.ToString());
        }

        private static void PrintVariable(Variable variable)
        {
            if (variable.VariableType.References.Count == 0)
            {
                Console.WriteLine($"  {variable.VariableType.Type.FullName} {variable.Name}");
            }
            else
            {
                if (variable.VariableType.References[0] is ReferenceTypePointer &&
                    variable.VariableType.References.OfType<ReferenceTypeArray>().Any())
                {
                    PrintVariablePointerToArray(variable);
                }
                else
                {
                    PrintVariableArrayOfPointers(variable);
                }
            }
        }

        public void PrintStructs()
        {
            var allStructs = dataRepository.GetAllStructs();
            var dataTypeStructs = allStructs.ToList();

            Console.WriteLine($"{dataTypeStructs.Count} structs:");

            foreach(var _struct in dataTypeStructs)
            {
                Console.WriteLine($" Name: {_struct.FullName}");
                Console.WriteLine($" Alignment: {_struct.Alignment}");
                Console.WriteLine($" Size: {_struct.Size}");

                foreach(var member in _struct.Members)
                    PrintVariable(member);

                Console.WriteLine();
            }

        }

        public void PrintUnions()
        {
            var allUnions = dataRepository.GetAllUnions();
            var dataTypeUnions = allUnions.ToList();

            Console.WriteLine($"{dataTypeUnions.Count} unions:");

            foreach(var union in dataTypeUnions)
            {
                Console.WriteLine($" Name: {union.FullName}");
                Console.WriteLine($" Alignment: {union.Alignment}");
                Console.WriteLine($" Size: {union.Size}");

                foreach(var member in union.Members)
                    PrintVariable(member);

                Console.WriteLine();
            }
        }

        private static void PrintTypedefPointerToArray(DataTypeTypedef typedef)
        {
            var arraySize = new List<int>();
            var pointerDepth = 0;
            var referenceOffset = 0;

            for (; referenceOffset < typedef.TypeDefinition.References.Count; referenceOffset++)
            {
                if (!(typedef.TypeDefinition.References[referenceOffset] is ReferenceTypePointer))
                    break;
                
                pointerDepth++;
            }

            for (; referenceOffset < typedef.TypeDefinition.References.Count; referenceOffset++)
            {
                if (!(typedef.TypeDefinition.References[referenceOffset] is ReferenceTypeArray array))
                    throw new Exception("Expected the rest of the references to be array references");
                
                arraySize.Add(array.ArraySize);
            }

            var memberBuilder = new StringBuilder();
            memberBuilder.Append("  ");
            memberBuilder.Append(typedef.TypeDefinition.Type.FullName);
            memberBuilder.Append('(');
            memberBuilder.Append('*', pointerDepth);
            memberBuilder.Append(')');

            foreach (var array in arraySize)
            {
                memberBuilder.Append('[');
                memberBuilder.Append(array);
                memberBuilder.Append(']');
            }

            Console.WriteLine(memberBuilder.ToString());
        }

        private static void PrintTypedefArrayOfPointers(DataTypeTypedef typedef)
        {
            var arraySize = new List<int>();
            var pointerDepth = 0;
            var referenceOffset = 0;

            for (; referenceOffset < typedef.TypeDefinition.References.Count; referenceOffset++)
            {
                if (!(typedef.TypeDefinition.References[referenceOffset] is ReferenceTypeArray array))
                    break;

                arraySize.Add(array.ArraySize);
            }

            for (; referenceOffset < typedef.TypeDefinition.References.Count; referenceOffset++)
            {
                if (!(typedef.TypeDefinition.References[referenceOffset] is ReferenceTypePointer))
                    throw new Exception("Expected the rest of the references to be pointer references");

                pointerDepth++;
            }

            var memberBuilder = new StringBuilder();
            memberBuilder.Append("  ");
            memberBuilder.Append(typedef.TypeDefinition.Type.FullName);
            memberBuilder.Append('*', pointerDepth);

            foreach (var array in arraySize)
            {
                memberBuilder.Append('[');
                memberBuilder.Append(array);
                memberBuilder.Append(']');
            }

            Console.WriteLine(memberBuilder.ToString());
        }

        public void PrintTypedefs()
        {
            var allTypedefs = dataRepository.GetAllTypedefs();
            var dataTypeTypedefs = allTypedefs.ToList();

            Console.WriteLine($"{dataTypeTypedefs.Count} typedefs:");

            foreach (var typeDef in dataTypeTypedefs)
            {
                Console.WriteLine($" Name: {typeDef.FullName}");
                Console.WriteLine($" Alignment: {typeDef.Alignment}");
                Console.WriteLine($" Size: {typeDef.Size}");

                if (typeDef.TypeDefinition.References.Count == 0)
                {
                    Console.WriteLine($"  {typeDef.TypeDefinition.Type.FullName}");
                }
                else
                {
                    if (typeDef.TypeDefinition.References[0] is ReferenceTypePointer &&
                        typeDef.TypeDefinition.References.OfType<ReferenceTypeArray>().Any())
                    {
                        PrintTypedefPointerToArray(typeDef);
                    }
                    else
                    {
                        PrintTypedefArrayOfPointers(typeDef);
                    }
                }

                Console.WriteLine();
            }

        }

        public void PrintAll()
        {
            PrintEnums();

            Console.WriteLine();
            Console.WriteLine("==========================================================================================================");
            Console.WriteLine();

            PrintStructs();

            Console.WriteLine();
            Console.WriteLine("==========================================================================================================");
            Console.WriteLine();

            PrintUnions();

            Console.WriteLine();
            Console.WriteLine("==========================================================================================================");
            Console.WriteLine();

            PrintTypedefs();
        }
    }
}

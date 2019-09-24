using System.Collections.Generic;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGeneratorTests.Persistence
{
    [TestClass]
    public class InMemoryDataRepositoryTest
    {
        private IDataRepository dataRepository;

        private DataTypeEnum enum1;
        private DataTypeEnum enum2;

        private DataTypeStruct struct1;
        private DataTypeStruct struct2;

        private DataTypeUnion union1;
        private DataTypeUnion union2;

        private DataTypeTypedef typedef1;
        private DataTypeTypedef typedef2;

        [TestInitialize]
        public void PrepareSampleStorage()
        {
            dataRepository = new InMemoryDataRepository();

            enum1 = new DataTypeEnum("namespace1", "enum1", DataTypeBaseType.INT);
            enum2 = new DataTypeEnum("other_namespace", "enum2", DataTypeBaseType.SHORT);

            struct1 = new DataTypeStruct("", "test_struct1", 4);
            struct2 = new DataTypeStruct("space", "aBeautifulStruct", 2);
            
            union1 = new DataTypeUnion("", "u", 4);
            union2 = new DataTypeUnion("namespacesareaverynice", "averylongunionnamewhowouldevercreateanamethislong", 16);

            var typeDeclaration1 = new TypeDeclaration(DataTypeBaseType.BOOL, new List<ReferenceType>
            {
                new ReferenceTypeArray(4),
                new ReferenceTypeArray(6)
            });
            typedef1 = new DataTypeTypedef("namespace1", "mytypedef", typeDeclaration1);
            var typeDeclaration2 = new TypeDeclaration(struct1, new List<ReferenceType>
            {
                new ReferenceTypePointer()
            });
            typedef2 = new DataTypeTypedef("", "struct1_p", typeDeclaration2);

            dataRepository.Add(enum1);
            dataRepository.Add(enum2);
            dataRepository.Add(struct1);
            dataRepository.Add(struct2);
            dataRepository.Add(union1);
            dataRepository.Add(union2);
            dataRepository.Add(typedef1);
            dataRepository.Add(typedef2);
        }

        [TestMethod]
        public void EnsureEnumsCanBeSavedAndRestored()
        {
            var allEnums = new List<DataTypeEnum>(dataRepository.GetAllEnums());

            Assert.AreEqual(2, allEnums.Count);
            
            Assert.That.Contains(enum1, allEnums);
            Assert.That.Contains(enum2, allEnums);
        }

        [TestMethod]
        public void EnsureStructsCanBeSavedAndRestored()
        {
            var allEnums = new List<DataTypeStruct>(dataRepository.GetAllStructs());

            Assert.AreEqual(2, allEnums.Count);
            Assert.That.Contains(struct1, allEnums);
            Assert.That.Contains(struct2, allEnums);
        }

        [TestMethod]
        public void EnsureUnionsCanBeSavedAndRestored()
        {
            var allUnions = new List<DataTypeUnion>(dataRepository.GetAllUnions());

            Assert.AreEqual(2, allUnions.Count);
            Assert.That.Contains(union1, allUnions);
            Assert.That.Contains(union2, allUnions);
        }

        [TestMethod]
        public void EnsureTypedefsCanBeSavedAndRestored()
        {
            var allTypedefs = new List<DataTypeTypedef>(dataRepository.GetAllTypedefs());

            Assert.AreEqual(2, allTypedefs.Count);
            Assert.That.Contains(typedef1, allTypedefs);
            Assert.That.Contains(typedef2, allTypedefs);
        }

        [TestMethod]
        public void EnsureEnumsCanBeFoundByName()
        {
            var foundDataType1 = dataRepository.GetDataTypeByName("namespace1::enum1");
            Assert.AreEqual(enum1, foundDataType1);
            
            var foundDataType2 = dataRepository.GetDataTypeByName("other_namespace::enum2");
            Assert.AreEqual(enum2, foundDataType2);
        }

        [TestMethod]
        public void EnsureStructsCanBeFoundByName()
        {
            var foundDataType1 = dataRepository.GetDataTypeByName("test_struct1");
            Assert.AreEqual(struct1, foundDataType1);
            
            var foundDataType2 = dataRepository.GetDataTypeByName("space::aBeautifulStruct");
            Assert.AreEqual(struct2, foundDataType2);
        }

        [TestMethod]
        public void EnsureUnionsCanBeFoundByName()
        {
            var foundDataType1 = dataRepository.GetDataTypeByName("u");
            Assert.AreEqual(union1, foundDataType1);
            
            var foundDataType2 = dataRepository.GetDataTypeByName("namespacesareaverynice::averylongunionnamewhowouldevercreateanamethislong");
            Assert.AreEqual(union2, foundDataType2);
        }

        [TestMethod]
        public void EnsureTypedefsCanBeFoundByName()
        {
            var foundDataType1 = dataRepository.GetDataTypeByName("namespace1::mytypedef");
            Assert.AreEqual(typedef1, foundDataType1);
            
            var foundDataType2 = dataRepository.GetDataTypeByName("struct1_p");
            Assert.AreEqual(typedef2, foundDataType2);
        }
    }
}

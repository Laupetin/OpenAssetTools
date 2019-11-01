using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.C_Header.Tests
{
    [TestClass]
    public class TestTypedefTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private List<DataType> typesOfState;
        private Namespace currentNamespace;
        private DataTypeTypedef addedTypedef;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            currentNamespace = new Namespace();

            parserStateMock = new Mock<IHeaderParserState>();
            typesOfState = new List<DataType>();
            parserStateMock.SetupGet(state => state.CurrentNamespace)
                .Returns(() => currentNamespace);
            parserStateMock.Setup(state => state.AddDataType(It.IsAny<DataTypeTypedef>()))
                .Callback((DataType dataType) => addedTypedef = (DataTypeTypedef)dataType);
            parserStateMock.Setup(state => state.FindType(It.IsAny<string>()))
                .Returns((string typename) =>
                    {
                        return typesOfState.Concat(DataTypeBaseType.BASE_TYPES)
                            .FirstOrDefault(type => type.FullName.Equals(typename));
                    });
            
            addedTypedef = null;
            tokenOffset = 0;
            tokens = new List<string>();
            lexerMock = new Mock<ILexer>();

            lexerMock.Setup(lexer => lexer.PeekToken(It.IsAny<int>()))
                .Returns((int index) => tokens.ElementAtOrDefault(index + tokenOffset));
            lexerMock.Setup(lexer => lexer.NextToken())
                .Returns(() => tokens.ElementAtOrDefault(tokenOffset++));
            lexerMock.Setup(lexer => lexer.SkipTokens(It.IsAny<int>()))
                .Callback((int count) => tokenOffset += count);
        }

        [TestMethod]
        public void EnsureAcceptsSimpleTypedef()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "char", "new_typedef", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("new_typedef", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.CHAR, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);
        }

        [TestMethod] 
        public void EnsureMakesUseOfCurrentNamespace()
        {
            currentNamespace.Push("test_namespace");

            tokens.AddRange(new List<string>
            {
                "typedef", "char", "new_typedef", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("test_namespace::new_typedef", addedTypedef.FullName);
        }

        [TestMethod] 
        public void EnsureCorrectlyHandlesTypesWithMoreThanOneToken()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "const", "unsigned", "int", "new_typedef", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(6, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("new_typedef", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.UNSIGNED_INT, addedTypedef.TypeDefinition.Type);
            Assert.IsTrue(addedTypedef.TypeDefinition.IsConst);
        }

        [TestMethod] 
        public void EnsureHandlesPointers()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "int", "*", "*", "*", "asdf", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(7, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("asdf", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.INT, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);

            Assert.AreEqual(3, addedTypedef.TypeDefinition.References.Count);
            foreach (var typeDefinitionReference in addedTypedef.TypeDefinition.References)
            {
                Assert.IsInstanceOfType(typeDefinitionReference, typeof(ReferenceTypePointer));
            }
        }

        [TestMethod] 
        public void EnsureHandlesArrays()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "int", "asdf", "[", "3", "]", "[", "5", "]", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(10, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("asdf", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.INT, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);

            Assert.AreEqual(2, addedTypedef.TypeDefinition.References.Count);
            foreach (var typeDefinitionReference in addedTypedef.TypeDefinition.References)
            {
                Assert.IsInstanceOfType(typeDefinitionReference, typeof(ReferenceTypeArray));
            }

            Assert.AreEqual(3, ((ReferenceTypeArray) addedTypedef.TypeDefinition.References[0]).ArraySize);
            Assert.AreEqual(5, ((ReferenceTypeArray) addedTypedef.TypeDefinition.References[1]).ArraySize);
        }

        [TestMethod] 
        public void EnsureCorrectlyHandlesArrayOfPointers()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "int", "*", "*", "asdf", "[", "1337", "]", "[", "0x1A4", "]", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(12, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("asdf", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.INT, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);

            Assert.AreEqual(4, addedTypedef.TypeDefinition.References.Count);
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[0], typeof(ReferenceTypeArray));
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[1], typeof(ReferenceTypeArray));
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[2], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[3], typeof(ReferenceTypePointer));

            Assert.AreEqual(1337, ((ReferenceTypeArray) addedTypedef.TypeDefinition.References[0]).ArraySize);
            Assert.AreEqual(0x1A4, ((ReferenceTypeArray) addedTypedef.TypeDefinition.References[1]).ArraySize);
        }

        [TestMethod] 
        public void EnsureCorrectlyHandlesPointerToArray()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "int", "(", "*", "*", "asdf", ")", "[", "1337", "]", "[", "0x1A4", "]", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(14, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("asdf", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.INT, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);

            Assert.AreEqual(4, addedTypedef.TypeDefinition.References.Count);
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[0], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[1], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[2], typeof(ReferenceTypeArray));
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[3], typeof(ReferenceTypeArray));

            Assert.AreEqual(1337, ((ReferenceTypeArray) addedTypedef.TypeDefinition.References[2]).ArraySize);
            Assert.AreEqual(0x1A4, ((ReferenceTypeArray) addedTypedef.TypeDefinition.References[3]).ArraySize);
        }

        [TestMethod]
        public void EnsureHandlesCustomSpecifiedAlignment()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "__declspec", "(", "align", "(", "16", ")", ")", "int", "*", "my_type_align_16", ";"
            });

            var test = new TestTypedef();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(12, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("my_type_align_16", addedTypedef.FullName);
            Assert.AreEqual(DataTypeBaseType.INT, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);
            Assert.AreEqual(16, addedTypedef.AlignmentOverride);

            Assert.AreEqual(1, addedTypedef.TypeDefinition.References.Count);
            Assert.IsInstanceOfType(addedTypedef.TypeDefinition.References[0], typeof(ReferenceTypePointer));
        }

        [TestMethod]
        public void EnsureTestFailsWhenTypeCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "some_type", "invalid_typedef", ";"
            });

            var test = new TestTypedef();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureCanUseCustomTypes()
        {
            tokens.AddRange(new List<string>
            {
                "typedef", "some_type", "custom_typedef", ";"
            });

            var test = new TestTypedef();

            var someType = new DataTypeStruct("", "some_type", 4);
            typesOfState.Add(someType);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            parserStateMock.Verify(state => state.AddDataType(It.IsAny<DataTypeTypedef>()));

            Assert.AreEqual("custom_typedef", addedTypedef.FullName);
            Assert.AreEqual(someType, addedTypedef.TypeDefinition.Type);
            Assert.IsFalse(addedTypedef.TypeDefinition.IsConst);

            Assert.AreEqual(0, addedTypedef.TypeDefinition.References.Count);
        }
    }
}

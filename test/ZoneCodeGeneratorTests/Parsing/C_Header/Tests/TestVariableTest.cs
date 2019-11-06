using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.C_Header;
using ZoneCodeGenerator.Parsing.C_Header.Blocks;
using ZoneCodeGenerator.Parsing.C_Header.Tests;
using ZoneCodeGenerator.Parsing.Testing;

namespace ZoneCodeGeneratorTests.Parsing.C_Header.Tests
{
    [TestClass]
    public class TestVariableTest
    {
        private Mock<IHeaderParserState> parserStateMock;
        private List<DataType> typesOfState;
        private Namespace currentNamespace;
        private Stack<Block> blocks;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            currentNamespace = new Namespace();
            blocks = new Stack<Block>();

            parserStateMock = new Mock<IHeaderParserState>();
            typesOfState = new List<DataType>();
            parserStateMock.SetupGet(state => state.CurrentNamespace)
                .Returns(() => currentNamespace);
            parserStateMock.SetupGet(state => state.CurrentBlock)
                .Returns(() => blocks.Peek());
            parserStateMock.Setup(state => state.FindType(It.IsAny<string>()))
                .Returns((string query) => typesOfState.Concat(DataTypeBaseType.BASE_TYPES).FirstOrDefault(type => type.FullName.Equals(query)));

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
        public void EnsureAcceptsSimpleVariableStatement()
        {
            tokens.AddRange(new List<string>
            {
                "int", "test", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(3, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("test", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(0, variable.VariableType.References.Count);
            Assert.AreEqual(DataTypeBaseType.INT, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureAcceptsCustomStructures()
        {
            tokens.AddRange(new List<string>
            {
                "test_namespace", ":", ":", "customStruct", "hey_ho", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var customStruct = new DataTypeStruct("test_namespace", "customStruct", 4);
            typesOfState.Add(customStruct);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(6, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("hey_ho", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(0, variable.VariableType.References.Count);
            Assert.AreEqual(customStruct, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureFailsWhenTypeCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "test_namespace", ":", ":", "customStruct", "hey_ho", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.ThrowsException<TestFailedException>(
                () => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureCanHandlePointers()
        {
            tokens.AddRange(new List<string>
            {
                "float", "*", "santa_claus", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("santa_claus", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(1, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypePointer));

            Assert.AreEqual(DataTypeBaseType.FLOAT, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureCanHandleMultiPointers()
        {
            tokens.AddRange(new List<string>
            {
                "float", "*", "*", "*", "*", "santa_claus", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(7, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("santa_claus", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(4, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(variable.VariableType.References[1], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(variable.VariableType.References[2], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(variable.VariableType.References[3], typeof(ReferenceTypePointer));

            Assert.AreEqual(DataTypeBaseType.FLOAT, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureCanHandleArrays()
        {
            tokens.AddRange(new List<string>
            {
                "double", "pepper", "[", "9", "]", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(6, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("pepper", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(1, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypeArray));
            Assert.AreEqual(9, ((ReferenceTypeArray) variable.VariableType.References[0]).ArraySize);

            Assert.AreEqual(DataTypeBaseType.DOUBLE, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureCanHandleMultipleArrays()
        {
            tokens.AddRange(new List<string>
            {
                "long", "long", "salt", "[", "4", "]", "[", "2", "]", "[", "1", "]", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(13, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("salt", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(3, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypeArray));
            Assert.AreEqual(4, ((ReferenceTypeArray) variable.VariableType.References[0]).ArraySize);
            Assert.IsInstanceOfType(variable.VariableType.References[1], typeof(ReferenceTypeArray));
            Assert.AreEqual(2, ((ReferenceTypeArray) variable.VariableType.References[1]).ArraySize);
            Assert.IsInstanceOfType(variable.VariableType.References[2], typeof(ReferenceTypeArray));
            Assert.AreEqual(1, ((ReferenceTypeArray) variable.VariableType.References[2]).ArraySize);

            Assert.AreEqual(DataTypeBaseType.LONG_LONG, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureCanHandleArrayOfPointers()
        {
            tokens.AddRange(new List<string>
            {
                "unsigned", "long", "long", "*", "*", "what_a_var", "[", "6", "]", "[", "9", "]", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(13, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("what_a_var", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(4, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypeArray));
            Assert.AreEqual(6, ((ReferenceTypeArray) variable.VariableType.References[0]).ArraySize);
            Assert.IsInstanceOfType(variable.VariableType.References[1], typeof(ReferenceTypeArray));
            Assert.AreEqual(9, ((ReferenceTypeArray) variable.VariableType.References[1]).ArraySize);
            Assert.IsInstanceOfType(variable.VariableType.References[2], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(variable.VariableType.References[3], typeof(ReferenceTypePointer));

            Assert.AreEqual(DataTypeBaseType.UNSIGNED_LONG_LONG, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureCanHandlePointerToArray()
        {
            tokens.AddRange(new List<string>
            {
                "unsigned", "char", "(", "*", "*", "send_halp", ")", "[", "1", "]", "[", "3", "]", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(14, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("send_halp", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(4, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(variable.VariableType.References[1], typeof(ReferenceTypePointer));
            Assert.IsInstanceOfType(variable.VariableType.References[2], typeof(ReferenceTypeArray));
            Assert.AreEqual(1, ((ReferenceTypeArray) variable.VariableType.References[2]).ArraySize);
            Assert.IsInstanceOfType(variable.VariableType.References[3], typeof(ReferenceTypeArray));
            Assert.AreEqual(3, ((ReferenceTypeArray) variable.VariableType.References[3]).ArraySize);

            Assert.AreEqual(DataTypeBaseType.UNSIGNED_CHAR, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureAcceptsConstVariables()
        {
            tokens.AddRange(new List<string>
            {
                "const", "int", "test", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(4, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("test", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsTrue(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(0, variable.VariableType.References.Count);
            Assert.AreEqual(DataTypeBaseType.INT, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureAcceptsConstPointerVariables()
        {
            tokens.AddRange(new List<string>
            {
                "const", "char", "*", "str", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(5, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("str", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsTrue(variable.VariableType.IsConst);
            Assert.IsFalse(variable.VariableType.HasCustomBitSize);

            Assert.AreEqual(1, variable.VariableType.References.Count);
            Assert.IsInstanceOfType(variable.VariableType.References[0], typeof(ReferenceTypePointer));
            Assert.AreEqual(DataTypeBaseType.CHAR, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureAcceptsCustomBitSizes()
        {
            tokens.AddRange(new List<string>
            {
                "int", "test", ":", "3", ";"
            });

            var block = new BlockStruct(parserStateMock.Object, "test_struct", false);
            blocks.Push(block);

            var test = new TestVariable();

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserStateMock.Object, lexerMock.Object));
            Assert.AreEqual(5, test.ConsumedTokenCount);

            Assert.AreEqual(1, block.Variables.Count);

            var variable = block.Variables[0];

            Assert.AreEqual("test", variable.Name);
            Assert.IsNull(variable.AlignmentOverride);
            Assert.IsFalse(variable.VariableType.IsConst);

            Assert.IsTrue(variable.VariableType.HasCustomBitSize);
            Assert.AreEqual(3, variable.VariableType.CustomBitSize);

            Assert.AreEqual(0, variable.VariableType.References.Count);
            Assert.AreEqual(DataTypeBaseType.INT, variable.VariableType.Type);
        }

        [TestMethod]
        public void EnsureThrowsExceptionWhenNotInBlockThatCanHoldVariables()
        {
            tokens.AddRange(new List<string>
            {
                "int", "test", ";"
            });

            var block = new BlockNamespace(parserStateMock.Object, "asdf");
            blocks.Push(block);

            var test = new TestVariable();

            Assert.ThrowsException<Exception>(() => test.PerformTest(parserStateMock.Object, lexerMock.Object));
        }
    }
}

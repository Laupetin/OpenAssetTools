using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Moq;
using ZoneCodeGenerator.Domain;
using ZoneCodeGenerator.Domain.Evaluation;
using ZoneCodeGenerator.Domain.Information;
using ZoneCodeGenerator.Parsing;
using ZoneCodeGenerator.Parsing.CommandFile.Impl;
using ZoneCodeGenerator.Parsing.CommandFile.Tests;
using ZoneCodeGenerator.Parsing.Testing;
using ZoneCodeGenerator.Persistence;

namespace ZoneCodeGeneratorTests.Parsing.CommandFile.Tests
{
    [TestClass]
    public class TestCountTest
    {
        private IDataRepository repository;
        private CommandParserState parserState;

        private Mock<ILexer> lexerMock;
        private int tokenOffset;
        private List<string> tokens;

        [TestInitialize]
        public void Setup()
        {
            tokenOffset = 0;
            tokens = new List<string>();
            lexerMock = new Mock<ILexer>();

            repository = new InMemoryDataRepository();
            parserState = new CommandParserState(repository);

            lexerMock.Setup(lexer => lexer.PeekToken(It.IsAny<int>()))
                .Returns((int index) => tokens.ElementAtOrDefault(index + tokenOffset));
            lexerMock.Setup(lexer => lexer.NextToken())
                .Returns(() => tokens.ElementAtOrDefault(tokenOffset++));
            lexerMock.Setup(lexer => lexer.SkipTokens(It.IsAny<int>()))
                .Callback((int count) => tokenOffset += count);
        }

        [TestMethod]
        public void EnsureCanHandleSimpleCountStatement()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "5", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            repository.Add(testStruct);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(8, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsTrue(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(OperandStatic));
            Assert.AreEqual(5, ((OperandStatic) pointerReference.Count).Value);
        }

        [TestMethod]
        public void EnsureCanHandleReferenceToSubStruct()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "papaStruct", ":", ":", "kiddo", ":", ":", "ptrEntry", "5", ";"
            });

            var test = new TestCount();

            var parentStruct = new DataTypeStruct("", "papaStruct", 4);
            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var childEntry = new Variable("kiddo", new TypeDeclaration(testStruct, new List<ReferenceType>()));
            parentStruct.Members.Add(childEntry);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            repository.Add(testStruct);
            repository.Add(parentStruct);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(11, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsTrue(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(OperandStatic));
            Assert.AreEqual(5, ((OperandStatic) pointerReference.Count).Value);
        }

        [TestMethod]
        public void EnsureFailsWhenStructureCannotBeFound()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "5", ";"
            });

            var test = new TestCount();

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserState, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureFailsWhenReferencingStructAndNotMembers()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", "5", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>()));
            testStruct.Members.Add(ptrEntry);

            repository.Add(testStruct);

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserState, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureFailsWhenReferencedMemberHasNoPointerReference()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "5", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>()));
            testStruct.Members.Add(ptrEntry);

            repository.Add(testStruct);

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserState, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureCanHandleMultipleOperations()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "1", "+", "countEntry", "*", "2", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT, new List<ReferenceType>()));
            testStruct.Members.Add(countEntry);

            repository.Add(testStruct);
            var testStructInfo = repository.GetInformationFor(testStruct);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(12, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsFalse(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(Operation));

            var countOperation = (Operation) pointerReference.Count;
            Assert.AreEqual(OperationType.OperationAdd, countOperation.OperationType);

            Assert.IsInstanceOfType(countOperation.Operand1, typeof(OperandStatic));
            Assert.AreEqual(1, ((OperandStatic) countOperation.Operand1).Value);

            Assert.IsInstanceOfType(countOperation.Operand2, typeof(Operation));
            var multiplicationOperation = (Operation) countOperation.Operand2;
            Assert.AreEqual(OperationType.OperationMultiply, multiplicationOperation.OperationType);

            Assert.IsInstanceOfType(multiplicationOperation.Operand2, typeof(OperandStatic));
            Assert.AreEqual(2, ((OperandStatic) multiplicationOperation.Operand2).Value);

            Assert.IsInstanceOfType(multiplicationOperation.Operand1, typeof(OperandDynamic));
            var ptrEntryDynamicOperand = (OperandDynamic) multiplicationOperation.Operand1;
            Assert.AreEqual(testStructInfo, ptrEntryDynamicOperand.Structure);
            Assert.AreEqual(1, ptrEntryDynamicOperand.ReferencedMemberChain.Count);
            Assert.AreEqual(countEntry, ptrEntryDynamicOperand.ReferencedMemberChain[0].Member);
            Assert.AreEqual(0, ptrEntryDynamicOperand.ArrayIndices.Count);
        }

        [TestMethod]
        public void EnsureCanHandleParenthesis()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "(", "1", "+", "countEntry", ")", "*", "2", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT, new List<ReferenceType>()));
            testStruct.Members.Add(countEntry);

            repository.Add(testStruct);
            var testStructInfo = repository.GetInformationFor(testStruct);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(14, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsFalse(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(Operation));

            var countOperation = (Operation) pointerReference.Count;
            Assert.AreEqual(OperationType.OperationMultiply, countOperation.OperationType);

            Assert.IsInstanceOfType(countOperation.Operand2, typeof(OperandStatic));
            Assert.AreEqual(2, ((OperandStatic) countOperation.Operand2).Value);

            Assert.IsInstanceOfType(countOperation.Operand1, typeof(Operation));
            var additionOperation = (Operation) countOperation.Operand1;
            Assert.AreEqual(OperationType.OperationAdd, additionOperation.OperationType);

            Assert.IsInstanceOfType(additionOperation.Operand1, typeof(OperandStatic));
            Assert.AreEqual(1, ((OperandStatic) additionOperation.Operand1).Value);

            Assert.IsInstanceOfType(additionOperation.Operand2, typeof(OperandDynamic));
            var ptrEntryDynamicOperand = (OperandDynamic) additionOperation.Operand2;
            Assert.AreEqual(testStructInfo, ptrEntryDynamicOperand.Structure);
            Assert.AreEqual(1, ptrEntryDynamicOperand.ReferencedMemberChain.Count);
            Assert.AreEqual(countEntry, ptrEntryDynamicOperand.ReferencedMemberChain[0].Member);
            Assert.AreEqual(0, ptrEntryDynamicOperand.ArrayIndices.Count);
        }

        [TestMethod]
        public void EnsureCanHandleReferencesToDifferentStructs()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "1", "+", "someOtherStruct", ":", ":",
                "countEntry", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);
            var someOtherStruct = new DataTypeStruct("", "someOtherStruct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT, new List<ReferenceType>()));
            someOtherStruct.Members.Add(countEntry);

            repository.Add(testStruct);
            repository.Add(someOtherStruct);

            var someOtherStructInfo = repository.GetInformationFor(someOtherStruct);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(13, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsFalse(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(Operation));

            var countOperation = (Operation) pointerReference.Count;
            Assert.AreEqual(OperationType.OperationAdd, countOperation.OperationType);

            Assert.IsInstanceOfType(countOperation.Operand1, typeof(OperandStatic));
            Assert.AreEqual(1, ((OperandStatic) countOperation.Operand1).Value);

            Assert.IsInstanceOfType(countOperation.Operand2, typeof(OperandDynamic));
            var dynamicOperand = (OperandDynamic) countOperation.Operand2;
            Assert.AreEqual(someOtherStructInfo, dynamicOperand.Structure);
            Assert.AreEqual(1, dynamicOperand.ReferencedMemberChain.Count);
            Assert.AreEqual(countEntry, dynamicOperand.ReferencedMemberChain[0].Member);
            Assert.AreEqual(0, dynamicOperand.ArrayIndices.Count);
        }

        [TestMethod]
        public void EnsureCanHandleArrayIndices()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "1", "+", "countEntry", "[", "7", "]", "[", "4",
                "]", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT,
                    new List<ReferenceType>(new[] {new ReferenceTypeArray(9), new ReferenceTypeArray(9)})));
            testStruct.Members.Add(countEntry);

            repository.Add(testStruct);
            var testStructInfo = repository.GetInformationFor(testStruct);

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(16, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsFalse(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(Operation));

            var countOperation = (Operation) pointerReference.Count;
            Assert.AreEqual(OperationType.OperationAdd, countOperation.OperationType);

            Assert.IsInstanceOfType(countOperation.Operand1, typeof(OperandStatic));
            Assert.AreEqual(1, ((OperandStatic) countOperation.Operand1).Value);

            Assert.IsInstanceOfType(countOperation.Operand2, typeof(OperandDynamic));
            var dynamicOperand = (OperandDynamic) countOperation.Operand2;
            Assert.AreEqual(testStructInfo, dynamicOperand.Structure);
            Assert.AreEqual(1, dynamicOperand.ReferencedMemberChain.Count);
            Assert.AreEqual(countEntry, dynamicOperand.ReferencedMemberChain[0].Member);

            Assert.AreEqual(2, dynamicOperand.ArrayIndices.Count);
            Assert.AreEqual(7, dynamicOperand.ArrayIndices[0]);
            Assert.AreEqual(4, dynamicOperand.ArrayIndices[1]);
        }

        [TestMethod]
        public void EnsureFailsWhenReferencingInvalidStructInDynamicOperand()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "1", "+", "someOtherStruct", ":", ":",
                "countEntry", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT,
                    new List<ReferenceType>(new[] {new ReferenceTypeArray(9), new ReferenceTypeArray(9)})));
            testStruct.Members.Add(countEntry);

            repository.Add(testStruct);

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserState, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureFailsWhenReferencingNoMembersAsDynamicOperands()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "1", "+", "test_struct", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT,
                    new List<ReferenceType>(new[] {new ReferenceTypeArray(9), new ReferenceTypeArray(9)})));
            testStruct.Members.Add(countEntry);

            repository.Add(testStruct);

            Assert.ThrowsException<TestFailedException>(() => test.PerformTest(parserState, lexerMock.Object));
        }

        [TestMethod]
        public void EnsureCanReferenceUsingUsedStructure()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "ptrEntry", "5", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            repository.Add(testStruct);
            var testStructInfo = repository.GetInformationFor(testStruct);
            parserState.DataTypeInUse = testStructInfo;

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(5, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsTrue(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(OperandStatic));
            Assert.AreEqual(5, ((OperandStatic) pointerReference.Count).Value);
        }

        [TestMethod]
        public void EnsureCanHandleReferencesToUsedStructure()
        {
            tokens.AddRange(new List<string>
            {
                "set", "count", "test_struct", ":", ":", "ptrEntry", "1", "+", "countEntry", ";"
            });

            var test = new TestCount();

            var testStruct = new DataTypeStruct("", "test_struct", 4);
            var someOtherStruct = new DataTypeStruct("", "someOtherStruct", 4);

            var pointerReference = new ReferenceTypePointer();
            var ptrEntry = new Variable("ptrEntry",
                new TypeDeclaration(DataTypeBaseType.INT, new List<ReferenceType>(new[] {pointerReference})));
            testStruct.Members.Add(ptrEntry);

            var countEntry = new Variable("countEntry",
                new TypeDeclaration(DataTypeBaseType.UNSIGNED_INT, new List<ReferenceType>()));
            someOtherStruct.Members.Add(countEntry);

            repository.Add(testStruct);

            repository.Add(someOtherStruct);
            var someOtherStructInfo = repository.GetInformationFor(someOtherStruct);
            parserState.DataTypeInUse = someOtherStructInfo;

            Assert.AreEqual(TokenTestResult.Match, test.PerformTest(parserState, lexerMock.Object));
            Assert.AreEqual(10, test.ConsumedTokenCount);

            Assert.AreNotEqual(ReferenceTypePointer.DefaultCount, pointerReference.Count);
            Assert.IsFalse(pointerReference.Count.IsStatic);

            Assert.IsInstanceOfType(pointerReference.Count, typeof(Operation));

            var countOperation = (Operation) pointerReference.Count;
            Assert.AreEqual(OperationType.OperationAdd, countOperation.OperationType);

            Assert.IsInstanceOfType(countOperation.Operand1, typeof(OperandStatic));
            Assert.AreEqual(1, ((OperandStatic) countOperation.Operand1).Value);

            Assert.IsInstanceOfType(countOperation.Operand2, typeof(OperandDynamic));
            var dynamicOperand = (OperandDynamic) countOperation.Operand2;
            Assert.AreEqual(someOtherStructInfo, dynamicOperand.Structure);
            Assert.AreEqual(1, dynamicOperand.ReferencedMemberChain.Count);
            Assert.AreEqual(countEntry, dynamicOperand.ReferencedMemberChain[0].Member);
            Assert.AreEqual(0, dynamicOperand.ArrayIndices.Count);
        }
    }
}
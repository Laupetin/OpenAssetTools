using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using ZoneCodeGenerator.Domain;

namespace ZoneCodeGeneratorTests.Domain
{
    [TestClass]
    public class NamespaceTest
    {
        [TestMethod]
        public void EnsureEmptyNamespaceResultIsCorrect()
        {
            var nameSpace = new Namespace();
            
            Assert.AreEqual("", nameSpace.GetName());
        }

        [TestMethod]
        public void EnsureSingleNamespaceElementResultIsCorrect()
        {
            var nameSpace = new Namespace();

            nameSpace.Push("NameTheSpace");
            
            Assert.AreEqual("NameTheSpace", nameSpace.GetName());
        }

        [TestMethod]
        public void EnsureDoubleNamespaceElementResultIsCorrect()
        {
            var nameSpace = new Namespace();

            nameSpace.Push("NameTheSpace");
            nameSpace.Push("Please");
            
            Assert.AreEqual("NameTheSpace::Please", nameSpace.GetName());
        }

        [TestMethod]
        public void EnsurePopRemovesNamespace()
        {
            var nameSpace = new Namespace();

            nameSpace.Push("NameTheSpace");
            nameSpace.Push("Please");
            nameSpace.Push("Not");
            
            Assert.AreEqual("Not", nameSpace.Pop());
            Assert.AreEqual("NameTheSpace::Please", nameSpace.GetName());
        }

        [TestMethod]
        public void EnsureMultipleNamespaceElementsResultIsCorrect()
        {
            var nameSpace = new Namespace();

            nameSpace.Push("NameTheSpace");
            nameSpace.Push("Please");
            nameSpace.Push("Just");
            nameSpace.Push("Not");
            nameSpace.Push("MySpace");
            
            Assert.AreEqual("NameTheSpace::Please::Just::Not::MySpace", nameSpace.GetName());
        }

        [TestMethod]
        public void EnsureNamespaceCanBePoppedToEmpty()
        {
            var nameSpace = new Namespace();

            nameSpace.Push("NameTheSpace");
            nameSpace.Push("Please");

            Assert.AreEqual("Please", nameSpace.Pop());
            Assert.AreEqual("NameTheSpace", nameSpace.Pop());
            
            Assert.AreEqual("", nameSpace.GetName());
        }

        [TestMethod]
        public void EnsureUsingPopTooOftenResultsInAnError()
        {
            var nameSpace = new Namespace();

            nameSpace.Push("NameTheSpace");
            nameSpace.Push("Please");

            Assert.AreEqual("Please", nameSpace.Pop());
            Assert.AreEqual("NameTheSpace", nameSpace.Pop());
            Assert.ThrowsException<InvalidOperationException>(() => nameSpace.Pop());
            
            Assert.AreEqual("", nameSpace.GetName());
        }
    }
}

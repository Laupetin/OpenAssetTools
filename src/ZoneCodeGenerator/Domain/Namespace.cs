using System;
using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace ZoneCodeGenerator.Domain
{
    class Namespace
    {
        private static readonly Regex nameRegex = new Regex(@"^[a-zA-Z_$][a-zA-Z0-9_$]*$");
        private readonly Stack<string> namespaceStack;

        public Namespace()
        {
            namespaceStack = new Stack<string>();
        }

        public void Push(string _namespace)
        {
            if(!nameRegex.IsMatch(_namespace))
                throw new ArgumentException("Namespace name invalid");

            namespaceStack.Push(_namespace);
        }

        public string Pop()
        {
            return namespaceStack.Pop();
        }

        public string GetName()
        {
            if (namespaceStack.Count == 0)
                return "";

            var result = "";
            var stackSnapshot = namespaceStack.ToArray();

            // The stack is read from top to bottom. Therefore we need to access it in reverse order here.
            for(var i = stackSnapshot.Length - 1; i >= 0; i--)
            {
                if (!string.IsNullOrEmpty(result))
                    result += "::";
                result += stackSnapshot[i];
            }

            return result;
        }

        public override string ToString()
        {
            return GetName();
        }

        public static string Combine(Namespace _namespace, string typename)
        {
            return $"{_namespace}::{typename}";
        }
    }
}

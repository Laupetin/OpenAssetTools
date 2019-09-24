using System;
using ZoneCodeGenerator.Interface;
using ZoneCodeGenerator.Parsing.C_Header;

namespace ZoneCodeGenerator
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static int Main(string[] args)
        {
            var cui = new CUI();

            return cui.Start(args) ? 0 : 1;
        }

        private static bool CreateDatabase(string[] args)
        {
            var storage = HeaderReader.ReadFile(args[0]);

            var prettyPrinter = new PrettyPrinter(storage);

            prettyPrinter.PrintAll();

            return true;
        }

        private static bool PrintDatabase(string[] args)
        {
            Console.WriteLine("Printing");
            return true;
        }
    }
}

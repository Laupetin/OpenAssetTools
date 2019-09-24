using System.IO;
using Antlr4.StringTemplate;

namespace ZoneCodeGenerator.Generating
{
    class CodeTemplate
    {
        private const string SourceTemplateName = "source";
        private const string HeaderTemplateName = "header";

        private readonly TemplateGroup templateGroup;

        public bool HasSourceTemplate => templateGroup.IsDefined(SourceTemplateName);
        public bool HasHeaderTemplate => templateGroup.IsDefined(HeaderTemplateName);

        private CodeTemplate(TemplateGroup templateGroup)
        {
            this.templateGroup = templateGroup;
            templateGroup.RegisterRenderer(typeof(string), new StringRenderer());
        }

        public static CodeTemplate FromResources(string fileName)
        {
            var templateGroup = new TemplateGroupResources(fileName);

            templateGroup.Load();

            return new CodeTemplate(templateGroup);
        }

        public void RenderSourceFile(Stream stream, RenderingContext context)
        {
            RenderTemplate(stream, templateGroup.GetInstanceOf(SourceTemplateName), context);
        }

        public void RenderHeaderFile(Stream stream, RenderingContext context)
        {
            RenderTemplate(stream, templateGroup.GetInstanceOf(HeaderTemplateName), context);
        }

        private static void RenderTemplate(Stream stream, Template template, RenderingContext context)
        {
            if (template == null)
                return;

            template.Add("context", context);

            using (var writer = new StreamWriter(stream))
            {
                template.Write(new AutoIndentWriter(writer));
            }
        }
    }
}

using System.Globalization;
using Antlr4.StringTemplate;
using ZoneCodeGenerator.Domain;

namespace ZoneCodeGenerator.Generating
{
    class DataTypeAttributeRenderer : IAttributeRenderer
    {
        public string ToString(object obj, string formatString, CultureInfo culture)
        {
            if (formatString == null)
                return obj.ToString();

            if (!(obj is DataType dataType))
            {
                return obj.ToString();
            }

            switch (formatString)
            {
                case "safe_name":
                    return dataType.Name.Replace(" ", "_");

                default:
                    return obj.ToString();
            }
        }
    }
}
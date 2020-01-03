using System;
using System.Collections.Generic;

namespace ZoneCodeGenerator.Domain
{
    class CustomAction
    {
        public string ActionName { get; }
        public IList<DataType> ParameterTypes { get; }

        public CustomAction(string actionName, IEnumerable<DataType> parameterTypes)
        {
            ActionName = actionName;
            ParameterTypes = new List<DataType>(parameterTypes);
        }
    }
}
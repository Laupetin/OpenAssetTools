using System;
using System.Reflection;

namespace ZoneCodeGeneratorTests
{
    internal static class FieldInjector
    {
        public static bool InjectToField(object target, object toInject)
        {
            if(target == null || toInject == null)
                throw new ArgumentException("Params must not be null");

            var injectType = toInject.GetType();
            var fields = target.GetType().GetFields(BindingFlags.NonPublic | BindingFlags.Instance);

            foreach(var field in fields)
            {
                if (!field.FieldType.IsAssignableFrom(injectType)) continue;

                field.SetValue(target, toInject);
                return true;
            }

            return false;
        }
    }
}

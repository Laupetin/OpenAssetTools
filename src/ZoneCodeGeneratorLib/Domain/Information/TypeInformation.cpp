#include "TypeInformation.h"

TypeInformation::TypeInformation(DataDefinition* definition)
    : m_definition(definition),
      m_reusable_reference_exists(false)
{
}

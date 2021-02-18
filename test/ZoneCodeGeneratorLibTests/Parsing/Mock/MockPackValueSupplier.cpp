#include "MockPackValueSupplier.h"

#include "Parsing/Impl/PackDefinitionStreamProxy.h"

int MockPackValueSupplier::GetCurrentPack() const
{
    return PackDefinitionStreamProxy::DEFAULT_PACK;
}

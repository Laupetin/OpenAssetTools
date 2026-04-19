#pragma once

#include "Game/IW3/IW3.h"
#include "Weapon/IWeaponWriter.h"

#include <memory>
#include <string>

namespace weapon
{
    class IWriterIW3 : public IWriter
    {
    public:
        virtual void WriteWeapon(const IW3::WeaponDef& physPreset) = 0;
    };

    std::unique_ptr<IWriterIW3> CreateWeaponWriterIW3(std::ostream& stream);
} // namespace weapon

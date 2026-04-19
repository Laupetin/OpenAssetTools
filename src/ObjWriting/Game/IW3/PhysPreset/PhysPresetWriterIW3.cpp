#include "PhysPresetWriterIW3.h"

//#include "Game/IW3/FontConstantsIW3.h"
#include "PhysPreset/AbstractPhysPresetWriter.h"
#include "ObjWriting.h"

#include <cassert>
#include <cmath>
#include <sstream>

using namespace IW3;

namespace
{
    class PhysPresetWriter final : public ::physpreset::AbstractBaseWriter, public physpreset::IWriterIW3
    {
    public:
        explicit PhysPresetWriter(std::ostream& stream)
            : AbstractBaseWriter(stream)
        {
        }

        void WritePhysPreset(const PhysPreset& physPreset) override
        {
            m_stream << "PHYSIC\\";

            if (physPreset.sndAliasPrefix)
            {
                m_stream << "sndAliasPrefix\\" << physPreset.sndAliasPrefix << "\\";
            }
            m_stream << "mass\\" << physPreset.mass << "\\";
            m_stream << "friction\\" << physPreset.friction << "\\";
            m_stream << "isFrictionInfinity\\0\\";
            m_stream << "bounce\\" << physPreset.bounce << "\\";
            m_stream << "bulletForceScale\\" << physPreset.bulletForceScale << "\\";
            m_stream << "explosiveForceScale\\" << physPreset.explosiveForceScale << "\\";
            m_stream << "piecesSpreadFraction\\" << physPreset.piecesSpreadFraction << "\\";
            m_stream << "piecesUpwardVelocity\\" << physPreset.piecesUpwardVelocity << "\\";
            m_stream << "tempDefaultToCylinder\\" << physPreset.tempDefaultToCylinder;

            return;
        }

    private:
    };
} // namespace

namespace physpreset
{
    std::unique_ptr<IWriterIW3> CreatePhysPresetWriterIW3(std::ostream& stream)
    {
        return std::make_unique<PhysPresetWriter>(stream);
    }
} // namespace menu

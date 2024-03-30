#pragma once

#include "GltfOutput.h"
#include "XModel/AbstractXModelWriter.h"
#include "XModel/Gltf/JsonGltf.h"

#include <memory>
#include <ostream>

namespace gltf
{
    class Writer : public AbstractXModelWriter
    {
    public:
        Writer() = default;
        virtual ~Writer() = default;
        Writer(const Writer& other) = default;
        Writer(Writer&& other) noexcept = default;
        Writer& operator=(const Writer& other) = default;
        Writer& operator=(Writer&& other) noexcept = default;

        virtual void Write(std::ostream& stream) = 0;

        static std::unique_ptr<Writer> CreateWriter(const Output* output, std::string gameName, std::string zoneName);
    };
} // namespace gltf

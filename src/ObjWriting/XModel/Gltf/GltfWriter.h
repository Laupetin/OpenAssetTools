#pragma once

#include "GltfOutput.h"
#include "XModel/Gltf/JsonGltf.h"
#include "XModel/XModelWriter.h"

#include <memory>
#include <ostream>

namespace gltf
{
    class Writer : public XModelWriter
    {
    public:
        Writer() = default;
        ~Writer() override = default;
        Writer(const Writer& other) = default;
        Writer(Writer&& other) noexcept = default;
        Writer& operator=(const Writer& other) = default;
        Writer& operator=(Writer&& other) noexcept = default;

        static std::unique_ptr<Writer> CreateWriter(const Output* output, std::string gameName, std::string zoneName);
    };
} // namespace gltf

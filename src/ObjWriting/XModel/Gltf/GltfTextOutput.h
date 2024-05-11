#pragma once

#include "GltfOutput.h"

#include <ostream>

namespace gltf
{
    class TextOutput final : public Output
    {
    public:
        explicit TextOutput(std::ostream& stream);

        std::optional<std::string> CreateBufferUri(const void* buffer, size_t bufferSize) const override;
        void EmitJson(const nlohmann::json& json) const override;
        void EmitBuffer(const void* buffer, size_t bufferSize) const override;
        void Finalize() const override;

    private:
        std::ostream& m_stream;
    };
} // namespace gltf

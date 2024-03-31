#pragma once

#include "GltfOutput.h"

#include <ostream>

namespace gltf
{
    class BinOutput final : public Output
    {
    public:
        explicit BinOutput(std::ostream& stream);

        std::optional<std::string> CreateBufferUri(const void* buffer, size_t bufferSize) const override;
        void EmitJson(const nlohmann::json& json) const override;
        void EmitBuffer(const void* buffer, size_t bufferSize) const override;
        void Finalize() const override;

    private:
        void Write(const void* data, size_t dataSize) const;
        void AlignToFour(char value) const;

        std::ostream& m_stream;
    };
} // namespace gltf

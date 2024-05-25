#pragma once

#include "GltfInput.h"

#include <istream>

namespace gltf
{
    class TextInput final : public Input
    {
    public:
        TextInput();

        bool ReadGltfData(std::istream& stream) override;
        bool GetEmbeddedBuffer(const void*& buffer, size_t& bufferSize) const override;
        [[nodiscard]] const nlohmann::json& GetJson() const override;

    private:
        std::unique_ptr<nlohmann::json> m_json;
        std::unique_ptr<uint8_t[]> m_buffer;
        size_t m_buffer_size;
    };
} // namespace gltf

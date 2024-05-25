#pragma once

#include "GltfInput.h"

#include <istream>

namespace gltf
{
    class BinInput final : public Input
    {
    public:
        BinInput();

        bool ReadGltfData(std::istream& stream) override;
        bool GetEmbeddedBuffer(const void*& buffer, size_t& bufferSize) const override;
        [[nodiscard]] const nlohmann::json& GetJson() const override;

    private:
        static bool Read(std::istream& stream, void* dest, size_t dataSize, const char* readTypeName, bool errorWhenFailed = true);
        static void Skip(std::istream& stream, size_t skipLength);

        std::unique_ptr<nlohmann::json> m_json;
        std::unique_ptr<uint8_t[]> m_buffer;
        size_t m_buffer_size;
    };
} // namespace gltf

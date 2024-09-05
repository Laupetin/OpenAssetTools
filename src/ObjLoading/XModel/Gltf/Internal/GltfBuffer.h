#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace gltf
{
    class Buffer
    {
    protected:
        Buffer() = default;

    public:
        virtual ~Buffer() = default;
        Buffer(const Buffer& other) = default;
        Buffer(Buffer&& other) noexcept = default;
        Buffer& operator=(const Buffer& other) = default;
        Buffer& operator=(Buffer&& other) noexcept = default;

        virtual bool ReadData(void* dest, size_t offset, size_t count) const = 0;
        [[nodiscard]] virtual size_t GetSize() const = 0;
    };

    class EmbeddedBuffer final : public Buffer
    {
    public:
        EmbeddedBuffer(const void* data, size_t dataSize);

        bool ReadData(void* dest, size_t offset, size_t count) const override;
        [[nodiscard]] size_t GetSize() const override;

    private:
        const uint8_t* m_data;
        size_t m_data_size;
    };

    class DataUriBuffer final : public Buffer
    {
    public:
        DataUriBuffer();

        static bool IsDataUri(const std::string& uri);
        bool ReadDataFromUri(const std::string& uri);

        bool ReadData(void* dest, size_t offset, size_t count) const override;
        [[nodiscard]] size_t GetSize() const override;

    private:
        std::unique_ptr<uint8_t[]> m_data;
        size_t m_data_size;
    };
} // namespace gltf

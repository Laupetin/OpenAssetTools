#pragma once
#include "GltfBufferView.h"
#include "XModel/Gltf/JsonGltf.h"

namespace gltf
{
    class Accessor
    {
    protected:
        Accessor() = default;

    public:
        virtual ~Accessor() = default;
        Accessor(const Accessor& other) = default;
        Accessor(Accessor&& other) noexcept = default;
        Accessor& operator=(const Accessor& other) = default;
        Accessor& operator=(Accessor&& other) noexcept = default;

        [[nodiscard]] virtual std::optional<JsonAccessorType> GetType() const = 0;
        [[nodiscard]] virtual std::optional<JsonAccessorComponentType> GetComponentType() const = 0;
        [[nodiscard]] virtual size_t GetCount() const = 0;

        [[nodiscard]] virtual bool GetFloatVec2(size_t index, float (&out)[2]) const = 0;
        [[nodiscard]] virtual bool GetFloatVec3(size_t index, float (&out)[3]) const = 0;
        [[nodiscard]] virtual bool GetFloatVec4(size_t index, float (&out)[4]) const = 0;
        [[nodiscard]] virtual bool GetUnsigned(size_t index, unsigned& out) const = 0;
        [[nodiscard]] virtual bool GetUnsignedVec4(size_t index, unsigned (&out)[4]) const = 0;
    };

    class NullAccessor final : public Accessor
    {
    public:
        explicit NullAccessor(size_t count);

        [[nodiscard]] std::optional<JsonAccessorType> GetType() const override;
        [[nodiscard]] std::optional<JsonAccessorComponentType> GetComponentType() const override;
        [[nodiscard]] size_t GetCount() const override;

        [[nodiscard]] bool GetFloatVec2(size_t index, float (&out)[2]) const override;
        [[nodiscard]] bool GetFloatVec3(size_t index, float (&out)[3]) const override;
        [[nodiscard]] bool GetFloatVec4(size_t index, float (&out)[4]) const override;
        [[nodiscard]] bool GetUnsigned(size_t index, unsigned& out) const override;
        [[nodiscard]] bool GetUnsignedVec4(size_t index, unsigned (&out)[4]) const override;

    private:
        size_t m_count;
    };

    class FloatAccessor final : public Accessor
    {
    public:
        FloatAccessor(const BufferView* bufferView, JsonAccessorType type, size_t count);

        [[nodiscard]] std::optional<JsonAccessorType> GetType() const override;
        [[nodiscard]] std::optional<JsonAccessorComponentType> GetComponentType() const override;
        [[nodiscard]] size_t GetCount() const override;

        [[nodiscard]] bool GetFloatVec2(size_t index, float (&out)[2]) const override;
        [[nodiscard]] bool GetFloatVec3(size_t index, float (&out)[3]) const override;
        [[nodiscard]] bool GetFloatVec4(size_t index, float (&out)[4]) const override;
        [[nodiscard]] bool GetUnsigned(size_t index, unsigned& out) const override;
        [[nodiscard]] bool GetUnsignedVec4(size_t index, unsigned (&out)[4]) const override;

    private:
        const BufferView* m_buffer_view;
        JsonAccessorType m_type;
        size_t m_count;
    };

    class UnsignedByteAccessor final : public Accessor
    {
    public:
        UnsignedByteAccessor(const BufferView* bufferView, JsonAccessorType type, size_t count);

        [[nodiscard]] std::optional<JsonAccessorType> GetType() const override;
        [[nodiscard]] std::optional<JsonAccessorComponentType> GetComponentType() const override;
        [[nodiscard]] size_t GetCount() const override;

        [[nodiscard]] bool GetFloatVec2(size_t index, float (&out)[2]) const override;
        [[nodiscard]] bool GetFloatVec3(size_t index, float (&out)[3]) const override;
        [[nodiscard]] bool GetFloatVec4(size_t index, float (&out)[4]) const override;
        [[nodiscard]] bool GetUnsigned(size_t index, unsigned& out) const override;
        [[nodiscard]] bool GetUnsignedVec4(size_t index, unsigned (&out)[4]) const override;

    private:
        const BufferView* m_buffer_view;
        JsonAccessorType m_type;
        size_t m_count;
    };

    class UnsignedShortAccessor final : public Accessor
    {
    public:
        UnsignedShortAccessor(const BufferView* bufferView, JsonAccessorType type, size_t count);

        [[nodiscard]] std::optional<JsonAccessorType> GetType() const override;
        [[nodiscard]] std::optional<JsonAccessorComponentType> GetComponentType() const override;
        [[nodiscard]] size_t GetCount() const override;

        [[nodiscard]] bool GetFloatVec2(size_t index, float (&out)[2]) const override;
        [[nodiscard]] bool GetFloatVec3(size_t index, float (&out)[3]) const override;
        [[nodiscard]] bool GetFloatVec4(size_t index, float (&out)[4]) const override;
        [[nodiscard]] bool GetUnsigned(size_t index, unsigned& out) const override;
        [[nodiscard]] bool GetUnsignedVec4(size_t index, unsigned (&out)[4]) const override;

    private:
        const BufferView* m_buffer_view;
        JsonAccessorType m_type;
        size_t m_count;
    };
} // namespace gltf

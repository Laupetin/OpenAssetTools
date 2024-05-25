#pragma once

#include <cstdlib>
#include <nlohmann/json_fwd.hpp>

namespace gltf
{
    class Input
    {
    protected:
        Input() = default;

    public:
        virtual ~Input() = default;
        Input(const Input& other) = default;
        Input(Input&& other) noexcept = default;
        Input& operator=(const Input& other) = default;
        Input& operator=(Input&& other) noexcept = default;

        virtual bool ReadGltfData(std::istream& stream) = 0;
        virtual bool GetEmbeddedBuffer(const void*& buffer, size_t& bufferSize) const = 0;
        [[nodiscard]] virtual const nlohmann::json& GetJson() const = 0;
    };
} // namespace gltf

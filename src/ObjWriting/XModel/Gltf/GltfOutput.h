#pragma once

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>

namespace gltf
{
    class Output
    {
    protected:
        Output() = default;
        virtual ~Output() = default;
        Output(const Output& other) = default;
        Output(Output&& other) noexcept = default;
        Output& operator=(const Output& other) = default;
        Output& operator=(Output&& other) noexcept = default;

    public:
        virtual std::optional<std::string> CreateBufferUri(const void* buffer, size_t bufferSize) const = 0;
        virtual void EmitJson(const nlohmann::json& json) const = 0;
        virtual void EmitBuffer(const void* buffer, size_t bufferSize) const = 0;
        virtual void Finalize() const = 0;
    };
} // namespace gltf

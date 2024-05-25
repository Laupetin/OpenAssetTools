#include "GltfTextInput.h"

#include <exception>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

using namespace gltf;

TextInput::TextInput()
    : m_buffer_size(0u)
{
}

bool TextInput::GetEmbeddedBuffer(const void*& buffer, size_t& bufferSize) const
{
    if (!m_buffer || !m_buffer_size)
        return false;

    buffer = m_buffer.get();
    bufferSize = m_buffer_size;

    return true;
}

const nlohmann::json& TextInput::GetJson() const
{
    if (!m_json)
        throw std::exception();

    return *m_json;
}

bool TextInput::ReadGltfData(std::istream& stream)
{
    try
    {
        m_json = std::make_unique<nlohmann::json>(nlohmann::json::parse(stream));

        return true;
    }
    catch (nlohmann::json::exception& e)
    {
        std::cerr << std::format("Failed to parse json of GLTF: {}", e.what());
    }

    return false;
}

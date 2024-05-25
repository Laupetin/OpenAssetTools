#include "GltfTextOutput.h"

#include "XModel/Gltf/GltfConstants.h"

#include <iomanip>
#include <nlohmann/json.hpp>

#define LTC_NO_PROTOTYPES
#include <tomcrypt.h>

using namespace gltf;

TextOutput::TextOutput(std::ostream& stream)
    : m_stream(stream)
{
}

std::optional<std::string> TextOutput::CreateBufferUri(const void* buffer, const size_t bufferSize) const
{
    const auto base64Length = 4u * ((bufferSize + 2u) / 3u);
    const auto base64BufferSize = URI_PREFIX_LENGTH + base64Length;

    std::string output(base64BufferSize, '\0');

    std::memcpy(output.data(), GLTF_DATA_URI_PREFIX, URI_PREFIX_LENGTH);

    unsigned long outLength = base64Length + 1u;
    const auto result = base64_encode(static_cast<const unsigned char*>(buffer), bufferSize, &output[URI_PREFIX_LENGTH], &outLength);

    assert(result == CRYPT_OK);
    assert(outLength == base64Length);

    return output;
}

void TextOutput::EmitJson(const nlohmann::json& json) const
{
    m_stream << std::setw(4) << json;
}

void TextOutput::EmitBuffer(const void* buffer, const size_t bufferSize) const
{
    // Nothing to do
}

void TextOutput::Finalize() const
{
    // Nothing to do
}

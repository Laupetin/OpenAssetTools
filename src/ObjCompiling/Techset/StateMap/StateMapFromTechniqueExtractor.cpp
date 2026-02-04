#include "StateMapFromTechniqueExtractor.h"

using namespace state_map;

std::string StateMapFromTechniqueExtractor::RetrieveStateMap()
{
    return std::move(m_state_map);
}

void StateMapFromTechniqueExtractor::AcceptNextPass() {}

bool StateMapFromTechniqueExtractor::AcceptEndPass(std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptStateMap(const std::string& stateMapName, std::string& errorMessage)
{
    m_state_map = stateMapName;
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptVertexShader(const std::string& vertexShaderName, std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptPixelShader(const std::string& pixelShaderName, std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptShaderConstantArgument(techset::ShaderSelector shader,
                                                                  techset::ShaderArgument shaderArgument,
                                                                  techset::ShaderArgumentCodeSource source,
                                                                  std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptShaderSamplerArgument(techset::ShaderSelector shader,
                                                                 techset::ShaderArgument shaderArgument,
                                                                 techset::ShaderArgumentCodeSource source,
                                                                 std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptShaderLiteralArgument(techset::ShaderSelector shader,
                                                                 techset::ShaderArgument shaderArgument,
                                                                 techset::ShaderArgumentLiteralSource source,
                                                                 std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptShaderMaterialArgument(techset::ShaderSelector shader,
                                                                  techset::ShaderArgument shaderArgument,
                                                                  techset::ShaderArgumentMaterialSource source,
                                                                  std::string& errorMessage)
{
    return true;
}

bool StateMapFromTechniqueExtractor::AcceptVertexStreamRouting(const std::string& destination, const std::string& source, std::string& errorMessage)
{
    return true;
}

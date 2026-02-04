#pragma once

#include "Techset/TechniqueDefinitionAcceptor.h"

#include <string>

namespace state_map
{
    class StateMapFromTechniqueExtractor final : public techset::ITechniqueDefinitionAcceptor
    {
    public:
        std::string RetrieveStateMap();

        void AcceptNextPass() override;
        bool AcceptEndPass(std::string& errorMessage) override;
        bool AcceptStateMap(const std::string& stateMapName, std::string& errorMessage) override;
        bool AcceptVertexShader(const std::string& vertexShaderName, std::string& errorMessage) override;
        bool AcceptPixelShader(const std::string& pixelShaderName, std::string& errorMessage) override;
        bool AcceptShaderConstantArgument(techset::ShaderSelector shader,
                                          techset::ShaderArgument shaderArgument,
                                          techset::ShaderArgumentCodeSource source,
                                          std::string& errorMessage) override;
        bool AcceptShaderSamplerArgument(techset::ShaderSelector shader,
                                         techset::ShaderArgument shaderArgument,
                                         techset::ShaderArgumentCodeSource source,
                                         std::string& errorMessage) override;
        bool AcceptShaderLiteralArgument(techset::ShaderSelector shader,
                                         techset::ShaderArgument shaderArgument,
                                         techset::ShaderArgumentLiteralSource source,
                                         std::string& errorMessage) override;
        bool AcceptShaderMaterialArgument(techset::ShaderSelector shader,
                                          techset::ShaderArgument shaderArgument,
                                          techset::ShaderArgumentMaterialSource source,
                                          std::string& errorMessage) override;
        bool AcceptVertexStreamRouting(const std::string& destination, const std::string& source, std::string& errorMessage) override;

    private:
        std::string m_state_map;
    };
} // namespace state_map

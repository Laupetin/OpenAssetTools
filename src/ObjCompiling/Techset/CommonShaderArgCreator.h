#pragma once

#include "Asset/AssetCreationContext.h"
#include "Techset/CommonTechnique.h"
#include "Utils/Result.h"

#include <array>
#include <memory>
#include <optional>
#include <string>

namespace techset
{
    class ITechniqueShaderLoader
    {
    public:
        ITechniqueShaderLoader() = default;
        virtual ~ITechniqueShaderLoader() = default;

        virtual std::optional<CommonTechniqueShaderBin> LoadVertexShader(const std::string& name) = 0;
        virtual std::optional<CommonTechniqueShaderBin> LoadPixelShader(const std::string& name) = 0;
    };

    class CommonShaderArgCreatorDestination
    {
    public:
        std::string m_argument_name;
        std::optional<unsigned> m_argument_index;

        CommonShaderArgCreatorDestination() = default;
        explicit CommonShaderArgCreatorDestination(std::string argumentName);
        CommonShaderArgCreatorDestination(std::string argumentName, unsigned argumentIndex);
    };

    class CommonShaderArgCreator
    {
    public:
        CommonShaderArgCreator() = default;
        virtual ~CommonShaderArgCreator() = default;

        virtual result::Expected<NoResult, std::string> EnterShader(CommonTechniqueShaderType shaderType, const std::string& name) = 0;
        virtual result::Expected<NoResult, std::string> LeaveShader() = 0;

        virtual result::Expected<NoResult, std::string>
            AcceptShaderConstantArgument(const CommonShaderArgCreatorDestination& destination, CommonCodeConstSource codeConstSource, unsigned sourceIndex) = 0;
        virtual result::Expected<NoResult, std::string> AcceptShaderSamplerArgument(const CommonShaderArgCreatorDestination& destination,
                                                                                    CommonCodeSamplerSource codeSamplerSource) = 0;
        virtual result::Expected<NoResult, std::string> AcceptShaderLiteralArgument(const CommonShaderArgCreatorDestination& destination,
                                                                                    const std::array<float, 4>& literalValue) = 0;
        virtual result::Expected<NoResult, std::string> AcceptShaderMaterialArgument(const CommonShaderArgCreatorDestination& destination,
                                                                                     unsigned nameHash) = 0;
        virtual result::Expected<NoResult, std::string> AcceptShaderMaterialArgument(const CommonShaderArgCreatorDestination& destination,
                                                                                     const std::string& nameValue) = 0;

        virtual result::Expected<NoResult, std::string> FinalizePass(techset::CommonTechnique& technique, CommonPass& pass) = 0;

        static std::unique_ptr<CommonShaderArgCreator>
            CreateDx9(ITechniqueShaderLoader& shaderLoader, AssetCreationContext& context, CommonCodeSourceInfos& commonCodeSourceInfos);
        static std::unique_ptr<CommonShaderArgCreator>
            CreateDx11(ITechniqueShaderLoader& shaderLoader, AssetCreationContext& context, CommonCodeSourceInfos& commonCodeSourceInfos);
    };
} // namespace techset

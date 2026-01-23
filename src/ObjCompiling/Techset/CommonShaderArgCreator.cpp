#include "CommonShaderArgCreator.h"

#include <algorithm>
#include <cassert>

namespace
{
    class BaseCommonShaderArgCreator : public techset::CommonShaderArgCreator
    {
    public:
        explicit BaseCommonShaderArgCreator(techset::ITechniqueShaderLoader& shaderLoader, techset::CommonCodeSourceInfos& commonCodeSourceInfos)
            : m_shader_loader(shaderLoader),
              m_common_code_source_infos(commonCodeSourceInfos),
              m_shader_type(techset::CommonTechniqueShaderType::VERTEX),
              m_bin{}
        {
        }

        result::Expected<NoResult, std::string> EnterShader(const techset::CommonTechniqueShaderType shaderType, const std::string& name) override
        {
            m_shader_type = shaderType;

            std::optional<techset::CommonTechniqueShaderBin> maybeShader;
            if (shaderType == techset::CommonTechniqueShaderType::VERTEX)
            {
                maybeShader = m_shader_loader.LoadVertexShader(name);
            }
            else if (shaderType == techset::CommonTechniqueShaderType::PIXEL)
            {
                maybeShader = m_shader_loader.LoadPixelShader(name);
            }

            if (!maybeShader)
                return result::Unexpected<std::string>("Failed to load shader");

            m_bin = *maybeShader;

            return NoResult{};
        }

        void LeaveShader() override
        {
            m_bin = {};
        }

        result::Expected<NoResult, std::string> AcceptShaderConstantArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                             techset::CommonCodeConstSource codeConstSource) override
        {
            return result::Unexpected<std::string>("Not implemented");
        }

        result::Expected<NoResult, std::string> AcceptShaderSamplerArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                            techset::CommonCodeSamplerSource codeSamplerSource) override
        {
            return result::Unexpected<std::string>("Not implemented");
        }

        result::Expected<NoResult, std::string> AcceptShaderLiteralArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                            const std::array<float, 4>& literalValue) override
        {
            return result::Unexpected<std::string>("Not implemented");
        }

        result::Expected<NoResult, std::string> AcceptShaderMaterialArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                             unsigned nameHash) override
        {
            return result::Unexpected<std::string>("Not implemented");
        }

        result::Expected<NoResult, std::string> AcceptShaderMaterialArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                             const std::string& nameValue) override
        {
            return result::Unexpected<std::string>("Not implemented");
        }

        result::Expected<std::vector<techset::CommonShaderArg>, std::string> FinalizeArgs() override
        {
            // TODO: Auto create remaining args

            std::ranges::sort(m_args,
                              [this](const techset::CommonShaderArg& arg0, const techset::CommonShaderArg& arg1) -> bool
                              {
                                  const auto updateFreq0 = arg0.GetFrequency(m_common_code_source_infos);
                                  const auto updateFreq1 = arg1.GetFrequency(m_common_code_source_infos);
                                  if (updateFreq0 != updateFreq1)
                                      return updateFreq0 < updateFreq1;

                                  const auto typeNumeric0 = m_common_code_source_infos.GetArgumentTypeNumericValue(arg0.m_type);
                                  const auto typeNumeric1 = m_common_code_source_infos.GetArgumentTypeNumericValue(arg1.m_type);
                                  assert(typeNumeric0);
                                  assert(typeNumeric1);
                                  if (*typeNumeric0 != *typeNumeric1)
                                      return *typeNumeric0 < *typeNumeric1;

                                  if (arg0.m_type.m_value_type == techset::CommonShaderValueType::MATERIAL_CONST
                                      || arg0.m_type.m_value_type == techset::CommonShaderValueType::MATERIAL_SAMPLER)
                                  {
                                      return arg0.m_value.name_hash < arg1.m_value.name_hash;
                                  }

                                  return CompareArgumentDestinations(arg0.m_destination, arg1.m_destination);
                              });

            return m_args;
        }

    protected:
        [[nodiscard]] virtual size_t CompareArgumentDestinations(const techset::CommonShaderArgDestination& dest0,
                                                                 const techset::CommonShaderArgDestination& dest1) const = 0;

        techset::ITechniqueShaderLoader& m_shader_loader;
        techset::CommonCodeSourceInfos& m_common_code_source_infos;

        techset::CommonTechniqueShaderType m_shader_type;
        techset::CommonTechniqueShaderBin m_bin;

        std::vector<techset::CommonShaderArg> m_args;
    };

    class CommonShaderArgCreatorDx11 final : public BaseCommonShaderArgCreator
    {
    public:
        explicit CommonShaderArgCreatorDx11(techset::ITechniqueShaderLoader& shaderLoader, techset::CommonCodeSourceInfos& commonCodeSourceInfos)
            : BaseCommonShaderArgCreator(shaderLoader, commonCodeSourceInfos)
        {
        }

    protected:
        [[nodiscard]] size_t CompareArgumentDestinations(const techset::CommonShaderArgDestination& dest0,
                                                         const techset::CommonShaderArgDestination& dest1) const override
        {
            if (dest0.dx11.m_buffer != dest1.dx11.m_buffer)
                return dest0.dx11.m_buffer < dest1.dx11.m_buffer;

            return dest0.dx11.m_location < dest1.dx11.m_location;
        }
    };
} // namespace

namespace techset
{
    CommonShaderArgCreatorDestination::CommonShaderArgCreatorDestination(std::string argumentName)
        : m_argument_name(std::move(argumentName))
    {
    }

    CommonShaderArgCreatorDestination::CommonShaderArgCreatorDestination(std::string argumentName, const unsigned argumentIndex)
        : m_argument_name(std::move(argumentName)),
          m_argument_index(argumentIndex)
    {
    }

    std::unique_ptr<CommonShaderArgCreator> CommonShaderArgCreator::CreateDx9(ITechniqueShaderLoader& shaderLoader, AssetCreationContext& context)
    {
        // TODO
        return nullptr;
    }

    std::unique_ptr<CommonShaderArgCreator>
        CommonShaderArgCreator::CreateDx11(ITechniqueShaderLoader& shaderLoader, AssetCreationContext& context, CommonCodeSourceInfos& commonCodeSourceInfos)
    {
        return std::make_unique<CommonShaderArgCreatorDx11>(shaderLoader, commonCodeSourceInfos);
    }
} // namespace techset

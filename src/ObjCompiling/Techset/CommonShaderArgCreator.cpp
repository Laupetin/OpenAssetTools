#include "CommonShaderArgCreator.h"

#include "Shader/D3D11ShaderAnalyser.h"
#include "Shader/D3D9ShaderAnalyser.h"
#include "Utils/Djb2.h"

#include <algorithm>
#include <cassert>
#include <format>

namespace
{
    class BaseCommonShaderArgCreator : public techset::CommonShaderArgCreator
    {
    public:
        explicit BaseCommonShaderArgCreator(techset::ITechniqueShaderLoader& shaderLoader, techset::CommonCodeSourceInfos& commonCodeSourceInfos)
            : m_shader_loader(shaderLoader),
              m_common_code_source_infos(commonCodeSourceInfos),
              m_shader_type(techset::CommonTechniqueShaderType::VERTEX),
              m_bin{},
              m_tech_flags(0),
              m_sampler_flags(0)
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

        result::Expected<NoResult, std::string> LeaveShader() override
        {
            m_bin = {};

            return AutoCreateMissingArgs();
        }

        result::Expected<NoResult, std::string> AcceptShaderConstantArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                             const techset::CommonCodeConstSource codeConstSource,
                                                                             const unsigned sourceIndex) override
        {
            techset::CommonShaderArgDestination commonDestination{};
            bool isTransposed;
            std::string errorMessage;
            if (!FindDestinationForConstant(commonDestination, isTransposed, errorMessage, destination))
            {
                if (!errorMessage.empty())
                    return result::Unexpected(std::move(errorMessage));

                return result::Unexpected(std::format("Could not find constant shader input with name {}", destination.m_argument_name));
            }

            return AcceptShaderConstantArgument(commonDestination, isTransposed, codeConstSource, sourceIndex);
        }

        result::Expected<NoResult, std::string> AcceptShaderSamplerArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                            const techset::CommonCodeSamplerSource codeSamplerSource) override
        {
            techset::CommonShaderArgDestination commonDestination{};
            std::string errorMessage;
            if (!FindDestinationForSampler(commonDestination, errorMessage, destination))
            {
                if (!errorMessage.empty())
                    return result::Unexpected(std::move(errorMessage));

                return result::Unexpected(std::format("Could not find sampler shader input with name {}", destination.m_argument_name));
            }

            return AcceptShaderSamplerArgument(commonDestination, codeSamplerSource);
        }

        result::Expected<NoResult, std::string> AcceptShaderLiteralArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                            const std::array<float, 4>& literalValue) override
        {
            techset::CommonShaderArgDestination commonDestination{};
            bool isTransposed;
            std::string errorMessage;
            if (!FindDestinationForConstant(commonDestination, isTransposed, errorMessage, destination))
            {
                if (!errorMessage.empty())
                    return result::Unexpected(std::move(errorMessage));

                return result::Unexpected(std::format("Could not find constant shader input with name {}", destination.m_argument_name));
            }

            techset::CommonShaderArgumentType argumentType{
                .m_shader_type = m_shader_type,
                .m_value_type = techset::CommonShaderValueType::LITERAL_CONST,
            };

            techset::CommonShaderArgValue value{.literal_value = literalValue};

            m_args.emplace_back(argumentType, commonDestination, value);
            return NoResult{};
        }

        result::Expected<NoResult, std::string> AcceptShaderMaterialArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                             const unsigned nameHash) override
        {
            techset::CommonShaderArgumentType argumentType{
                .m_shader_type = m_shader_type,
                .m_value_type = techset::CommonShaderValueType::MATERIAL_CONST,
            };

            techset::CommonShaderArgDestination commonDestination{};
            bool isTransposed;
            std::string errorMessage;
            if (!FindDestinationForConstant(commonDestination, isTransposed, errorMessage, destination))
            {
                if (!errorMessage.empty())
                    return result::Unexpected(std::move(errorMessage));

                argumentType.m_value_type = techset::CommonShaderValueType::MATERIAL_SAMPLER;

                if (!FindDestinationForSampler(commonDestination, errorMessage, destination))
                {
                    if (!errorMessage.empty())
                        return result::Unexpected(std::move(errorMessage));

                    return result::Unexpected(std::format("Could not find shader input with name {}", destination.m_argument_name));
                }
            }

            techset::CommonShaderArgValue value{.name_hash = nameHash};

            m_args.emplace_back(argumentType, commonDestination, value);
            return NoResult{};
        }

        result::Expected<NoResult, std::string> AcceptShaderMaterialArgument(const techset::CommonShaderArgCreatorDestination& destination,
                                                                             const std::string& nameValue) override
        {
            // All game's call R_HashString here which has the same implementation in all games
            return AcceptShaderMaterialArgument(destination, djb2_xor_nocase(nameValue.c_str(), 0));
        }

        result::Expected<NoResult, std::string> FinalizePass(techset::CommonTechnique& technique, techset::CommonPass& pass) override
        {
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

                                  return CompareArgumentDestinations(arg0, arg1);
                              });

            technique.m_flags |= m_tech_flags;
            pass.m_args = std::move(m_args);
            pass.m_sampler_flags |= m_sampler_flags;

            m_tech_flags = 0;
            m_args = std::vector<techset::CommonShaderArg>();
            m_sampler_flags = 0;

            return NoResult{};
        }

    protected:
        result::Expected<NoResult, std::string> AcceptShaderConstantArgument(const techset::CommonShaderArgDestination& commonDestination,
                                                                             const bool isTransposed,
                                                                             const techset::CommonCodeConstSource codeConstSource,
                                                                             const unsigned sourceIndex)
        {
            techset::CommonShaderArgumentType argumentType{
                .m_shader_type = m_shader_type,
                .m_value_type = techset::CommonShaderValueType::CODE_CONST,
            };

            const auto maybeInfo = m_common_code_source_infos.GetInfoForCodeConstSource(codeConstSource);
            if (!maybeInfo)
                return result::Unexpected<std::string>("Could not find info for code constant");

            const auto isMatrix = maybeInfo->transposedMatrix.has_value();
            techset::CommonShaderArgCodeConstValue value{
                .m_index = 0,
                .m_first_row = 0,
                .m_row_count = isMatrix ? 4u : 1u,
            };

            if (isMatrix)
            {
                if (sourceIndex >= 4)
                    return result::Unexpected(std::format("Index for matrix code const is out of bounds: {} (must be < 4)", sourceIndex));

                value.m_index = isTransposed ? *maybeInfo->transposedMatrix : codeConstSource;
                value.m_first_row = sourceIndex;
            }
            else
            {
                const auto arrayCount = std::max<unsigned>(maybeInfo->arrayCount, 1u);
                if (sourceIndex >= arrayCount)
                    return result::Unexpected(std::format("Index for code const is out of bounds: {} (must be < {})", sourceIndex, arrayCount));

                value.m_index = codeConstSource + static_cast<techset::CommonCodeConstSource>(sourceIndex);
                value.m_first_row = 0;
            }

            m_args.emplace_back(argumentType, commonDestination, techset::CommonShaderArgValue{.code_const_source = value});
            if (maybeInfo->techFlags)
                m_tech_flags |= *maybeInfo->techFlags;

            return NoResult{};
        }

        result::Expected<NoResult, std::string> AcceptShaderSamplerArgument(const techset::CommonShaderArgDestination& commonDestination,
                                                                            const techset::CommonCodeSamplerSource codeSamplerSource)
        {
            techset::CommonShaderArgumentType argumentType{
                .m_shader_type = m_shader_type,
                .m_value_type = techset::CommonShaderValueType::CODE_SAMPLER,
            };

            const auto maybeInfo = m_common_code_source_infos.GetInfoForCodeSamplerSource(codeSamplerSource);
            if (!maybeInfo)
                return result::Unexpected<std::string>("Could not find info for code sampler");

            m_args.emplace_back(argumentType, commonDestination, techset::CommonShaderArgValue{.code_sampler_source = codeSamplerSource});
            if (maybeInfo->techFlags)
                m_tech_flags |= *maybeInfo->techFlags;
            if (maybeInfo->customSamplerIndex)
                m_sampler_flags |= (1 << *maybeInfo->customSamplerIndex);

            return NoResult{};
        }

        [[nodiscard]] virtual size_t CompareArgumentDestinations(const techset::CommonShaderArg& arg0, const techset::CommonShaderArg& arg1) const = 0;

        [[nodiscard]] virtual bool FindDestinationForConstant(techset::CommonShaderArgDestination& commonDestination,
                                                              bool& isTransposed,
                                                              std::string& errorMessage,
                                                              const techset::CommonShaderArgCreatorDestination& input) = 0;
        [[nodiscard]] virtual bool FindDestinationForSampler(techset::CommonShaderArgDestination& commonDestination,
                                                             std::string& errorMessage,
                                                             const techset::CommonShaderArgCreatorDestination& input) = 0;

        virtual result::Expected<NoResult, std::string> AutoCreateMissingArgs() = 0;

        techset::ITechniqueShaderLoader& m_shader_loader;
        techset::CommonCodeSourceInfos& m_common_code_source_infos;

        techset::CommonTechniqueShaderType m_shader_type;
        techset::CommonTechniqueShaderBin m_bin;

        std::vector<techset::CommonShaderArg> m_args;
        uint64_t m_tech_flags;
        unsigned m_sampler_flags;
    };

    class CommonShaderArgCreatorDx9 final : public BaseCommonShaderArgCreator
    {
    public:
        explicit CommonShaderArgCreatorDx9(techset::ITechniqueShaderLoader& shaderLoader, techset::CommonCodeSourceInfos& commonCodeSourceInfos)
            : BaseCommonShaderArgCreator(shaderLoader, commonCodeSourceInfos)
        {
        }

        result::Expected<NoResult, std::string> EnterShader(const techset::CommonTechniqueShaderType shaderType, const std::string& name) override
        {
            auto result = BaseCommonShaderArgCreator::EnterShader(shaderType, name);
            if (!result)
                return std::move(result);

            m_shader_info = d3d9::ShaderAnalyser::GetShaderInfo(m_bin.m_shader_bin, m_bin.m_shader_bin_size);
            if (!m_shader_info)
                return result::Unexpected(std::format("Failed to analyse dx9 shader {}", name));

            return NoResult{};
        }

        result::Expected<NoResult, std::string> LeaveShader() override
        {
            auto result = BaseCommonShaderArgCreator::LeaveShader();
            m_shader_info = nullptr;

            return std::move(result);
        }

    protected:
        [[nodiscard]] size_t CompareArgumentDestinations(const techset::CommonShaderArg& arg0, const techset::CommonShaderArg& arg1) const override
        {
            return arg0.m_destination.dx9.m_destination_register < arg1.m_destination.dx9.m_destination_register;
        }

        [[nodiscard]] bool FindDestinationForConstant(techset::CommonShaderArgDestination& commonDestination,
                                                      bool& isTransposed,
                                                      std::string& errorMessage,
                                                      const techset::CommonShaderArgCreatorDestination& input) override
        {
            assert(m_shader_info);
            // TODO
            return false;
        }

        [[nodiscard]] bool FindDestinationForSampler(techset::CommonShaderArgDestination& commonDestination,
                                                     std::string& errorMessage,
                                                     const techset::CommonShaderArgCreatorDestination& input) override
        {
            assert(m_shader_info);
            // TODO
            return false;
        }

        result::Expected<NoResult, std::string> AutoCreateMissingArgs() override
        {
            // TODO
            return NoResult{};
        }

    private:
        std::unique_ptr<d3d9::ShaderInfo> m_shader_info;
    };

    class CommonShaderArgCreatorDx11 final : public BaseCommonShaderArgCreator
    {
    public:
        explicit CommonShaderArgCreatorDx11(techset::ITechniqueShaderLoader& shaderLoader, techset::CommonCodeSourceInfos& commonCodeSourceInfos)
            : BaseCommonShaderArgCreator(shaderLoader, commonCodeSourceInfos)
        {
        }

        result::Expected<NoResult, std::string> EnterShader(const techset::CommonTechniqueShaderType shaderType, const std::string& name) override
        {
            auto result = BaseCommonShaderArgCreator::EnterShader(shaderType, name);
            if (!result)
                return std::move(result);

            m_shader_info = d3d11::ShaderAnalyser::GetShaderInfo(m_bin.m_shader_bin, m_bin.m_shader_bin_size);
            if (!m_shader_info)
                return result::Unexpected(std::format("Failed to analyse dx11 shader {}", name));

            CountShaderArgs();

            return NoResult{};
        }

        result::Expected<NoResult, std::string> LeaveShader() override
        {
            auto result = BaseCommonShaderArgCreator::LeaveShader();
            m_shader_info = nullptr;

            return std::move(result);
        }

    protected:
        [[nodiscard]] size_t CompareArgumentDestinations(const techset::CommonShaderArg& arg0, const techset::CommonShaderArg& arg1) const override
        {
            if (arg0.m_destination.dx11.m_buffer != arg1.m_destination.dx11.m_buffer)
                return arg0.m_destination.dx11.m_buffer < arg1.m_destination.dx11.m_buffer;

            return LocationComparisonValue(arg0) < LocationComparisonValue(arg1);
        }

        [[nodiscard]] static size_t LocationComparisonValue(const techset::CommonShaderArg& arg)
        {
            switch (arg.m_type.m_value_type)
            {
            case techset::CommonShaderValueType::LITERAL_CONST:
            case techset::CommonShaderValueType::MATERIAL_CONST:
            case techset::CommonShaderValueType::CODE_CONST:
                return arg.m_destination.dx11.m_location.constant_buffer_offset;
            case techset::CommonShaderValueType::MATERIAL_SAMPLER:
            case techset::CommonShaderValueType::CODE_SAMPLER:
                return arg.m_destination.dx11.m_location.sampler_index;
            default:
                assert(false);
                return 0;
            }
        }

        [[nodiscard]] bool FindDestinationForConstant(techset::CommonShaderArgDestination& commonDestination,
                                                      bool& isTransposed,
                                                      std::string& errorMessage,
                                                      const techset::CommonShaderArgCreatorDestination& input) override
        {
            assert(m_shader_info);

            auto usedConstantIndex = 0uz;
            for (const auto& buffer : m_shader_info->m_constant_buffers)
            {
                const auto bufferBinding =
                    std::ranges::find_if(m_shader_info->m_bound_resources,
                                         [buffer](const d3d11::BoundResource& boundResource)
                                         {
                                             return boundResource.m_type == d3d11::BoundResourceType::CBUFFER && boundResource.m_name == buffer.m_name;
                                         });
                if (bufferBinding == m_shader_info->m_bound_resources.end())
                {
                    errorMessage = std::format("Failed to find binding for constant buffer {}", buffer.m_name);
                    return false;
                }

                auto variableIterator = buffer.m_variables.begin();
                const auto variableEnd = buffer.m_variables.end();
                for (; variableIterator != variableEnd; ++variableIterator)
                {
                    if (variableIterator->m_name == input.m_argument_name)
                        break;

                    if (variableIterator->m_is_used)
                        usedConstantIndex++;
                }

                if (variableIterator != variableEnd)
                {
                    const auto variableElementCount = std::max<unsigned>(variableIterator->m_element_count, 1);
                    const auto inputArgumentIndex = input.m_argument_index.value_or(0);
                    if (inputArgumentIndex >= variableElementCount)
                    {
                        errorMessage = std::format("Shader variable {} only has {} elements", variableIterator->m_name, variableElementCount);
                        return false;
                    }

                    const auto variableElementSize = variableIterator->m_size / variableElementCount;
                    commonDestination.dx11.m_location.constant_buffer_offset = variableIterator->m_offset + variableElementSize * inputArgumentIndex;
                    commonDestination.dx11.m_size = variableElementSize;
                    commonDestination.dx11.m_buffer = bufferBinding->m_bind_point;
                    isTransposed = variableIterator->m_variable_class == d3d11::VariableClass::MATRIX_COLUMNS;

                    m_const_arg_added[usedConstantIndex] = true;

                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] bool FindDestinationForSampler(techset::CommonShaderArgDestination& commonDestination,
                                                     std::string& errorMessage,
                                                     const techset::CommonShaderArgCreatorDestination& input) override
        {
            assert(m_shader_info);

            // Find texture and its index
            size_t textureIndex = 0;
            auto maybeTextureResource = m_shader_info->m_bound_resources.begin();
            const auto resourceEnd = m_shader_info->m_bound_resources.end();
            for (; maybeTextureResource != resourceEnd; ++maybeTextureResource)
            {
                if (maybeTextureResource->m_type != d3d11::BoundResourceType::TEXTURE)
                    continue;

                if (maybeTextureResource->m_name == input.m_argument_name)
                    break;

                textureIndex++;
            }

            // Find sampler
            const auto maybeSamplerResource =
                std::ranges::find_if(m_shader_info->m_bound_resources,
                                     [input](const d3d11::BoundResource& boundResource)
                                     {
                                         return boundResource.m_type == d3d11::BoundResourceType::SAMPLER && boundResource.m_name == input.m_argument_name;
                                     });

            const auto hasTextureResource = maybeTextureResource != resourceEnd;
            const auto hasSamplerResource = maybeSamplerResource != resourceEnd;

            if (!hasSamplerResource && !hasTextureResource)
                return false;

            if (hasSamplerResource && !hasTextureResource)
            {
                errorMessage = std::format("Shader binds a sampler for argument {} but not a texture", input.m_argument_name);
                return false;
            }
            assert(hasTextureResource);

            commonDestination.dx11.m_location.texture_index = maybeTextureResource->m_bind_point;

            // The game seems to be fine with not being able to find the sampler
            // and just takes the same index as the texture
            if (hasSamplerResource)
                commonDestination.dx11.m_location.sampler_index = maybeSamplerResource->m_bind_point;
            else
                commonDestination.dx11.m_location.sampler_index = maybeTextureResource->m_bind_point;

            commonDestination.dx11.m_size = maybeTextureResource->m_bind_count;
            commonDestination.dx11.m_buffer = 0;

            m_texture_arg_added[textureIndex] = true;

            return true;
        }

        result::Expected<NoResult, std::string> AutoCreateMissingArgs() override
        {
            size_t usedConstantCount = 0;
            size_t textureCount = 0;

            for (const auto& buffer : m_shader_info->m_constant_buffers)
            {
                const auto bufferBinding =
                    std::ranges::find_if(m_shader_info->m_bound_resources,
                                         [buffer](const d3d11::BoundResource& boundResource)
                                         {
                                             return boundResource.m_type == d3d11::BoundResourceType::CBUFFER && boundResource.m_name == buffer.m_name;
                                         });
                if (bufferBinding == m_shader_info->m_bound_resources.end())
                    return result::Unexpected(std::format("Failed to find binding for constant buffer {}", buffer.m_name));

                for (const auto& variable : buffer.m_variables)
                {
                    if (!variable.m_is_used)
                        continue;

                    if (m_const_arg_added[usedConstantCount++])
                        continue;

                    auto result = AutoCreateConstantArg(variable, bufferBinding->m_bind_point);
                    if (!result)
                        return std::move(result);
                }
            }

            for (const auto& maybeTextureResource : m_shader_info->m_bound_resources)
            {
                if (maybeTextureResource.m_type != d3d11::BoundResourceType::TEXTURE)
                    continue;

                if (m_texture_arg_added[textureCount++])
                    continue;

                const auto maybeSamplerResource = std::ranges::find_if(m_shader_info->m_bound_resources,
                                                                       [maybeTextureResource](const d3d11::BoundResource& boundResource)
                                                                       {
                                                                           return boundResource.m_type == d3d11::BoundResourceType::SAMPLER
                                                                                  && boundResource.m_name == maybeTextureResource.m_name;
                                                                       });

                auto result = AutoCreateSamplerArg(maybeTextureResource,
                                                   maybeSamplerResource != m_shader_info->m_bound_resources.end() ? maybeSamplerResource->m_bind_point
                                                                                                                  : maybeTextureResource.m_bind_point);
                if (!result)
                    return std::move(result);
            }

            // TODO
            return NoResult{};
        }

    private:
        void CountShaderArgs()
        {
            size_t usedConstantCount = 0;
            size_t textureCount = 0;

            for (const auto& buffer : m_shader_info->m_constant_buffers)
            {
                for (const auto& variable : buffer.m_variables)
                {
                    if (variable.m_is_used)
                        usedConstantCount++;
                }
            }

            for (const auto& resource : m_shader_info->m_bound_resources)
            {
                if (resource.m_type == d3d11::BoundResourceType::TEXTURE)
                    textureCount++;
            }

            m_const_arg_added = std::vector(usedConstantCount, false);
            m_texture_arg_added = std::vector(textureCount, false);
        }

        result::Expected<NoResult, std::string> AutoCreateConstantArg(const d3d11::ConstantBufferVariable& variable, const size_t bufferIndex)
        {
            const auto maybeCodeConst = m_common_code_source_infos.GetCodeConstSourceForAccessor(variable.m_name);
            if (!maybeCodeConst)
            {
                // Some variables are simply not added as args for some reason
                if (m_common_code_source_infos.IsArgAccessorIgnored(variable.m_name))
                    return NoResult{};

                return result::Unexpected(std::format("Missing assignment to shader constant {}", variable.m_name));
            }

            const auto constInfo = m_common_code_source_infos.GetInfoForCodeConstSource(*maybeCodeConst);
            if (!constInfo)
                return result::Unexpected(std::format("Missing info for code const {}", variable.m_name));

            const auto variableElementCount = std::max<unsigned>(variable.m_element_count, 1);
            const auto variableElementSize = variable.m_size / variableElementCount;
            const auto infoArrayCount = std::max<unsigned>(constInfo->arrayCount, 1);
            if (variableElementCount > infoArrayCount)
            {
                return result::Unexpected(std::format("Could not auto create argument for constant {} as it has more elements ({}) than the code constant ({})",
                                                      variable.m_name,
                                                      variableElementCount,
                                                      infoArrayCount));
            }

            techset::CommonShaderArgDestination commonDestination;
            commonDestination.dx11.m_size = variableElementSize;
            commonDestination.dx11.m_buffer = static_cast<unsigned>(bufferIndex);
            const auto isTransposed = variable.m_variable_class == d3d11::VariableClass::MATRIX_COLUMNS;
            for (auto elementIndex = 0u; elementIndex < variableElementCount; elementIndex++)
            {
                commonDestination.dx11.m_location.constant_buffer_offset = variable.m_offset + variableElementSize * elementIndex;
                auto result = AcceptShaderConstantArgument(commonDestination, isTransposed, *maybeCodeConst, elementIndex);
                if (!result)
                    return std::move(result);
            }

            if (constInfo->techFlags)
                m_tech_flags |= *constInfo->techFlags;

            return NoResult{};
        }

        result::Expected<NoResult, std::string> AutoCreateSamplerArg(const d3d11::BoundResource& textureResource, const unsigned samplerBindPoint)
        {
            const auto maybeCodeSampler = m_common_code_source_infos.GetCodeSamplerSourceForAccessor(textureResource.m_name);
            if (!maybeCodeSampler)
                return result::Unexpected(std::format("Missing assignment to shader texture {}", textureResource.m_name));

            const auto samplerInfo = m_common_code_source_infos.GetInfoForCodeSamplerSource(*maybeCodeSampler);
            if (!samplerInfo)
                return result::Unexpected(std::format("Missing info for code sampler {}", textureResource.m_name));

            techset::CommonShaderArgDestination commonDestination;
            commonDestination.dx11.m_location.texture_index = textureResource.m_bind_point;
            commonDestination.dx11.m_location.sampler_index = samplerBindPoint;
            commonDestination.dx11.m_size = textureResource.m_bind_count;
            commonDestination.dx11.m_buffer = 0;

            if (samplerInfo->techFlags)
                m_tech_flags |= *samplerInfo->techFlags;
            if (samplerInfo->customSamplerIndex)
                m_sampler_flags |= (1 << *samplerInfo->customSamplerIndex);

            return AcceptShaderSamplerArgument(commonDestination, *maybeCodeSampler);
        }

        std::unique_ptr<d3d11::ShaderInfo> m_shader_info;
        std::vector<bool> m_const_arg_added;
        std::vector<bool> m_texture_arg_added;
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

    std::unique_ptr<CommonShaderArgCreator>
        CommonShaderArgCreator::CreateDx9(ITechniqueShaderLoader& shaderLoader, AssetCreationContext& context, CommonCodeSourceInfos& commonCodeSourceInfos)
    {
        return std::make_unique<CommonShaderArgCreatorDx9>(shaderLoader, commonCodeSourceInfos);
    }

    std::unique_ptr<CommonShaderArgCreator>
        CommonShaderArgCreator::CreateDx11(ITechniqueShaderLoader& shaderLoader, AssetCreationContext& context, CommonCodeSourceInfos& commonCodeSourceInfos)
    {
        return std::make_unique<CommonShaderArgCreatorDx11>(shaderLoader, commonCodeSourceInfos);
    }
} // namespace techset

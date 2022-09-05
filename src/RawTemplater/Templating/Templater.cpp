#include "Templater.h"

#include <unordered_map>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Utils/ClassUtils.h"
#include "DirectiveEscapeStreamProxy.h"
#include "TemplatingStreamProxy.h"
#include "Parsing/ParsingException.h"
#include "Parsing/Impl/DefinesStreamProxy.h"
#include "Parsing/Impl/ParserSingleInputStream.h"
#include "Parsing/Impl/SkipUntilFirstNonEmptyProxy.h"

using namespace templating;
namespace fs = std::filesystem;

namespace templating
{
    class TemplatingPass
    {
    public:
        TemplatingPass()
            : m_stream(nullptr)
        {
        }

        TemplatingPass(std::istream& stream, const std::string& fileName, ITemplaterControl* templaterControl)
        {
            m_base_stream = std::make_unique<ParserSingleInputStream>(stream, fileName);

            m_templating_proxy = std::make_unique<TemplatingStreamProxy>(m_base_stream.get(), templaterControl);
            m_defines_proxy = std::make_unique<DefinesStreamProxy>(m_templating_proxy.get());
            m_directive_escape_proxy = std::make_unique<DirectiveEscapeStreamProxy>(m_defines_proxy.get());
            m_skip_until_first_non_empty_proxy = std::make_unique<SkipUntilFirstNonEmptyProxy>(m_directive_escape_proxy.get());

            m_templating_proxy->SetDefinesProxy(m_defines_proxy.get());
            m_stream = m_skip_until_first_non_empty_proxy.get();
        }

        std::unique_ptr<IParserLineStream> m_base_stream;
        std::unique_ptr<TemplatingStreamProxy> m_templating_proxy;
        std::unique_ptr<DefinesStreamProxy> m_defines_proxy;
        std::unique_ptr<DirectiveEscapeStreamProxy> m_directive_escape_proxy;
        std::unique_ptr<SkipUntilFirstNonEmptyProxy> m_skip_until_first_non_empty_proxy;
        IParserLineStream* m_stream;
    };

    enum class TemplatingVariationType
    {
        OPTIONS,
        SWITCH
    };

    class ITemplatingVariation
    {
    protected:
        ITemplatingVariation() = default;

    public:
        virtual ~ITemplatingVariation() = default;
        ITemplatingVariation(const ITemplatingVariation& other) = default;
        ITemplatingVariation(ITemplatingVariation&& other) noexcept = default;
        ITemplatingVariation& operator=(const ITemplatingVariation& other) = default;
        ITemplatingVariation& operator=(ITemplatingVariation&& other) noexcept = default;

        virtual const std::string& GetName() = 0;
        virtual void Advance() = 0;
        virtual void Apply(DefinesStreamProxy* definesProxy) = 0;
        _NODISCARD virtual bool IsFinished() const = 0;
        _NODISCARD virtual TemplatingVariationType GetVariationType() const = 0;
    };

    class SwitchVariation final : public ITemplatingVariation
    {
    public:
        explicit SwitchVariation(std::string name)
            : m_name(std::move(name)),
              m_should_define(true),
              m_finished(false)
        {
        }

        const std::string& GetName() override
        {
            return m_name;
        }

        void Advance() override
        {
            if (!m_should_define)
                m_finished = true;
            else
                m_should_define = false;
        }

        void Apply(DefinesStreamProxy* definesProxy) override
        {
            if (m_should_define)
                definesProxy->AddDefine(DefinesStreamProxy::Define(m_name, "1"));
        }

        _NODISCARD bool IsFinished() const override
        {
            return m_finished;
        }

        _NODISCARD TemplatingVariationType GetVariationType() const override
        {
            return TemplatingVariationType::SWITCH;
        }

        std::string m_name;
        bool m_should_define;
        bool m_finished;
    };

    class OptionsVariation final : public ITemplatingVariation
    {
    public:
        OptionsVariation(std::string name, std::vector<std::string> values)
            : m_name(std::move(name)),
              m_values(std::move(values)),
              m_value_offset(0u)
        {
        }

        const std::string& GetName() override
        {
            return m_name;
        }

        void Advance() override
        {
            m_value_offset++;
        }

        void Apply(DefinesStreamProxy* definesProxy) override
        {
            if (m_value_offset < m_values.size())
                definesProxy->AddDefine(DefinesStreamProxy::Define(m_name, m_values[m_value_offset]));
        }

        _NODISCARD bool IsFinished() const override
        {
            return m_value_offset >= m_values.size();
        }

        _NODISCARD TemplatingVariationType GetVariationType() const override
        {
            return TemplatingVariationType::OPTIONS;
        }

        std::string m_name;
        std::vector<std::string> m_values;
        size_t m_value_offset;
    };

    class TemplaterControlImpl final : ITemplaterControl
    {
    public:
        TemplaterControlImpl(std::istream& stream, std::string filename, const std::string& outputDirectory)
            : m_stream(stream),
              m_filename(std::move(filename)),
              m_output_directory(outputDirectory),
              m_first_line(true),
              m_write_output_to_file(false)
        {
            fs::path filenamePath(m_filename);
            m_default_output_file = (m_output_directory / filenamePath.replace_extension()).string();
        }

        bool RunNextPass(std::ostream* buildLogFile)
        {
            m_stream.clear();
            m_stream.seekg(0, std::ios::beg);

            m_output_file = m_default_output_file;
            m_current_pass = TemplatingPass(m_stream, m_filename, this);

            for (const auto& activeVariation : m_active_variations)
                activeVariation->Apply(m_current_pass.m_defines_proxy.get());

            while (!m_current_pass.m_stream->Eof())
            {
                auto nextLine = m_current_pass.m_stream->NextLine();

                if (m_write_output_to_file)
                {
                    if (m_first_line)
                        m_first_line = false;
                    else
                        m_output_stream << '\n';

                    m_output_stream << nextLine.m_line;
                }
                else
                {
                    if (m_first_line)
                        m_first_line = false;
                    else
                        m_output_cache << '\n';

                    m_output_cache << nextLine.m_line;
                }
            }

            if (!m_write_output_to_file)
            {
                m_output_stream = std::ofstream(m_output_file, std::ios::out | std::ios::binary);
                if (!m_output_stream.is_open())
                {
                    std::cerr << "Failed to open output file \"" << m_output_file << "\"\n";
                    return false;
                }

                const auto cachedData = m_output_cache.str();
                if (!cachedData.empty())
                    m_output_stream << cachedData;
            }

            std::cout << "Templated file \"" << m_output_file << "\"\n";

            if(buildLogFile)
                *buildLogFile << "Templated file \"" << m_output_file << "\"\n";

            m_first_line = true;
            m_write_output_to_file = false;
            m_output_cache.clear();
            m_output_cache.str(std::string());
            m_output_stream.close();

            return true;
        }

        void AdvanceActiveVariations()
        {
            while (!m_active_variations.empty())
            {
                const auto& lastVariation = m_active_variations[m_active_variations.size() - 1];
                lastVariation->Advance();

                if (lastVariation->IsFinished())
                {
                    m_active_variations_by_name.erase(lastVariation->GetName());
                    m_active_variations.pop_back();
                }
                else
                    break;
            }
        }

        _NODISCARD bool HasActiveVariations() const
        {
            return !m_active_variations.empty();
        }

    protected:
        bool AddSwitch(std::string switchName) override
        {
            const auto existingVariation = m_active_variations_by_name.find(switchName);
            if (existingVariation != m_active_variations_by_name.end())
                return existingVariation->second->GetVariationType() == TemplatingVariationType::SWITCH;

            auto switchVariation = std::make_unique<SwitchVariation>(std::move(switchName));
            if (m_current_pass.m_defines_proxy)
                switchVariation->Apply(m_current_pass.m_defines_proxy.get());
            m_active_variations_by_name.emplace(switchVariation->m_name, switchVariation.get());
            m_active_variations.emplace_back(std::move(switchVariation));
            return true;
        }

        bool AddOptions(std::string optionsName, std::vector<std::string> optionValues) override
        {
            const auto existingVariation = m_active_variations_by_name.find(optionsName);
            if (existingVariation != m_active_variations_by_name.end())
                return existingVariation->second->GetVariationType() == TemplatingVariationType::SWITCH;

            auto optionsVariation = std::make_unique<OptionsVariation>(std::move(optionsName), std::move(optionValues));
            if (m_current_pass.m_defines_proxy)
                optionsVariation->Apply(m_current_pass.m_defines_proxy.get());
            m_active_variations_by_name.emplace(optionsVariation->m_name, optionsVariation.get());
            m_active_variations.emplace_back(std::move(optionsVariation));
            return true;
        }

        bool SetFileName(const std::string& fileName) override
        {
            if (m_write_output_to_file)
                return false;

            m_output_file = (m_output_directory / fileName).string();
            m_output_stream = std::ofstream(m_output_file, std::ios::out | std::ios::binary);
            if (!m_output_stream.is_open())
            {
                std::cerr << "Failed to open output file \"" << m_output_file << "\"\n";
                return false;
            }

            m_write_output_to_file = true;
            const auto cachedData = m_output_cache.str();
            if (!cachedData.empty())
                m_output_stream << cachedData;
            m_output_cache.clear();
            m_output_cache.str(std::string());

            return true;
        }

    private:
        std::vector<std::unique_ptr<ITemplatingVariation>> m_active_variations;
        std::unordered_map<std::string, ITemplatingVariation*> m_active_variations_by_name;
        TemplatingPass m_current_pass;

        std::istream& m_stream;
        std::string m_filename;
        std::string m_output_file;
        std::string m_default_output_file;
        const fs::path m_output_directory;

        bool m_first_line;
        bool m_write_output_to_file;
        std::ofstream m_output_stream;
        std::ostringstream m_output_cache;
    };
}

Templater::Templater(std::istream& stream, std::string fileName)
    : m_stream(stream),
      m_build_log(nullptr),
      m_file_name(std::move(fileName))
{
}

void Templater::SetBuildLogFile(std::ostream* buildLogFile)
{
    m_build_log = buildLogFile;
}

bool Templater::TemplateToDirectory(const std::string& outputDirectory) const
{
    TemplaterControlImpl control(m_stream, m_file_name, outputDirectory);

    try
    {
        if (!control.RunNextPass(m_build_log))
            return false;

        control.AdvanceActiveVariations();
        while (control.HasActiveVariations())
        {
            if (!control.RunNextPass(m_build_log))
                return false;

            control.AdvanceActiveVariations();
        }
    }
    catch (ParsingException& e)
    {
        std::cerr << "Error: " << e.FullMessage() << std::endl;

        return false;
    }

    return true;
}

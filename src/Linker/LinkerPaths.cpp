#include "LinkerPaths.h"

#include "SearchPath/IWD.h"
#include "SearchPath/SearchPathFilesystem.h"
#include "SearchPath/SearchPaths.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <unordered_set>

namespace fs = std::filesystem;

namespace
{
    enum class PathTemplateParameterType : std::uint8_t
    {
        BIN = 1 << 0,
        BASE = 1 << 1,
        PROJECT = 1 << 2,
        GAME = 1 << 3,
    };

    class LinkerPathTemplate
    {
        static constexpr auto PATTERN_BIN = "?bin?";
        static constexpr auto PATTERN_BASE = "?base?";
        static constexpr auto PATTERN_GAME = "?game?";
        static constexpr auto PATTERN_PROJECT = "?project?";

        struct Pattern
        {
            const char* m_str;
            PathTemplateParameterType m_type;
        };

        static constexpr Pattern PATTERNS[]{
            {PATTERN_BIN,     PathTemplateParameterType::BIN    },
            {PATTERN_BASE,    PathTemplateParameterType::BASE   },
            {PATTERN_GAME,    PathTemplateParameterType::GAME   },
            {PATTERN_PROJECT, PathTemplateParameterType::PROJECT},
        };

    public:
        LinkerPathTemplate()
            : m_parameter_type_flags(0u)
        {
        }

        void CreateFromString(const std::string& templateString)
        {
            const auto templateStringLength = templateString.size();
            auto partStart = 0u;
            for (auto i = 0u; i < templateStringLength; i++)
            {
                if (templateString[i] != '?')
                    continue;

                for (const auto& pattern : PATTERNS)
                {
                    const auto patternLength = std::strlen(pattern.m_str);
                    if (templateString.compare(i, patternLength, pattern.m_str) == 0)
                    {
                        m_parts.emplace_back(templateString.substr(partStart, i - partStart));
                        m_parameters.emplace_back(pattern.m_type);
                        m_parameter_type_flags |= static_cast<std::underlying_type_t<PathTemplateParameterType>>(pattern.m_type);
                        i += patternLength;

                        partStart = i;
                        break;
                    }
                }
            }

            if (partStart < templateStringLength)
                m_parts.emplace_back(templateString.substr(partStart, templateStringLength - partStart));
        }

        [[nodiscard]] std::string
            Render(const std::string& binDir, const std::string& baseDir, const std::string& projectName, const std::string& gameName) const
        {
            if (m_parts.empty())
                return "";

            if (m_parameters.empty())
                return m_parts[0];

            std::ostringstream ss;
            ss << m_parts[0];

            const auto partsCount = m_parts.size();
            const auto parameterCount = m_parameters.size();
            for (auto parameterIndex = 0u; parameterIndex < parameterCount; parameterIndex++)
            {
                switch (m_parameters[parameterIndex])
                {
                case PathTemplateParameterType::BIN:
                    ss << binDir;
                    break;
                case PathTemplateParameterType::BASE:
                    ss << baseDir;
                    break;
                case PathTemplateParameterType::PROJECT:
                    ss << projectName;
                    break;
                case PathTemplateParameterType::GAME:
                    ss << gameName;
                    break;
                default:
                    assert(false);
                    break;
                }

                if (parameterIndex + 1 < partsCount)
                    ss << m_parts[parameterIndex + 1];
            }

            return fs::path(ss.str()).make_preferred().string();
        }

        [[nodiscard]] bool CanRender(const std::underlying_type_t<PathTemplateParameterType> availableParameters) const
        {
            return (m_parameter_type_flags & ~availableParameters) == 0;
        }

    private:
        std::vector<std::string> m_parts;
        std::vector<PathTemplateParameterType> m_parameters;
        std::underlying_type_t<PathTemplateParameterType> m_parameter_type_flags;
    };

    class LinkerSearchPathBuilder final : public ILinkerSearchPathBuilder
    {
        static constexpr auto INDEPENDENT_MASK = static_cast<unsigned>(PathTemplateParameterType::BIN) | static_cast<unsigned>(PathTemplateParameterType::BASE);
        static constexpr auto PROJECT_MASK = static_cast<unsigned>(PathTemplateParameterType::BIN) | static_cast<unsigned>(PathTemplateParameterType::BASE)
                                             | static_cast<unsigned>(PathTemplateParameterType::PROJECT);
        static constexpr auto GAME_MASK = static_cast<unsigned>(PathTemplateParameterType::BIN) | static_cast<unsigned>(PathTemplateParameterType::BASE)
                                          | static_cast<unsigned>(PathTemplateParameterType::PROJECT) | static_cast<unsigned>(PathTemplateParameterType::GAME);

    public:
        LinkerSearchPathBuilder(const char* typeName, std::string binDir, std::string baseDir)
            : m_type_name(typeName),
              m_bin_dir(std::move(binDir)),
              m_base_dir(std::move(baseDir))
        {
        }

        void BuildFromArgs(const std::set<std::string>& templates)
        {
            m_templates.reserve(templates.size());
            for (const auto& templateString : templates)
            {
                LinkerPathTemplate templateStruct;
                templateStruct.CreateFromString(templateString);
                m_templates.emplace_back(std::move(templateStruct));
            }
        }

        [[nodiscard]] std::unique_ptr<ISearchPath> BuildIndependentSearchPaths() const override
        {
            SearchPaths searchPaths;
            std::unordered_set<std::string> addedSearchPaths;
            auto hasSearchPath = false;

            for (const auto& curTemplate : m_templates)
            {
                if (curTemplate.CanRender(INDEPENDENT_MASK))
                {
                    auto renderedTemplate = curTemplate.Render(m_bin_dir, m_base_dir, std::string(), std::string());
                    if (AddSearchPath(addedSearchPaths, searchPaths, renderedTemplate))
                        hasSearchPath = true;
                }
            }

            if (hasSearchPath)
                return std::make_unique<SearchPaths>(std::move(searchPaths));

            return {};
        }

        [[nodiscard]] std::unique_ptr<ISearchPath> BuildSearchPathsSpecificToProject(const std::string& projectName) const override
        {
            SearchPaths searchPaths;
            std::unordered_set<std::string> addedSearchPaths;
            auto hasSearchPath = false;

            for (const auto& curTemplate : m_templates)
            {
                if (!curTemplate.CanRender(INDEPENDENT_MASK) && curTemplate.CanRender(PROJECT_MASK))
                {
                    auto renderedTemplate = curTemplate.Render(m_bin_dir, m_base_dir, projectName, std::string());
                    if (AddSearchPath(addedSearchPaths, searchPaths, renderedTemplate))
                        hasSearchPath = true;
                }
            }

            if (hasSearchPath)
                return std::make_unique<SearchPaths>(std::move(searchPaths));

            return {};
        }

        [[nodiscard]] std::unique_ptr<ISearchPath> BuildSearchPathsSpecificToProjectAndGame(const std::string& projectName, GameId game) const override
        {
            SearchPaths searchPaths;
            std::unordered_set<std::string> addedSearchPaths;
            auto hasSearchPath = false;

            for (const auto& curTemplate : m_templates)
            {
                if (!curTemplate.CanRender(PROJECT_MASK) && curTemplate.CanRender(GAME_MASK))
                {
                    std::string gameName(GameId_Names[static_cast<unsigned>(game)]);
                    utils::MakeStringLowerCase(gameName);

                    auto renderedTemplate = curTemplate.Render(m_bin_dir, m_base_dir, projectName, gameName);
                    if (AddSearchPath(addedSearchPaths, searchPaths, renderedTemplate))
                        hasSearchPath = true;
                }
            }

            if (hasSearchPath)
                return std::make_unique<SearchPaths>(std::move(searchPaths));

            return {};
        }

    private:
        bool AddSearchPath(std::unordered_set<std::string>& existingSearchPaths, SearchPaths& searchPaths, const std::string& path) const
        {
            const auto existingSearchPath = existingSearchPaths.find(path);
            if (existingSearchPath != existingSearchPaths.end())
                return false;
            existingSearchPaths.emplace(path);

            if (!fs::is_directory(path))
            {
                std::cout << std::format("Adding {} search path (Not found): {}\n", m_type_name, path);
                return false;
            }

            std::cout << std::format("Adding {} search path: {}\n", m_type_name, path);
            searchPaths.CommitSearchPath(std::make_unique<SearchPathFilesystem>(path));
            return true;
        }

        const char* m_type_name;
        std::vector<LinkerPathTemplate> m_templates;
        std::string m_bin_dir;
        std::string m_base_dir;
    };

    class LinkerPaths final : public ILinkerPaths
    {
    public:
        LinkerPaths(std::string binDir,
                    std::string baseDir,
                    LinkerSearchPathBuilder assetSearchPaths,
                    LinkerSearchPathBuilder gdtSearchPaths,
                    LinkerSearchPathBuilder sourceSearchPaths,
                    LinkerPathTemplate cacheTemplate,
                    LinkerPathTemplate outTemplate)
            : m_bin_dir(std::move(binDir)),
              m_base_dir(std::move(baseDir)),
              m_asset_search_paths(std::move(assetSearchPaths)),
              m_gdt_search_paths(std::move(gdtSearchPaths)),
              m_source_search_paths(std::move(sourceSearchPaths)),
              m_cache_template(std::move(cacheTemplate)),
              m_out_template(std::move(outTemplate))
        {
        }

        [[nodiscard]] const ILinkerSearchPathBuilder& AssetSearchPaths() const override
        {
            return m_asset_search_paths;
        }

        [[nodiscard]] const ILinkerSearchPathBuilder& GdtSearchPaths() const override
        {
            return m_gdt_search_paths;
        }

        [[nodiscard]] const ILinkerSearchPathBuilder& SourceSearchPaths() const override
        {
            return m_source_search_paths;
        }

        [[nodiscard]] std::string BuildCacheFolderPath(const std::string& projectName, GameId game) const override
        {
            return m_cache_template.Render(m_bin_dir, m_base_dir, projectName, GameId_Names[static_cast<unsigned>(game)]);
        }

        [[nodiscard]] std::string BuildOutputFolderPath(const std::string& projectName, GameId game) const override
        {
            return m_out_template.Render(m_bin_dir, m_base_dir, projectName, GameId_Names[static_cast<unsigned>(game)]);
        }

    private:
        std::string m_bin_dir;
        std::string m_base_dir;
        LinkerSearchPathBuilder m_asset_search_paths;
        LinkerSearchPathBuilder m_gdt_search_paths;
        LinkerSearchPathBuilder m_source_search_paths;
        LinkerPathTemplate m_cache_template;
        LinkerPathTemplate m_out_template;
    };
} // namespace

std::unique_ptr<ILinkerPaths> ILinkerPaths::FromArgs(const LinkerArgs& args)
{
    std::string normalizedBinPath = fs::weakly_canonical(args.m_bin_folder).make_preferred().string();
    std::string normalizedBasePath = fs::weakly_canonical(args.m_base_folder).make_preferred().string();

    LinkerSearchPathBuilder assetSearchPaths("asset", normalizedBinPath, normalizedBasePath);
    assetSearchPaths.BuildFromArgs(args.m_asset_search_paths);

    LinkerSearchPathBuilder gdtSearchPaths("gdt", normalizedBinPath, normalizedBasePath);
    gdtSearchPaths.BuildFromArgs(args.m_gdt_search_paths);

    LinkerSearchPathBuilder sourceSearchPaths("source", normalizedBinPath, normalizedBasePath);
    sourceSearchPaths.BuildFromArgs(args.m_source_search_paths);

    LinkerPathTemplate cacheTemplate;
    cacheTemplate.CreateFromString(LinkerArgs::DEFAULT_CACHE_FOLDER);

    LinkerPathTemplate outTemplate;
    outTemplate.CreateFromString(args.m_out_folder);

    return std::make_unique<LinkerPaths>(std::move(normalizedBinPath),
                                         std::move(normalizedBasePath),
                                         std::move(assetSearchPaths),
                                         std::move(gdtSearchPaths),
                                         std::move(sourceSearchPaths),
                                         std::move(cacheTemplate),
                                         std::move(outTemplate));
}

#pragma once

#include "Asset/AssetCreationContext.h"
#include "Asset/AssetRegistration.h"
#include "Asset/IAssetCreator.h"
#include "PhysPreset/PhysPresetCommon.h"
#include "PhysPreset/PhysPresetLexer.h"
#include "PhysPreset/PhysPresetParser.h"
#include "SearchPath/ISearchPath.h"
#include "Utils/Logging/Log.h"
#include "Utils/MemoryManager.h"

#include <antlr4-runtime.h>
#include <string>

namespace phys_preset
{
    class PhysPresetErrorListener final : public antlr4::BaseErrorListener
    {
    public:
        explicit PhysPresetErrorListener(const std::string& fileName)
            : m_file_name(fileName),
              m_has_errors(false)
        {
        }

        void syntaxError(antlr4::Recognizer*, antlr4::Token*, size_t line, size_t charPos, const std::string& msg, std::exception_ptr) override
        {
            con::error("{}:{}:{}: {}", m_file_name, line, charPos, msg);
            m_has_errors = true;
        }

        bool HasErrors() const
        {
            return m_has_errors;
        }

    private:
        std::string m_file_name;
        bool m_has_errors;
    };

    template<typename AssetT>
    class AbstractPhysPresetLoader : public AssetCreator<AssetT>
    {
    public:
        AbstractPhysPresetLoader(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) final
        {
            const auto fileName = phys_preset::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            antlr4::ANTLRInputStream input(*file.m_stream);
            PhysPresetLexer lexer(&input);
            antlr4::CommonTokenStream tokens(&lexer);
            PhysPresetParser parser(&tokens);

            PhysPresetErrorListener errorListener(fileName);
            lexer.removeErrorListeners();
            lexer.addErrorListener(&errorListener);
            parser.removeErrorListeners();
            parser.addErrorListener(&errorListener);

            auto* tree = parser.physPreset();

            if (errorListener.HasErrors())
            {
                con::error("Failed to parse phys preset: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            auto* physPreset = m_memory.Alloc<typename AssetT::Type>();
            AssetRegistration<AssetT> registration(assetName, physPreset);

            if (!FillFromTree(*physPreset, assetName, tree))
            {
                con::error("Failed to convert phys preset: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    protected:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;

        virtual bool FillFromTree(typename AssetT::Type& physPreset,
                                   const std::string& assetName,
                                   PhysPresetParser::PhysPresetContext* tree) = 0;
    };
} // namespace phys_preset

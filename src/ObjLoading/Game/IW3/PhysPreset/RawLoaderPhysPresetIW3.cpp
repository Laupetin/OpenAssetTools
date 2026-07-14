#include "RawLoaderPhysPresetIW3.h"

#include "Game/IW3/IW3.h"
#include "PhysPreset/PhysPresetCommon.h"
#include "PhysPresetLexer.h"
#include "PhysPresetParser.h"
#include "Utils/Logging/Log.h"

#include <antlr4-runtime.h>
#include <format>
#include <limits>
#include <string>

using namespace IW3;

namespace
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

    bool FillPhysPreset(PhysPreset& physPreset,
                        const std::string& assetName,
                        PhysPresetParser::PhysPresetContext* tree,
                        MemoryManager& memory)
    {
        physPreset.name = memory.Dup(assetName.c_str());

        float friction = 0.0f;
        bool isFrictionInfinity = false;

        for (auto* pair : tree->pair())
        {
            const auto key = pair->key()->getText();
            const auto value = pair->value()->getText();

            if (key == "mass")
                physPreset.mass = std::stof(value);
            else if (key == "bounce")
                physPreset.bounce = std::stof(value);
            else if (key == "friction")
                friction = std::stof(value);
            else if (key == "isFrictionInfinity")
                isFrictionInfinity = value != "0";
            else if (key == "bulletForceScale")
                physPreset.bulletForceScale = std::stof(value);
            else if (key == "explosiveForceScale")
                physPreset.explosiveForceScale = std::stof(value);
            else if (key == "sndAliasPrefix")
                physPreset.sndAliasPrefix = memory.Dup(value.c_str());
            else if (key == "piecesSpreadFraction")
                physPreset.piecesSpreadFraction = std::stof(value);
            else if (key == "piecesUpwardVelocity")
                physPreset.piecesUpwardVelocity = std::stof(value);
            else if (key == "tempDefaultToCylinder")
                physPreset.tempDefaultToCylinder = value != "0";
        }

        physPreset.friction = isFrictionInfinity ? std::numeric_limits<float>::max() : friction;

        return true;
    }

    class RawLoaderPhysPreset final : public AssetCreator<AssetPhysPreset>
    {
    public:
        RawLoaderPhysPreset(MemoryManager& memory, ISearchPath& searchPath)
            : m_memory(memory),
              m_search_path(searchPath)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
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

            auto* physPreset = m_memory.Alloc<PhysPreset>();
            AssetRegistration<AssetPhysPreset> registration(assetName, physPreset);

            if (!FillPhysPreset(*physPreset, assetName, tree, m_memory))
            {
                con::error("Failed to convert phys preset: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        MemoryManager& m_memory;
        ISearchPath& m_search_path;
    };
} // namespace

namespace phys_preset
{
    std::unique_ptr<AssetCreator<AssetPhysPreset>> CreateRawLoaderIW3(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderPhysPreset>(memory, searchPath);
    }
} // namespace phys_preset

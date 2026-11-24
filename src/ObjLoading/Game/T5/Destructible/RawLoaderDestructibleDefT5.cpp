#include "RawLoaderDestructibleDefT5.h"

#include "Destructible/DestructibleCommon.h"
#include "Game/T5/Destructible/DestructiblePieceFields.h"
#include "Game/T5/InfoString/InfoStringToStructConverter.h"
#include "Game/T5/ObjConstantsT5.h"
#include "InfoString/InfoString.h"
#include "Utils/Logging/Log.h"
#include "Utils/StringUtils.h"

#include <cassert>
#include <cstdlib>
#include <format>
#include <type_traits>

using namespace T5;

namespace
{
    constexpr auto MAX_DESTRUCTIBLE_PIECES = 32;
    constexpr auto MAX_DESTRUCTIBLE_STAGES = std::extent_v<decltype(DestructiblePiece::stages)>;
    constexpr auto HIDE_BONES_PER_WORD = 32u;

    class InfoStringToDestructiblePieceConverter final : public InfoStringToStructConverter
    {
    protected:
        bool ConvertExtensionField(const cspField_t& field, const std::string& value) override
        {
            switch (static_cast<destructibleFieldType_t>(field.iFieldType))
            {
            case DFT_BREAK_HEALTH:
                FieldValue<float>(field) = static_cast<float>(std::atof(value.c_str())) / 100.0f;
                return true;

            case DFT_EXPLOSIVE_DAMAGE_ONLY:
                return ConvertFlag(field, value, DSF_EXPLOSIVE_DAMAGE_ONLY);

            case DFT_MELEE_DAMAGE:
                return ConvertFlag(field, value, DSF_MELEE_DAMAGE);

            case DFT_PARENT_DAMAGE:
                return ConvertFlag(field, value, DSF_PARENT_DAMAGE);

            case DFT_PHYSICS_PARENT:
                return ConvertFlag(field, value, DSF_PHYSICS_PARENT);

            default:
                assert(false);
                return false;
            }
        }

    public:
        InfoStringToDestructiblePieceConverter(const InfoString& infoString,
                                               DestructiblePiece& piece,
                                               ZoneScriptStrings& zoneScriptStrings,
                                               MemoryManager& memory,
                                               AssetCreationContext& context,
                                               AssetRegistration<AssetDestructibleDef>& registration)
            : InfoStringToStructConverter(infoString,
                                          &piece,
                                          zoneScriptStrings,
                                          memory,
                                          context,
                                          registration,
                                          destructible_piece_fields,
                                          std::extent_v<decltype(destructible_piece_fields)>)
        {
        }

    private:
        template<typename T> T& FieldValue(const cspField_t& field)
        {
            return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
        }

        bool ConvertFlag(const cspField_t& field, const std::string& value, const unsigned flag)
        {
            if (std::atoi(value.c_str()) != 0)
                FieldValue<unsigned>(field) |= flag;

            return true;
        }
    };

    class RawLoaderDestructibleDef final : public AssetCreator<AssetDestructibleDef>
    {
    public:
        RawLoaderDestructibleDef(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
            : m_memory(memory),
              m_search_path(searchPath),
              m_zone(zone)
        {
        }

        AssetCreationResult CreateAsset(const std::string& assetName, AssetCreationContext& context) override
        {
            const auto fileName = destructible::GetFileNameForAssetName(assetName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
                return AssetCreationResult::NoAction();

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_DESTRUCTIBLE_DEF, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return AssetCreationResult::Failure();
            }

            auto* def = m_memory.Alloc<DestructibleDef>();
            def->name = m_memory.Dup(assetName.c_str());

            AssetRegistration<AssetDestructibleDef> registration(assetName, def);
            if (!LoadDef(*def, infoString, context, registration))
            {
                con::error("Failed to load destructible def: \"{}\"", assetName);
                return AssetCreationResult::Failure();
            }

            return AssetCreationResult::Success(context.AddAsset(std::move(registration)));
        }

    private:
        static int CountPieces(const InfoString& infoString)
        {
            auto pieceCount = 0;
            for (auto i = 0; i < MAX_DESTRUCTIBLE_PIECES; i++)
            {
                if (infoString.HasKey(std::format("piece{}", i)))
                    pieceCount = i + 1;
            }

            return pieceCount;
        }

        static bool GetValue(const InfoString& infoString, const std::string& key, std::string& value)
        {
            auto found = false;
            value = infoString.GetValueForKey(key, &found);

            return found;
        }

        template<typename AssetType>
        bool LoadAssetDependency(const std::string& value,
                                 typename AssetType::Type*& target,
                                 AssetCreationContext& context,
                                 AssetRegistration<AssetDestructibleDef>& registration)
        {
            if (value.empty())
            {
                target = nullptr;
                return true;
            }

            auto* dependency = context.LoadDependency<AssetType>(value);
            if (dependency == nullptr)
            {
                con::error("Failed to load dependency \"{}\"", value);
                return false;
            }

            registration.AddDependency(dependency);
            target = dependency->Asset();
            return true;
        }

        static const char* NullIfEmpty(const char* value)
        {
            if (value != nullptr && value[0] == '\0')
                return nullptr;

            return value;
        }

        const char* LoadString(const std::string& value) const
        {
            if (value.empty())
                return nullptr;

            return m_memory.Dup(value.c_str());
        }

        bool LoadPiece(DestructiblePiece& piece,
                       const std::string& pieceName,
                       AssetCreationContext& context,
                       AssetRegistration<AssetDestructibleDef>& registration)
        {
            if (pieceName.empty())
                return true;

            const auto fileName = destructible::GetPieceFileNameForPieceName(pieceName);
            const auto file = m_search_path.Open(fileName);
            if (!file.IsOpen())
            {
                con::error("Could not find destructible piece file: \"{}\"", fileName);
                return false;
            }

            InfoString infoString;
            if (!infoString.FromStream(INFO_STRING_PREFIX_DESTRUCTIBLE_PIECE, *file.m_stream))
            {
                con::error("Could not parse as info string file: \"{}\"", fileName);
                return false;
            }

            InfoStringToDestructiblePieceConverter converter(infoString, piece, m_zone.m_script_strings, m_memory, context, registration);
            if (!converter.Convert())
                return false;

            for (auto& stage : piece.stages)
            {
                stage.breakSound = NullIfEmpty(stage.breakSound);
                stage.breakNotify = NullIfEmpty(stage.breakNotify);
                stage.loopSound = NullIfEmpty(stage.loopSound);
            }

            return true;
        }

        bool LoadPieceProperties(DestructiblePiece& piece,
                                 const int pieceIndex,
                                 const InfoString& infoString,
                                 AssetCreationContext& context,
                                 AssetRegistration<AssetDestructibleDef>& registration)
        {
            std::string value;

            if (GetValue(infoString, std::format("piece{}Health", pieceIndex), value))
                piece.health = std::atoi(value.c_str());

            if (GetValue(infoString, std::format("parentPiece{}", pieceIndex), value))
                piece.parentPiece = static_cast<char>(std::atoi(value.c_str()));

            if (GetValue(infoString, std::format("parentDamage{}", pieceIndex), value))
                piece.parentDamagePercent = static_cast<float>(static_cast<float>(std::atoi(value.c_str())) / 100.0);

            if (GetValue(infoString, std::format("bulletDamageScale{}", pieceIndex), value))
                piece.bulletDamageScale = static_cast<float>(std::atof(value.c_str()));

            if (GetValue(infoString, std::format("explosiveDamageScale{}", pieceIndex), value))
                piece.explosiveDamageScale = static_cast<float>(std::atof(value.c_str()));

            if (GetValue(infoString, std::format("meleeDamageScale{}", pieceIndex), value))
                piece.meleeDamageScale = static_cast<float>(std::atof(value.c_str()));

            if (GetValue(infoString, std::format("impactDamageScale{}", pieceIndex), value))
                piece.impactDamageScale = static_cast<float>(std::atof(value.c_str()));

            if (GetValue(infoString, std::format("physConstraint{}", pieceIndex), value)
                && !LoadAssetDependency<AssetPhysConstraints>(value, piece.physConstraints, context, registration))
                return false;

            if (GetValue(infoString, std::format("damageSound{}", pieceIndex), value))
                piece.damageSound = LoadString(value);

            if (GetValue(infoString, std::format("burnEffect{}", pieceIndex), value)
                && !LoadAssetDependency<AssetFx>(value, piece.burnEffect, context, registration))
                return false;

            if (GetValue(infoString, std::format("burnSound{}", pieceIndex), value))
                piece.burnSound = LoadString(value);

            if (GetValue(infoString, std::format("damageEntity{}", pieceIndex), value))
                piece.entityDamageTransfer = static_cast<float>(std::atoi(value.c_str())) * 0.0099999998f;

            if (GetValue(infoString, std::format("pieceLabel{}", pieceIndex), value))
            {
                utils::MakeStringLowerCase(value);
                piece.enableLabel = m_zone.m_script_strings.AddOrGetScriptString(value);
                registration.AddScriptString(piece.enableLabel);
            }

            return true;
        }

        void CalculateHideBones(const DestructibleDef& def, const XAssetInfo<XModel>& modelInfo) const
        {
            const auto& model = *modelInfo.Asset();
            const auto& modelScriptStrings = modelInfo.m_zone->m_script_strings;

            for (auto pieceIndex = 0; pieceIndex < def.numPieces; pieceIndex++)
            {
                auto& piece = def.pieces[pieceIndex];
                for (auto stageIndex = 0u; stageIndex < MAX_DESTRUCTIBLE_STAGES; stageIndex++)
                {
                    const auto& showBone = m_zone.m_script_strings[piece.stages[stageIndex].showBone];
                    if (showBone.empty())
                        continue;

                    auto foundBone = false;
                    for (auto boneIndex = 0u; boneIndex < model.numBones; boneIndex++)
                    {
                        if (model.boneNames[boneIndex] >= modelScriptStrings.Count() || modelScriptStrings[model.boneNames[boneIndex]] != showBone)
                            continue;

                        piece.hideBones[boneIndex / HIDE_BONES_PER_WORD] |= static_cast<int>(0x80000000u >> (boneIndex % HIDE_BONES_PER_WORD));
                        foundBone = true;
                        break;
                    }

                    if (!foundBone)
                        con::warn("Destructible \"{}\" can not find bone \"{}\" in model \"{}\" for piece {} stage {}",
                                  def.name,
                                  showBone,
                                  model.name,
                                  pieceIndex,
                                  stageIndex);
                }
            }
        }

        bool LoadDef(DestructibleDef& def, const InfoString& infoString, AssetCreationContext& context, AssetRegistration<AssetDestructibleDef>& registration)
        {
            std::string value;

            if (GetValue(infoString, "clientOnly", value))
                def.clientOnly = std::atoi(value.c_str()) != 0 ? 1 : 0;

            XAssetInfo<XModel>* modelInfo = nullptr;
            if (GetValue(infoString, "model", value) && !value.empty())
            {
                modelInfo = context.LoadDependency<AssetXModel>(value);
                if (modelInfo == nullptr)
                {
                    con::error("Failed to load xmodel asset \"{}\"", value);
                    return false;
                }

                registration.AddDependency(modelInfo);
                def.model = modelInfo->Asset();
            }

            if (GetValue(infoString, "pristineModel", value) && !LoadAssetDependency<AssetXModel>(value, def.pristineModel, context, registration))
                return false;

            def.numPieces = CountPieces(infoString);
            if (def.numPieces == 0)
                return true;

            def.pieces = m_memory.Alloc<DestructiblePiece>(static_cast<size_t>(def.numPieces));
            for (auto i = 0; i < def.numPieces; i++)
            {
                GetValue(infoString, std::format("piece{}", i), value);
                if (!LoadPiece(def.pieces[i], value, context, registration))
                    return false;

                if (!LoadPieceProperties(def.pieces[i], i, infoString, context, registration))
                    return false;
            }

            if (modelInfo != nullptr)
                CalculateHideBones(def, *modelInfo);

            return true;
        }

        MemoryManager& m_memory;
        ISearchPath& m_search_path;
        Zone& m_zone;
    };
} // namespace

namespace destructible
{
    std::unique_ptr<AssetCreator<AssetDestructibleDef>> CreateRawLoaderT5(MemoryManager& memory, ISearchPath& searchPath, Zone& zone)
    {
        return std::make_unique<RawLoaderDestructibleDef>(memory, searchPath, zone);
    }
} // namespace destructible

#include "DestructibleDefDumperT5.h"

#include "Destructible/DestructibleCommon.h"
#include "Game/T5/Destructible/DestructiblePieceFields.h"
#include "Game/T5/InfoString/InfoStringFromStructConverter.h"
#include "Game/T5/ObjConstantsT5.h"

#include <cassert>
#include <cmath>
#include <format>
#include <sstream>
#include <type_traits>

using namespace T5;

namespace
{
    constexpr auto MAX_DESTRUCTIBLE_PIECES = 32;

    std::string FloatToString(const float value)
    {
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }

    std::string PercentToString(const float value)
    {
        return std::to_string(std::lround(value * 100.0f));
    }

    std::string AssetNameOrEmpty(const char* name)
    {
        if (name == nullptr)
            return "";

        if (name[0] == ',')
            return &name[1];

        return name;
    }

    class InfoStringFromDestructiblePieceConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<destructibleFieldType_t>(field.iFieldType))
            {
            case DFT_BREAK_HEALTH:
                m_info_string.SetValueForKey(field.szName, FloatToString(FieldValue<float>(field) * 100.0f));
                break;

            case DFT_EXPLOSIVE_DAMAGE_ONLY:
                FillFromFlag(field, DSF_EXPLOSIVE_DAMAGE_ONLY);
                break;

            case DFT_MELEE_DAMAGE:
                FillFromFlag(field, DSF_MELEE_DAMAGE);
                break;

            case DFT_PARENT_DAMAGE:
                FillFromFlag(field, DSF_PARENT_DAMAGE);
                break;

            case DFT_PHYSICS_PARENT:
                FillFromFlag(field, DSF_PHYSICS_PARENT);
                break;

            default:
                assert(false);
                break;
            }
        }

    public:
        InfoStringFromDestructiblePieceConverter(const DestructiblePiece* structure, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(
                  structure, destructible_piece_fields, std::extent_v<decltype(destructible_piece_fields)>, std::move(scriptStringValueCallback))
        {
        }

    private:
        template<typename T> [[nodiscard]] const T& FieldValue(const cspField_t& field) const
        {
            return *reinterpret_cast<const T*>(reinterpret_cast<uintptr_t>(m_structure) + field.iOffset);
        }

        void FillFromFlag(const cspField_t& field, const unsigned flag)
        {
            m_info_string.SetValueForKey(field.szName, (FieldValue<unsigned>(field) & flag) != 0 ? "1" : "0");
        }
    };

    std::function<std::string(scr_string_t)> ScriptStringCallback(const XAssetInfo<DestructibleDef>& asset)
    {
        return [&asset](const scr_string_t scrStr) -> std::string
        {
            assert(scrStr < asset.m_zone->m_script_strings.Count());
            if (scrStr >= asset.m_zone->m_script_strings.Count())
                return "";

            return asset.m_zone->m_script_strings[scrStr];
        };
    }

    InfoString CreatePieceInfoString(const XAssetInfo<DestructibleDef>& asset, const DestructiblePiece& piece)
    {
        InfoStringFromDestructiblePieceConverter converter(&piece, ScriptStringCallback(asset));
        return converter.Convert();
    }

    InfoString CreateDefInfoString(const XAssetInfo<DestructibleDef>& asset)
    {
        const auto& def = *asset.Asset();
        const auto scriptString = ScriptStringCallback(asset);

        InfoString infoString;
        infoString.SetValueForKey("clientOnly", def.clientOnly != 0 ? "1" : "0");
        infoString.SetValueForKey("model", AssetNameOrEmpty(def.model ? def.model->name : nullptr));
        infoString.SetValueForKey("pristineModel", AssetNameOrEmpty(def.pristineModel ? def.pristineModel->name : nullptr));

        for (auto i = 0; i < def.numPieces; i++)
        {
            const auto& piece = def.pieces[i];

            infoString.SetValueForKey(std::format("piece{}", i), destructible::GetPieceNameForPieceIndex(asset.m_name, i));
            infoString.SetValueForKey(std::format("piece{}Health", i), std::to_string(piece.health));
            infoString.SetValueForKey(std::format("parentPiece{}", i), std::to_string(static_cast<unsigned char>(piece.parentPiece)));
            infoString.SetValueForKey(std::format("parentDamage{}", i), PercentToString(piece.parentDamagePercent));
            infoString.SetValueForKey(std::format("bulletDamageScale{}", i), FloatToString(piece.bulletDamageScale));
            infoString.SetValueForKey(std::format("explosiveDamageScale{}", i), FloatToString(piece.explosiveDamageScale));
            infoString.SetValueForKey(std::format("meleeDamageScale{}", i), FloatToString(piece.meleeDamageScale));
            infoString.SetValueForKey(std::format("impactDamageScale{}", i), FloatToString(piece.impactDamageScale));
            infoString.SetValueForKey(std::format("physConstraint{}", i), AssetNameOrEmpty(piece.physConstraints ? piece.physConstraints->name : nullptr));
            infoString.SetValueForKey(std::format("damageSound{}", i), AssetNameOrEmpty(piece.damageSound));
            infoString.SetValueForKey(std::format("burnEffect{}", i), AssetNameOrEmpty(piece.burnEffect ? piece.burnEffect->name : nullptr));
            infoString.SetValueForKey(std::format("burnSound{}", i), AssetNameOrEmpty(piece.burnSound));
            infoString.SetValueForKey(std::format("damageEntity{}", i), PercentToString(piece.entityDamageTransfer));
            infoString.SetValueForKey(std::format("pieceLabel{}", i), scriptString(piece.enableLabel));
        }

        return infoString;
    }

    void WriteInfoString(AssetDumpingContext& context, const std::string& fileName, const std::string& prefix, const InfoString& infoString)
    {
        const auto assetFile = context.OpenAssetFile(fileName);
        if (!assetFile)
            return;

        const auto stringValue = infoString.ToString(prefix);
        assetFile->write(stringValue.c_str(), static_cast<std::streamsize>(stringValue.size()));
    }
} // namespace

namespace destructible
{
    void DumperT5::DumpAsset(AssetDumpingContext& context, const XAssetInfo<AssetDestructibleDef::Type>& asset)
    {
        const auto& def = *asset.Asset();
        assert(def.numPieces >= 0 && def.numPieces <= MAX_DESTRUCTIBLE_PIECES);

        WriteInfoString(context, GetFileNameForAssetName(asset.m_name), INFO_STRING_PREFIX_DESTRUCTIBLE_DEF, CreateDefInfoString(asset));

        for (auto i = 0; i < def.numPieces; i++)
        {
            WriteInfoString(context,
                            GetPieceFileNameForPieceName(GetPieceNameForPieceIndex(asset.m_name, i)),
                            INFO_STRING_PREFIX_DESTRUCTIBLE_PIECE,
                            CreatePieceInfoString(asset, def.pieces[i]));
        }
    }
} // namespace destructible

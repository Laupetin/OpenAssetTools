#include "AssetDumperWeaponAttachment.h"

#include <cassert>
#include <sstream>
#include <type_traits>
#include <cstring>

#include "Game/T6/CommonT6.h"
#include "Game/T6/InfoString/InfoStringFromStructConverter.h"

using namespace T6;

cspField_t AssetDumperWeaponAttachment::attachment_fields[]
{
    {"displayName", offsetof(WeaponAttachment, szDisplayName), CSPFT_STRING},
    {"attachmentType", offsetof(WeaponAttachment, attachmentType), AFT_ATTACHMENTTYPE},
    {"penetrateType", offsetof(WeaponAttachment, penetrateType), AFT_PENETRATE_TYPE},
    {"firstRaisePriority", offsetof(WeaponAttachment, firstRaisePriority), CSPFT_INT},
    {"hipIdleAmount", offsetof(WeaponAttachment, fHipIdleAmount), CSPFT_FLOAT},
    {"fireType", offsetof(WeaponAttachment, fireType), AFT_FIRETYPE},
    {"damageRangeScale", offsetof(WeaponAttachment, fDamageRangeScale), CSPFT_FLOAT},
    {"adsZoomFov1", offsetof(WeaponAttachment, fAdsZoomFov1), CSPFT_FLOAT},
    {"adsZoomFov2", offsetof(WeaponAttachment, fAdsZoomFov2), CSPFT_FLOAT},
    {"adsZoomFov3", offsetof(WeaponAttachment, fAdsZoomFov3), CSPFT_FLOAT},
    {"adsZoomInFrac", offsetof(WeaponAttachment, fAdsZoomInFrac), CSPFT_FLOAT},
    {"adsZoomOutFrac", offsetof(WeaponAttachment, fAdsZoomOutFrac), CSPFT_FLOAT},
    {"adsTransInTimeScale", offsetof(WeaponAttachment, fAdsTransInTimeScale), CSPFT_FLOAT},
    {"adsTransOutTimeScale", offsetof(WeaponAttachment, fAdsTransOutTimeScale), CSPFT_FLOAT},
    {"adsRecoilReductionRate", offsetof(WeaponAttachment, fAdsRecoilReductionRate), CSPFT_FLOAT},
    {"adsRecoilReductionLimit", offsetof(WeaponAttachment, fAdsRecoilReductionLimit), CSPFT_FLOAT},
    {"adsViewKickCenterSpeedScale", offsetof(WeaponAttachment, fAdsViewKickCenterSpeedScale), CSPFT_FLOAT},
    {"adsIdleAmountScale", offsetof(WeaponAttachment, fAdsIdleAmountScale), CSPFT_FLOAT},
    {"swayOverride", offsetof(WeaponAttachment, swayOverride), CSPFT_BOOL},
    {"swayMaxAngle", offsetof(WeaponAttachment, swayMaxAngle), CSPFT_FLOAT},
    {"swayLerpSpeed", offsetof(WeaponAttachment, swayLerpSpeed), CSPFT_FLOAT},
    {"swayPitchScale", offsetof(WeaponAttachment, swayPitchScale), CSPFT_FLOAT},
    {"swayYawScale", offsetof(WeaponAttachment, swayYawScale), CSPFT_FLOAT},
    {"swayHorizScale", offsetof(WeaponAttachment, swayHorizScale), CSPFT_FLOAT},
    {"swayVertScale", offsetof(WeaponAttachment, swayVertScale), CSPFT_FLOAT},
    {"adsSwayOverride", offsetof(WeaponAttachment, adsSwayOverride), CSPFT_BOOL},
    {"adsSwayMaxAngle", offsetof(WeaponAttachment, adsSwayMaxAngle), CSPFT_FLOAT},
    {"adsSwayLerpSpeed", offsetof(WeaponAttachment, adsSwayLerpSpeed), CSPFT_FLOAT},
    {"adsSwayPitchScale", offsetof(WeaponAttachment, adsSwayPitchScale), CSPFT_FLOAT},
    {"adsSwayYawScale", offsetof(WeaponAttachment, adsSwayYawScale), CSPFT_FLOAT},
    {"adsSwayHorizScale", offsetof(WeaponAttachment, fAdsSwayHorizScale), CSPFT_FLOAT},
    {"adsSwayVertScale", offsetof(WeaponAttachment, fAdsSwayVertScale), CSPFT_FLOAT},
    {"adsMoveSpeedScale", offsetof(WeaponAttachment, adsMoveSpeedScale), CSPFT_FLOAT},
    {"hipSpreadMinScale", offsetof(WeaponAttachment, fHipSpreadMinScale), CSPFT_FLOAT},
    {"hipSpreadMaxScale", offsetof(WeaponAttachment, fHipSpreadMaxScale), CSPFT_FLOAT},
    {"strafeRotR", offsetof(WeaponAttachment, strafeRotR), CSPFT_FLOAT},
    {"standMoveF", offsetof(WeaponAttachment, standMoveF), CSPFT_FLOAT},
    {"standRotP", offsetof(WeaponAttachment, vStandRot.x), CSPFT_FLOAT},
    {"standRotY", offsetof(WeaponAttachment, vStandRot.y), CSPFT_FLOAT},
    {"standRotR", offsetof(WeaponAttachment, vStandRot.z), CSPFT_FLOAT},
    {"fireTimeScale", offsetof(WeaponAttachment, fFireTimeScale), CSPFT_FLOAT},
    {"reloadTimeScale", offsetof(WeaponAttachment, fReloadTimeScale), CSPFT_FLOAT},
    {"reloadEmptyTimeScale", offsetof(WeaponAttachment, fReloadEmptyTimeScale), CSPFT_FLOAT},
    {"reloadAddTimeScale", offsetof(WeaponAttachment, fReloadAddTimeScale), CSPFT_FLOAT},
    {"reloadQuickTimeScale", offsetof(WeaponAttachment, fReloadQuickTimeScale), CSPFT_FLOAT},
    {"reloadQuickEmptyTimeScale", offsetof(WeaponAttachment, fReloadQuickEmptyTimeScale), CSPFT_FLOAT},
    {"reloadQuickAddTimeScale", offsetof(WeaponAttachment, fReloadQuickAddTimeScale), CSPFT_FLOAT},
    {"perks1", offsetof(WeaponAttachment, perks[0]), CSPFT_UINT},
    {"perks0", offsetof(WeaponAttachment, perks[1]), CSPFT_UINT},
    {"altWeaponAdsOnly", offsetof(WeaponAttachment, bAltWeaponAdsOnly), CSPFT_BOOL},
    {"altWeaponDisableSwitching", offsetof(WeaponAttachment, bAltWeaponDisableSwitching), CSPFT_BOOL},
    {"altScopeADSTransInTime", offsetof(WeaponAttachment, altScopeADSTransInTime), CSPFT_FLOAT},
    {"altScopeADSTransOutTime", offsetof(WeaponAttachment, altScopeADSTransOutTime), CSPFT_FLOAT},
    {"silenced", offsetof(WeaponAttachment, bSilenced), CSPFT_BOOL},
    {"dualMag", offsetof(WeaponAttachment, bDualMag), CSPFT_BOOL},
    {"laserSight", offsetof(WeaponAttachment, laserSight), CSPFT_BOOL},
    {"infrared", offsetof(WeaponAttachment, bInfraRed), CSPFT_BOOL},
    {"useAsMelee", offsetof(WeaponAttachment, bUseAsMelee), CSPFT_BOOL},
    {"dualWield", offsetof(WeaponAttachment, bDualWield), CSPFT_BOOL},
    {"sharedAmmo", offsetof(WeaponAttachment, sharedAmmo), CSPFT_BOOL},
    {"mmsWeapon", offsetof(WeaponAttachment, mmsWeapon), CSPFT_BOOL},
    {"mmsInScope", offsetof(WeaponAttachment, mmsInScope), CSPFT_BOOL},
    {"mmsFOV", offsetof(WeaponAttachment, mmsFOV), CSPFT_FLOAT},
    {"mmsAspect", offsetof(WeaponAttachment, mmsAspect), CSPFT_FLOAT},
    {"mmsMaxDist", offsetof(WeaponAttachment, mmsMaxDist), CSPFT_FLOAT},
    {"clipSizeScale", offsetof(WeaponAttachment, clipSizeScale), CSPFT_FLOAT},
    {"clipSize", offsetof(WeaponAttachment, iClipSize), CSPFT_INT},
    {"stackFire", offsetof(WeaponAttachment, stackFire), CSPFT_FLOAT},
    {"stackFireSpread", offsetof(WeaponAttachment, stackFireSpread), CSPFT_FLOAT},
    {"stackFireAccuracyDecay", offsetof(WeaponAttachment, stackFireAccuracyDecay), CSPFT_FLOAT},
    {"customFloat0", offsetof(WeaponAttachment, customFloat0), CSPFT_FLOAT},
    {"customFloat1", offsetof(WeaponAttachment, customFloat1), CSPFT_FLOAT},
    {"customFloat2", offsetof(WeaponAttachment, customFloat2), CSPFT_FLOAT},
    {"customBool0", offsetof(WeaponAttachment, customBool0), CSPFT_BOOL},
    {"customBool1", offsetof(WeaponAttachment, customBool1), CSPFT_BOOL},
    {"customBool2", offsetof(WeaponAttachment, customBool2), CSPFT_BOOL},
};

namespace T6
{
    class InfoStringFromAttachmentConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            switch (static_cast<attachmentFieldType_t>(field.iFieldType))
            {
            case AFT_ATTACHMENTTYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szAttachmentTypeNames, std::extent<decltype(szAttachmentTypeNames)>::value);
                break;

            case AFT_PENETRATE_TYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, penetrateTypeNames, std::extent<decltype(penetrateTypeNames)>::value);
                break;

            case AFT_FIRETYPE:
                FillFromEnumInt(std::string(field.szName), field.iOffset, szWeapFireTypeNames, std::extent<decltype(szWeapFireTypeNames)>::value);
                break;

            default:
                break;
            }
        }

    public:
        InfoStringFromAttachmentConverter(const WeaponAttachment* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

InfoString AssetDumperWeaponAttachment::CreateInfoString(XAssetInfo<WeaponAttachment>* asset)
{
    InfoStringFromAttachmentConverter converter(asset->Asset(), attachment_fields, std::extent<decltype(attachment_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
    {
        assert(scrStr < asset->m_zone->m_script_strings.Count());
        if (scrStr >= asset->m_zone->m_script_strings.Count())
            return "";

        return asset->m_zone->m_script_strings[scrStr];
    });

    return converter.Convert();
}

bool AssetDumperWeaponAttachment::ShouldDump(XAssetInfo<WeaponAttachment>* asset)
{
    return true;
}

bool AssetDumperWeaponAttachment::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperWeaponAttachment::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperWeaponAttachment::GetFileNameForAsset(Zone* zone, XAssetInfo<WeaponAttachment>* asset)
{
    return "attachment/" + asset->m_name;
}

GdtEntry AssetDumperWeaponAttachment::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperWeaponAttachment::DumpRaw(AssetDumpingContext& context, XAssetInfo<WeaponAttachment>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(FILE_TYPE_STR);
    stream.write(stringValue.c_str(), stringValue.size());
}

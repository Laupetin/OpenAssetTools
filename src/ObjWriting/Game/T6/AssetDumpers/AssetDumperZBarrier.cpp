#include "AssetDumperZBarrier.h"

#include <cassert>
#include <type_traits>

#include "Game/T6/InfoStringT6.h"

using namespace T6;

cspField_t AssetDumperZBarrier::zbarrier_fields[]
{
    {"delayBetweenGeneralRepSounds", offsetof(ZBarrierDef, delayBetweenRepSoundsDuration), CSPFT_FLOAT},
    {"earthquakeMaxDuration", offsetof(ZBarrierDef, earthquakeMaxDuration), CSPFT_FLOAT},
    {"earthquakeMaxScale", offsetof(ZBarrierDef, earthquakeMaxScale), CSPFT_FLOAT},
    {"earthquakeMinDuration", offsetof(ZBarrierDef, earthquakeMinDuration), CSPFT_FLOAT},
    {"earthquakeMinScale", offsetof(ZBarrierDef, earthquakeMinScale), CSPFT_FLOAT},
    {"earthquakeOnRepair", offsetof(ZBarrierDef, earthquakeOnRepair), CSPFT_UINT},
    {"earthquakeRadius", offsetof(ZBarrierDef, earthquakeRadius), CSPFT_FLOAT},
    {"generalRepairSound0", offsetof(ZBarrierDef, generalRepairSound1), CSPFT_SOUND_ALIAS_ID},
    {"generalRepairSound1", offsetof(ZBarrierDef, generalRepairSound2), CSPFT_SOUND_ALIAS_ID},
    {"upgradedGeneralRepairSound0", offsetof(ZBarrierDef, upgradedGeneralRepairSound1), CSPFT_SOUND_ALIAS_ID},
    {"upgradedGeneralRepairSound1", offsetof(ZBarrierDef, upgradedGeneralRepairSound2), CSPFT_SOUND_ALIAS_ID},
    {"useDelayBetweenGeneralRepSounds", offsetof(ZBarrierDef, delayBetweenRepSounds), CSPFT_UINT},
    {"taunts", offsetof(ZBarrierDef, taunts), CSPFT_UINT},
    {"reachThroughAttacks", offsetof(ZBarrierDef, reachThroughAttacks), CSPFT_UINT},
    {"zombieTauntAnimState", offsetof(ZBarrierDef, zombieTauntAnimState), CSPFT_SCRIPT_STRING},
    {"zombieReachThroughAnimState", offsetof(ZBarrierDef, zombieReachThroughAnimState), CSPFT_SCRIPT_STRING},
    {"numAttackSlots", offsetof(ZBarrierDef, numAttackSlots), CSPFT_UINT},
    {"attackSpotHorzOffset", offsetof(ZBarrierDef, attackSpotHorzOffset), CSPFT_FLOAT},
    {"autoHideOpenPieces", offsetof(ZBarrierDef, autoHideOpenPieces), CSPFT_UINT},
    {"alternateBoardModel1", offsetof(ZBarrierDef, boards[0].pAlternateBoardModel), CSPFT_XMODEL},
    {"boardAnim1", offsetof(ZBarrierDef, boards[0].pBoardAnim), CSPFT_STRING},
    {"boardModel1", offsetof(ZBarrierDef, boards[0].pBoardModel), CSPFT_XMODEL},
    {"boardRepairSound1", offsetof(ZBarrierDef, boards[0].boardRepairSound), CSPFT_SOUND_ALIAS_ID},
    {"boardRepairHoverSound1", offsetof(ZBarrierDef, boards[0].boardRepairHoverSound), CSPFT_SOUND_ALIAS_ID},
    {"OffsetRepairFxX10", offsetof(ZBarrierDef, boards[0].repairEffect1Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxX11", offsetof(ZBarrierDef, boards[0].repairEffect2Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxY10", offsetof(ZBarrierDef, boards[0].repairEffect1Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxY11", offsetof(ZBarrierDef, boards[0].repairEffect2Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxZ10", offsetof(ZBarrierDef, boards[0].repairEffect1Offset.z), CSPFT_FLOAT},
    {"OffsetRepairFxZ11", offsetof(ZBarrierDef, boards[0].repairEffect2Offset.z), CSPFT_FLOAT},
    {"pauseAndRepeatBoardRepairSound1", offsetof(ZBarrierDef, boards[0].pauseAndRepeatRepSound), CSPFT_UINT},
    {"pauseBetweenRepSoundsMax1", offsetof(ZBarrierDef, boards[0].maxPause), CSPFT_FLOAT},
    {"pauseBetweenRepSoundsMin1", offsetof(ZBarrierDef, boards[0].minPause), CSPFT_FLOAT},
    {"proBoardNumRepsToTear1", offsetof(ZBarrierDef, boards[0].numRepsToPullProBoard), CSPFT_UINT},
    {"repairFx10", offsetof(ZBarrierDef, boards[0].repairEffect1), CSPFT_FX},
    {"repairFx11", offsetof(ZBarrierDef, boards[0].repairEffect2), CSPFT_FX},
    {"tearAnim1", offsetof(ZBarrierDef, boards[0].pTearAnim), CSPFT_STRING},
    {"upgradedBoardModel1", offsetof(ZBarrierDef, boards[0].pUpgradedBoardModel), CSPFT_XMODEL},
    {"zombieBoardTearAnimState1", offsetof(ZBarrierDef, boards[0].zombieBoardTearStateName), CSPFT_SCRIPT_STRING},
    {"zombieBoardTearAnimSubState1", offsetof(ZBarrierDef, boards[0].zombieBoardTearSubStateName), CSPFT_SCRIPT_STRING},
    {"alternateBoardModel2", offsetof(ZBarrierDef, boards[1].pAlternateBoardModel), CSPFT_XMODEL},
    {"boardAnim2", offsetof(ZBarrierDef, boards[1].pBoardAnim), CSPFT_STRING},
    {"boardModel2", offsetof(ZBarrierDef, boards[1].pBoardModel), CSPFT_XMODEL},
    {"boardRepairSound2", offsetof(ZBarrierDef, boards[1].boardRepairSound), CSPFT_SOUND_ALIAS_ID},
    {"boardRepairHoverSound2", offsetof(ZBarrierDef, boards[1].boardRepairHoverSound), CSPFT_SOUND_ALIAS_ID},
    {"OffsetRepairFxX20", offsetof(ZBarrierDef, boards[1].repairEffect1Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxX21", offsetof(ZBarrierDef, boards[1].repairEffect2Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxY20", offsetof(ZBarrierDef, boards[1].repairEffect1Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxY21", offsetof(ZBarrierDef, boards[1].repairEffect2Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxZ20", offsetof(ZBarrierDef, boards[1].repairEffect1Offset.z), CSPFT_FLOAT},
    {"OffsetRepairFxZ21", offsetof(ZBarrierDef, boards[1].repairEffect2Offset.z), CSPFT_FLOAT},
    {"pauseAndRepeatBoardRepairSound2", offsetof(ZBarrierDef, boards[1].pauseAndRepeatRepSound), CSPFT_UINT},
    {"pauseBetweenRepSoundsMax2", offsetof(ZBarrierDef, boards[1].maxPause), CSPFT_FLOAT},
    {"pauseBetweenRepSoundsMin2", offsetof(ZBarrierDef, boards[1].minPause), CSPFT_FLOAT},
    {"proBoardNumRepsToTear2", offsetof(ZBarrierDef, boards[1].numRepsToPullProBoard), CSPFT_UINT},
    {"repairFx20", offsetof(ZBarrierDef, boards[1].repairEffect1), CSPFT_FX},
    {"repairFx21", offsetof(ZBarrierDef, boards[1].repairEffect2), CSPFT_FX},
    {"tearAnim2", offsetof(ZBarrierDef, boards[1].pTearAnim), CSPFT_STRING},
    {"upgradedBoardModel2", offsetof(ZBarrierDef, boards[1].pUpgradedBoardModel), CSPFT_XMODEL},
    {"zombieBoardTearAnimState2", offsetof(ZBarrierDef, boards[1].zombieBoardTearStateName), CSPFT_SCRIPT_STRING},
    {"zombieBoardTearAnimSubState2", offsetof(ZBarrierDef, boards[1].zombieBoardTearSubStateName), CSPFT_SCRIPT_STRING},
    {"alternateBoardModel3", offsetof(ZBarrierDef, boards[2].pAlternateBoardModel), CSPFT_XMODEL},
    {"boardAnim3", offsetof(ZBarrierDef, boards[2].pBoardAnim), CSPFT_STRING},
    {"boardModel3", offsetof(ZBarrierDef, boards[2].pBoardModel), CSPFT_XMODEL},
    {"boardRepairSound3", offsetof(ZBarrierDef, boards[2].boardRepairSound), CSPFT_SOUND_ALIAS_ID},
    {"boardRepairHoverSound3", offsetof(ZBarrierDef, boards[2].boardRepairHoverSound), CSPFT_SOUND_ALIAS_ID},
    {"OffsetRepairFxX30", offsetof(ZBarrierDef, boards[2].repairEffect1Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxX31", offsetof(ZBarrierDef, boards[2].repairEffect2Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxY30", offsetof(ZBarrierDef, boards[2].repairEffect1Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxY31", offsetof(ZBarrierDef, boards[2].repairEffect2Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxZ30", offsetof(ZBarrierDef, boards[2].repairEffect1Offset.z), CSPFT_FLOAT},
    {"OffsetRepairFxZ31", offsetof(ZBarrierDef, boards[2].repairEffect2Offset.z), CSPFT_FLOAT},
    {"pauseAndRepeatBoardRepairSound3", offsetof(ZBarrierDef, boards[2].pauseAndRepeatRepSound), CSPFT_UINT},
    {"pauseBetweenRepSoundsMax3", offsetof(ZBarrierDef, boards[2].maxPause), CSPFT_FLOAT},
    {"pauseBetweenRepSoundsMin3", offsetof(ZBarrierDef, boards[2].minPause), CSPFT_FLOAT},
    {"proBoardNumRepsToTear3", offsetof(ZBarrierDef, boards[2].numRepsToPullProBoard), CSPFT_UINT},
    {"repairFx30", offsetof(ZBarrierDef, boards[2].repairEffect1), CSPFT_FX},
    {"repairFx31", offsetof(ZBarrierDef, boards[2].repairEffect2), CSPFT_FX},
    {"tearAnim3", offsetof(ZBarrierDef, boards[2].pTearAnim), CSPFT_STRING},
    {"upgradedBoardModel3", offsetof(ZBarrierDef, boards[2].pUpgradedBoardModel), CSPFT_XMODEL},
    {"zombieBoardTearAnimState3", offsetof(ZBarrierDef, boards[2].zombieBoardTearStateName), CSPFT_SCRIPT_STRING},
    {"zombieBoardTearAnimSubState3", offsetof(ZBarrierDef, boards[2].zombieBoardTearSubStateName), CSPFT_SCRIPT_STRING},
    {"alternateBoardModel4", offsetof(ZBarrierDef, boards[3].pAlternateBoardModel), CSPFT_XMODEL},
    {"boardAnim4", offsetof(ZBarrierDef, boards[3].pBoardAnim), CSPFT_STRING},
    {"boardModel4", offsetof(ZBarrierDef, boards[3].pBoardModel), CSPFT_XMODEL},
    {"boardRepairSound4", offsetof(ZBarrierDef, boards[3].boardRepairSound), CSPFT_SOUND_ALIAS_ID},
    {"boardRepairHoverSound4", offsetof(ZBarrierDef, boards[3].boardRepairHoverSound), CSPFT_SOUND_ALIAS_ID},
    {"OffsetRepairFxX40", offsetof(ZBarrierDef, boards[3].repairEffect1Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxX41", offsetof(ZBarrierDef, boards[3].repairEffect2Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxY40", offsetof(ZBarrierDef, boards[3].repairEffect1Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxY41", offsetof(ZBarrierDef, boards[3].repairEffect2Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxZ40", offsetof(ZBarrierDef, boards[3].repairEffect1Offset.z), CSPFT_FLOAT},
    {"OffsetRepairFxZ41", offsetof(ZBarrierDef, boards[3].repairEffect2Offset.z), CSPFT_FLOAT},
    {"pauseAndRepeatBoardRepairSound4", offsetof(ZBarrierDef, boards[3].pauseAndRepeatRepSound), CSPFT_UINT},
    {"pauseBetweenRepSoundsMax4", offsetof(ZBarrierDef, boards[3].maxPause), CSPFT_FLOAT},
    {"pauseBetweenRepSoundsMin4", offsetof(ZBarrierDef, boards[3].minPause), CSPFT_FLOAT},
    {"proBoardNumRepsToTear4", offsetof(ZBarrierDef, boards[3].numRepsToPullProBoard), CSPFT_UINT},
    {"repairFx40", offsetof(ZBarrierDef, boards[3].repairEffect1), CSPFT_FX},
    {"repairFx41", offsetof(ZBarrierDef, boards[3].repairEffect2), CSPFT_FX},
    {"tearAnim4", offsetof(ZBarrierDef, boards[3].pTearAnim), CSPFT_STRING},
    {"upgradedBoardModel4", offsetof(ZBarrierDef, boards[3].pUpgradedBoardModel), CSPFT_XMODEL},
    {"zombieBoardTearAnimState4", offsetof(ZBarrierDef, boards[3].zombieBoardTearStateName), CSPFT_SCRIPT_STRING},
    {"zombieBoardTearAnimSubState4", offsetof(ZBarrierDef, boards[3].zombieBoardTearSubStateName), CSPFT_SCRIPT_STRING},
    {"alternateBoardModel5", offsetof(ZBarrierDef, boards[4].pAlternateBoardModel), CSPFT_XMODEL},
    {"boardAnim5", offsetof(ZBarrierDef, boards[4].pBoardAnim), CSPFT_STRING},
    {"boardModel5", offsetof(ZBarrierDef, boards[4].pBoardModel), CSPFT_XMODEL},
    {"boardRepairSound5", offsetof(ZBarrierDef, boards[4].boardRepairSound), CSPFT_SOUND_ALIAS_ID},
    {"boardRepairHoverSound5", offsetof(ZBarrierDef, boards[4].boardRepairHoverSound), CSPFT_SOUND_ALIAS_ID},
    {"OffsetRepairFxX50", offsetof(ZBarrierDef, boards[4].repairEffect1Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxX51", offsetof(ZBarrierDef, boards[4].repairEffect2Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxY50", offsetof(ZBarrierDef, boards[4].repairEffect1Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxY51", offsetof(ZBarrierDef, boards[4].repairEffect2Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxZ50", offsetof(ZBarrierDef, boards[4].repairEffect1Offset.z), CSPFT_FLOAT},
    {"OffsetRepairFxZ51", offsetof(ZBarrierDef, boards[4].repairEffect2Offset.z), CSPFT_FLOAT},
    {"pauseAndRepeatBoardRepairSound5", offsetof(ZBarrierDef, boards[4].pauseAndRepeatRepSound), CSPFT_UINT},
    {"pauseBetweenRepSoundsMax5", offsetof(ZBarrierDef, boards[4].maxPause), CSPFT_FLOAT},
    {"pauseBetweenRepSoundsMin5", offsetof(ZBarrierDef, boards[4].minPause), CSPFT_FLOAT},
    {"proBoardNumRepsToTear5", offsetof(ZBarrierDef, boards[4].numRepsToPullProBoard), CSPFT_UINT},
    {"repairFx50", offsetof(ZBarrierDef, boards[4].repairEffect1), CSPFT_FX},
    {"repairFx51", offsetof(ZBarrierDef, boards[4].repairEffect2), CSPFT_FX},
    {"tearAnim5", offsetof(ZBarrierDef, boards[4].pTearAnim), CSPFT_STRING},
    {"upgradedBoardModel5", offsetof(ZBarrierDef, boards[4].pUpgradedBoardModel), CSPFT_XMODEL},
    {"zombieBoardTearAnimState5", offsetof(ZBarrierDef, boards[4].zombieBoardTearStateName), CSPFT_SCRIPT_STRING},
    {"zombieBoardTearAnimSubState5", offsetof(ZBarrierDef, boards[4].zombieBoardTearSubStateName), CSPFT_SCRIPT_STRING},
    {"alternateBoardModel6", offsetof(ZBarrierDef, boards[5].pAlternateBoardModel), CSPFT_XMODEL},
    {"boardAnim6", offsetof(ZBarrierDef, boards[5].pBoardAnim), CSPFT_STRING},
    {"boardModel6", offsetof(ZBarrierDef, boards[5].pBoardModel), CSPFT_XMODEL},
    {"boardRepairSound6", offsetof(ZBarrierDef, boards[5].boardRepairSound), CSPFT_SOUND_ALIAS_ID},
    {"boardRepairHoverSound6", offsetof(ZBarrierDef, boards[5].boardRepairHoverSound), CSPFT_SOUND_ALIAS_ID},
    {"OffsetRepairFxX60", offsetof(ZBarrierDef, boards[5].repairEffect1Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxX61", offsetof(ZBarrierDef, boards[5].repairEffect2Offset.x), CSPFT_FLOAT},
    {"OffsetRepairFxY60", offsetof(ZBarrierDef, boards[5].repairEffect1Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxY61", offsetof(ZBarrierDef, boards[5].repairEffect2Offset.y), CSPFT_FLOAT},
    {"OffsetRepairFxZ60", offsetof(ZBarrierDef, boards[5].repairEffect1Offset.z), CSPFT_FLOAT},
    {"OffsetRepairFxZ61", offsetof(ZBarrierDef, boards[5].repairEffect2Offset.z), CSPFT_FLOAT},
    {"pauseAndRepeatBoardRepairSound6", offsetof(ZBarrierDef, boards[5].pauseAndRepeatRepSound), CSPFT_UINT},
    {"pauseBetweenRepSoundsMax6", offsetof(ZBarrierDef, boards[5].maxPause), CSPFT_FLOAT},
    {"pauseBetweenRepSoundsMin6", offsetof(ZBarrierDef, boards[5].minPause), CSPFT_FLOAT},
    {"proBoardNumRepsToTear6", offsetof(ZBarrierDef, boards[5].numRepsToPullProBoard), CSPFT_UINT},
    {"repairFx60", offsetof(ZBarrierDef, boards[5].repairEffect1), CSPFT_FX},
    {"repairFx61", offsetof(ZBarrierDef, boards[5].repairEffect2), CSPFT_FX},
    {"tearAnim6", offsetof(ZBarrierDef, boards[5].pTearAnim), CSPFT_STRING},
    {"upgradedBoardModel6", offsetof(ZBarrierDef, boards[5].pUpgradedBoardModel), CSPFT_XMODEL},
    {"zombieBoardTearAnimState6", offsetof(ZBarrierDef, boards[5].zombieBoardTearStateName), CSPFT_SCRIPT_STRING},
    {"zombieBoardTearAnimSubState6", offsetof(ZBarrierDef, boards[5].zombieBoardTearSubStateName), CSPFT_SCRIPT_STRING},
};

namespace T6
{
    class InfoStringFromZBarrierConverter final : public InfoStringFromStructConverter
    {
    protected:
        void FillFromExtensionField(const cspField_t& field) override
        {
            assert(false);
        }

    public:
        InfoStringFromZBarrierConverter(const ZBarrierDef* structure, const cspField_t* fields, const size_t fieldCount, std::function<std::string(scr_string_t)> scriptStringValueCallback)
            : InfoStringFromStructConverter(structure, fields, fieldCount, std::move(scriptStringValueCallback))
        {
        }
    };
}

InfoString AssetDumperZBarrier::CreateInfoString(XAssetInfo<ZBarrierDef>* asset)
{
    InfoStringFromZBarrierConverter converter(asset->Asset(), zbarrier_fields, std::extent<decltype(zbarrier_fields)>::value, [asset](const scr_string_t scrStr) -> std::string
    {
        assert(scrStr < asset->m_zone->m_script_strings.Count());
        if (scrStr >= asset->m_zone->m_script_strings.Count())
            return "";

        return asset->m_zone->m_script_strings[scrStr];
    });

    return converter.Convert();
}

bool AssetDumperZBarrier::ShouldDump(XAssetInfo<ZBarrierDef>* asset)
{
    return true;
}

bool AssetDumperZBarrier::CanDumpAsRaw()
{
    return true;
}

bool AssetDumperZBarrier::CanDumpAsGdtEntry()
{
    return true;
}

std::string AssetDumperZBarrier::GetFileNameForAsset(Zone* zone, XAssetInfo<ZBarrierDef>* asset)
{
    return "zbarrier/" + asset->m_name;
}

GdtEntry AssetDumperZBarrier::DumpGdtEntry(AssetDumpingContext& context, XAssetInfo<ZBarrierDef>* asset)
{
    const auto infoString = CreateInfoString(asset);
    GdtEntry gdtEntry(asset->m_name, GDF_NAME);
    infoString.ToGdtProperties(FILE_TYPE_STR, gdtEntry);

    return gdtEntry;
}

void AssetDumperZBarrier::DumpRaw(AssetDumpingContext& context, XAssetInfo<ZBarrierDef>* asset, std::ostream& stream)
{
    const auto infoString = CreateInfoString(asset);
    const auto stringValue = infoString.ToString(FILE_TYPE_STR);
    stream.write(stringValue.c_str(), stringValue.size());
}

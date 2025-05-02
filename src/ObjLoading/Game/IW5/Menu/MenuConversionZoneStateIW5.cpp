#include "MenuConversionZoneStateIW5.h"

#include <cstring>

using namespace IW5;

MenuConversionZoneState::MenuConversionZoneState()
    : m_zone(nullptr),
      m_supporting_data(nullptr)
{
}

void MenuConversionZoneState::Inject(ZoneAssetCreationInjection& inject)
{
    auto& memory = inject.m_zone.Memory();

    m_zone = &inject.m_zone;
    m_supporting_data = memory.Alloc<ExpressionSupportingData>();
}

Statement_s* MenuConversionZoneState::FindFunction(const std::string& functionName)
{
    const auto foundFunction = m_function_by_name.find(functionName);

    if (foundFunction != m_function_by_name.end())
        return foundFunction->second;

    return nullptr;
}

Statement_s* MenuConversionZoneState::AddFunction(const std::string& functionName, Statement_s* function)
{
    m_functions.push_back(function);
    m_function_by_name.emplace(std::make_pair(functionName, function));

    return function;
}

size_t MenuConversionZoneState::AddStaticDvar(const std::string& dvarName)
{
    const auto foundDvar = m_dvars_by_name.find(dvarName);

    if (foundDvar != m_dvars_by_name.end())
        return foundDvar->second;

    auto& memory = m_zone->Memory();
    auto* staticDvar = memory.Alloc<StaticDvar>();

    staticDvar->dvarName = memory.Dup(dvarName.c_str());
    staticDvar->dvar = nullptr;

    const auto staticDvarIndex = m_static_dvars.size();
    m_static_dvars.push_back(staticDvar);
    m_dvars_by_name.emplace(std::make_pair(dvarName, staticDvarIndex));

    return staticDvarIndex;
}

const char* MenuConversionZoneState::AddString(const std::string& str)
{
    const auto foundString = m_strings_by_value.find(str);

    if (foundString != m_strings_by_value.end())
        return foundString->second;

    auto& memory = m_zone->Memory();
    const auto* strDuped = memory.Dup(str.c_str());

    m_strings.push_back(strDuped);
    m_strings_by_value.emplace(std::make_pair(str, strDuped));

    return strDuped;
}

void MenuConversionZoneState::AddLoadedFile(std::string loadedFileName, std::vector<XAssetInfo<menuDef_t>*> menusOfFile)
{
    m_menus_by_filename.emplace(std::make_pair(std::move(loadedFileName), std::move(menusOfFile)));
}

void MenuConversionZoneState::FinalizeSupportingData() const
{
    auto& memory = m_zone->Memory();

    m_supporting_data->uifunctions.totalFunctions = static_cast<int>(m_functions.size());
    m_supporting_data->staticDvarList.numStaticDvars = static_cast<int>(m_static_dvars.size());
    m_supporting_data->uiStrings.totalStrings = static_cast<int>(m_strings.size());

    if (m_supporting_data->uifunctions.functions)
        memory.Free(m_supporting_data->uifunctions.functions);

    if (m_supporting_data->staticDvarList.staticDvars)
        memory.Free(m_supporting_data->staticDvarList.staticDvars);

    if (m_supporting_data->uiStrings.strings)
        memory.Free(m_supporting_data->uiStrings.strings);

    if (!m_functions.empty())
    {
        m_supporting_data->uifunctions.functions = memory.Alloc<Statement_s*>(m_functions.size());
        memcpy(m_supporting_data->uifunctions.functions, m_functions.data(), sizeof(void*) * m_functions.size());
    }
    else
        m_supporting_data->uifunctions.functions = nullptr;

    if (!m_static_dvars.empty())
    {
        m_supporting_data->staticDvarList.staticDvars = memory.Alloc<StaticDvar*>(m_static_dvars.size());
        memcpy(m_supporting_data->staticDvarList.staticDvars, m_static_dvars.data(), sizeof(void*) * m_static_dvars.size());
    }
    else
        m_supporting_data->staticDvarList.staticDvars = nullptr;

    if (!m_strings.empty())
    {
        m_supporting_data->uiStrings.strings = memory.Alloc<const char*>(m_strings.size());
        memcpy(m_supporting_data->uiStrings.strings, m_strings.data(), sizeof(void*) * m_strings.size());
    }
    else
        m_supporting_data->uiStrings.strings = nullptr;
}

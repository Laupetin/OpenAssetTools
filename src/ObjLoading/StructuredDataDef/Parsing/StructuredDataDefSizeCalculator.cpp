#include "StructuredDataDefSizeCalculator.h"

#include <cassert>
#include <sstream>

#include "Utils/ClassUtils.h"

using namespace sdd;

SizeCalculationException::SizeCalculationException(std::string message)
    : m_message(std::move(message))
{
}

const std::string& SizeCalculationException::Message() const
{
    return m_message;
}

char const* SizeCalculationException::what() const
{
    return m_message.c_str();
}

class StructuredDataDefSizeCalculatorInternal
{
    CommonStructuredDataDef& m_def;
    std::vector<CommonStructuredDataType> m_type_stack;
    std::vector<bool> m_struct_in_calculation;
    std::vector<bool> m_struct_calculated;
    std::vector<bool> m_indexed_array_calculated;
    std::vector<bool> m_enumed_array_calculated;

    _NORETURN void ThrowCircularDependencyException(const size_t structIndex) const
    {
        assert(structIndex < m_def.m_structs.size());

        std::ostringstream ss;

        ss << "Circular dependencies detected: ";

        auto foundDependencyChainStart = false;
        for(const auto& type : m_type_stack)
        {
            if(foundDependencyChainStart)
            {
                if(type.m_category == CommonStructuredDataTypeCategory::STRUCT)
                {
                    assert(type.m_info.type_index < m_def.m_structs.size());
                    ss << " -> " << m_def.m_structs[type.m_info.type_index]->m_name;
                }
            }
            else if (type.m_category == CommonStructuredDataTypeCategory::STRUCT && type.m_info.type_index == structIndex)
            {
                foundDependencyChainStart = true;
                ss << m_def.m_structs[structIndex]->m_name;
            }
        }

        ss << " -> " << m_def.m_structs[structIndex]->m_name;

        throw SizeCalculationException(ss.str());
    }

    void CalculateForStruct(const size_t index, CommonStructuredDataStruct& _struct)
    {
        if (m_struct_calculated[index])
            return;

        if (m_struct_in_calculation[index])
            ThrowCircularDependencyException(index);
        m_struct_in_calculation[index] = true;

        m_type_stack.emplace_back(CommonStructuredDataTypeCategory::STRUCT, index);

        // TODO: Actually calculate stuff

        m_struct_calculated[index] = true;
        m_type_stack.pop_back();
    }

    void CalculateForIndexedArray(size_t index, CommonStructuredDataIndexedArray& indexedArray)
    {
        // TODO: Actually calculate stuff
    }

    void CalculateForEnumedArray(size_t index, CommonStructuredDataEnumedArray& enumedArray)
    {
        // TODO: Actually calculate stuff
    }

    void CalculateForType(const CommonStructuredDataType type)
    {
        switch (type.m_category)
        {
        case CommonStructuredDataTypeCategory::STRUCT:
            assert(type.m_info.type_index < m_def.m_structs.size());
            CalculateForStruct(type.m_info.type_index, *m_def.m_structs[type.m_info.type_index]);
            break;
        case CommonStructuredDataTypeCategory::INDEXED_ARRAY:
            assert(type.m_info.type_index < m_def.m_indexed_arrays.size());
            CalculateForIndexedArray(type.m_info.type_index, m_def.m_indexed_arrays[type.m_info.type_index]);
            break;
        case CommonStructuredDataTypeCategory::ENUM_ARRAY:
            assert(type.m_info.type_index < m_def.m_enumed_arrays.size());
            CalculateForEnumedArray(type.m_info.type_index, m_def.m_enumed_arrays[type.m_info.type_index]);
            break;
        default:
            break;
        }
    }

public:
    explicit StructuredDataDefSizeCalculatorInternal(CommonStructuredDataDef& def)
        : m_def(def),
          m_struct_in_calculation(def.m_structs.size()),
          m_struct_calculated(def.m_structs.size()),
          m_indexed_array_calculated(def.m_indexed_arrays.size()),
          m_enumed_array_calculated(def.m_enumed_arrays.size())
    {
    }

    void CalculateSizesAndOffsets()
    {
        auto index = 0u;
        for (auto& _struct : m_def.m_structs)
            CalculateForStruct(index++, *_struct);

        index = 0u;
        for (auto& indexedArray : m_def.m_indexed_arrays)
            CalculateForIndexedArray(index++, indexedArray);

        index = 0u;
        for (auto& enumedArray : m_def.m_enumed_arrays)
            CalculateForEnumedArray(index++, enumedArray);
    }
};

void StructuredDataDefSizeCalculator::CalculateSizesAndOffsetsForDef(CommonStructuredDataDef& def)
{
    StructuredDataDefSizeCalculatorInternal calculator(def);
    calculator.CalculateSizesAndOffsets();
}

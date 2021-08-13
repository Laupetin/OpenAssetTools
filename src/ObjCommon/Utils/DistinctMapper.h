#pragma once

#include <map>
#include <vector>

#include "Utils/ClassUtils.h"

/*
template <typename T>
class Deduplicator
{
public:
    Deduplicator()
        : m_current_entry_index(0)
    {
    }

    explicit Deduplicator(const size_t totalEntryCount)
        : m_current_entry_index(0)
    {
        m_position_lookup.reserve(totalEntryCount);
    }

    bool AddEntry(T pos)
    {
        const auto mapEntry = m_index_map.find(pos);
        if (mapEntry == m_index_map.end())
        {
            m_position_lookup.push_back(m_current_entry_index);
            m_unique_entry_position_indices.push_back(m_current_entry_index);
            m_index_map.emplace(std::make_pair(pos, m_current_entry_index));
            m_current_entry_index++;
            return true;
        }

        m_position_lookup.push_back(mapEntry->second);
        m_current_entry_index++;
        return false;
    }

    size_t GetUniqueIndexForIndex(const size_t entryIndex)
    {
        if (entryIndex >= m_position_lookup.size())
            return 0;

        return m_position_lookup[entryIndex];
    }

    _NODISCARD const std::vector<size_t>& GetUniqueEntryIndices() const
    {
        return m_unique_entry_position_indices;
    }

private:
    size_t m_current_entry_index;
    std::vector<size_t> m_unique_entry_position_indices;
    std::vector<size_t> m_position_lookup;
    std::map<T, int> m_index_map;
};*/

template <typename T>
class DistinctMapper
{
public:
    DistinctMapper()
        : m_input_entry_index(0),
          m_distinct_entry_index(0)
    {
    }

    explicit DistinctMapper(const size_t totalInputCount)
        : m_input_entry_index(0),
          m_distinct_entry_index(0)
    {
        m_distinct_position_by_input_position.reserve(totalInputCount);
    }

    bool Add(T inputValue)
    {
        const auto mapEntry = m_distinct_position_by_value_map.find(inputValue);
        if (mapEntry == m_distinct_position_by_value_map.end())
        {
            m_distinct_position_by_input_position.push_back(m_distinct_entry_index);
            m_input_position_by_distinct_position.push_back(m_input_entry_index);
            m_distinct_values.push_back(inputValue);
            m_distinct_position_by_value_map.emplace(std::make_pair(std::move(inputValue), m_distinct_entry_index));
            m_distinct_entry_index++;
            m_input_entry_index++;
            return true;
        }

        m_distinct_position_by_input_position.push_back(mapEntry->second);
        m_input_entry_index++;
        return false;
    }

    _NODISCARD size_t GetDistinctPositionByInputPosition(const size_t inputPosition) const
    {
        if (inputPosition >= m_distinct_position_by_input_position.size())
            return 0;

        return m_distinct_position_by_input_position[inputPosition];
    }

    _NODISCARD T GetDistinctValueByInputPosition(const size_t inputPosition) const
    {
        if (inputPosition >= m_distinct_values.size())
            return T{};

        return m_distinct_values[inputPosition];
    }

    _NODISCARD size_t GetInputPositionByDistinctPosition(const size_t distinctPosition) const
    {
        if (distinctPosition >= m_input_position_by_distinct_position.size())
            return 0;

        return m_input_position_by_distinct_position[distinctPosition];
    }

    _NODISCARD const std::vector<T>& GetDistinctValues() const
    {
        return m_distinct_values;
    }

private:
    size_t m_input_entry_index;
    size_t m_distinct_entry_index;
    std::map<T, size_t> m_distinct_position_by_value_map;
    std::vector<size_t> m_distinct_position_by_input_position;
    std::vector<size_t> m_input_position_by_distinct_position;
    std::vector<T> m_distinct_values;
};

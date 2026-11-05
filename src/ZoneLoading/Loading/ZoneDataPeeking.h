#pragma once

#include <cstring>
#include <functional>
#include <istream>
#include <vector>

class ZoneDataPeeking
{
public:
    explicit ZoneDataPeeking(std::istream& stream);

    template<typename T> const T& PeekStruct()
    {
        EnsureHasEnoughData(sizeof(T));

        return *reinterpret_cast<const T*>(m_data.data());
    }

    void PeekWithStream(const std::function<void(std::istream& stream)>& cb) const;

private:
    void EnsureHasEnoughData(size_t size);

    std::istream& m_stream;
    std::vector<char> m_data;
};

#pragma once

#include <ostream>
#include <string>
#include <vector>

class CsvWriter
{
public:
    explicit CsvWriter(std::ostream& stream);

    void WriteHeader(const std::vector<std::string>& columns) const;

    void WriteRow(const std::vector<std::string>& values) const;

    template<typename T> void WriteNumericRow(const std::vector<T>& values) const;

private:
    std::ostream& m_stream;

    static std::string Escape(const std::string& value);
};

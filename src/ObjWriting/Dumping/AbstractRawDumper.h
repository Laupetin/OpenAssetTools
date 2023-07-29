#pragma once

#include <ostream>

class AbstractRawDumper
{
protected:
    explicit AbstractRawDumper(std::ostream& stream);

public:
    virtual ~AbstractRawDumper() = default;
    AbstractRawDumper(const AbstractRawDumper& other) = default;
    AbstractRawDumper(AbstractRawDumper&& other) noexcept = default;
    AbstractRawDumper& operator=(const AbstractRawDumper& other) = delete;
    AbstractRawDumper& operator=(AbstractRawDumper&& other) noexcept = delete;

protected:
    size_t OffsetAdd(size_t len);
    size_t OffsetAdd(const std::string& str);
    void Write(const void* ptr, size_t size) const;
    void Write(const std::string& str) const;

    std::ostream& m_stream;
    size_t m_current_offset;
};
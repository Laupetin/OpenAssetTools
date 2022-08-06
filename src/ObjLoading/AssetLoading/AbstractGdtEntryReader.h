#pragma once

#include <exception>

#include "Utils/ClassUtils.h"
#include "Obj/Gdt/GdtEntry.h"

class GdtReadingException : public std::exception
{
public:
    explicit GdtReadingException(std::string message);
    _NODISCARD const char* what() const noexcept override;

private:
    std::string m_message;
};

class AbstractGdtEntryReader
{
protected:
    explicit AbstractGdtEntryReader(const GdtEntry& entry);

    _NODISCARD std::string ReadStringProperty(const std::string& propertyName, std::string defaultValue = std::string()) const;
    _NODISCARD bool ReadBoolProperty(const std::string& propertyName, bool defaultValue = false) const;
    _NODISCARD int ReadIntegerProperty(const std::string& propertyName, int defaultValue = 0) const;
    _NODISCARD float ReadFloatProperty(const std::string& propertyName, float defaultValue = 0.0f) const;

    const GdtEntry& m_entry;
};

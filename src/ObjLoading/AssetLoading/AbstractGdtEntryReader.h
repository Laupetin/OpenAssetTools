#pragma once

#include "Obj/Gdt/GdtEntry.h"
#include "Utils/ClassUtils.h"

#include <exception>

class GdtReadingException : public std::exception
{
public:
    explicit GdtReadingException(std::string message);
    _NODISCARD const char* what() const noexcept override;

private:
    std::string m_message;
};

struct GdtVec2
{
    float x;
    float y;
};

struct GdtVec3
{
    float x;
    float y;
    float z;
};

struct GdtVec4
{
    float x;
    float y;
    float z;
    float w;
};

class AbstractGdtEntryReader
{
protected:
    explicit AbstractGdtEntryReader(const GdtEntry& entry);

    _NODISCARD std::string ReadStringProperty(const std::string& propertyName, std::string defaultValue = std::string()) const;
    _NODISCARD bool ReadBoolProperty(const std::string& propertyName, bool defaultValue = false) const;
    _NODISCARD int ReadIntegerProperty(const std::string& propertyName, int defaultValue = 0) const;
    _NODISCARD float ReadFloatProperty(const std::string& propertyName, float defaultValue = 0.0f) const;
    _NODISCARD GdtVec2 ReadVec2Property(const std::string& propertyName, GdtVec2 defaultValue = {}) const;
    _NODISCARD GdtVec3 ReadVec3Property(const std::string& propertyName, GdtVec3 defaultValue = {}) const;
    _NODISCARD GdtVec4 ReadVec4Property(const std::string& propertyName, GdtVec4 defaultValue = {}) const;

    const GdtEntry& m_entry;
};

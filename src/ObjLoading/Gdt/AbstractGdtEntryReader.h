#pragma once

#include "Obj/Gdt/GdtEntry.h"

#include <exception>

class GdtReadingException : public std::exception
{
public:
    explicit GdtReadingException(std::string message);
    [[nodiscard]] const char* what() const noexcept override;

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

    [[nodiscard]] std::string ReadStringProperty(const std::string& propertyName, std::string defaultValue = std::string()) const;
    [[nodiscard]] bool ReadBoolProperty(const std::string& propertyName, bool defaultValue = false) const;
    [[nodiscard]] int ReadIntegerProperty(const std::string& propertyName, int defaultValue = 0) const;
    [[nodiscard]] float ReadFloatProperty(const std::string& propertyName, float defaultValue = 0.0f) const;
    [[nodiscard]] GdtVec2 ReadVec2Property(const std::string& propertyName, GdtVec2 defaultValue = {}) const;
    [[nodiscard]] GdtVec3 ReadVec3Property(const std::string& propertyName, GdtVec3 defaultValue = {}) const;
    [[nodiscard]] GdtVec4 ReadVec4Property(const std::string& propertyName, GdtVec4 defaultValue = {}) const;

    const GdtEntry& m_entry;
};

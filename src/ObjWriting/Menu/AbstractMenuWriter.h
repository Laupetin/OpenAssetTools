#pragma once

#include "IMenuWriter.h"

#include <cstddef>
#include <ostream>
#include <string>
#include <vector>

namespace menu
{
    class AbstractBaseWriter : public IWriter
    {
    protected:
        static constexpr auto MENU_KEY_SPACING = 28u;
        static const inline std::string BOOL_VALUE_TRUE = "1";
        static const inline std::string BOOL_VALUE_FALSE = "0";
        static constexpr inline float COLOR_0000[4]{0.0f, 0.0f, 0.0f, 0.0f};
        static constexpr inline float COLOR_1111[4]{1.0f, 1.0f, 1.0f, 1.0f};

    public:
        void Start() override;
        void End() override;

        void IncludeMenu(const std::string& menuPath) const override;

    protected:
        explicit AbstractBaseWriter(std::ostream& stream);

        void IncIndent();
        void DecIndent();
        void Indent() const;

        void StartScope(const std::string& scopeName);
        void StartMenuDefScope();
        void StartItemDefScope();
        void StartFunctionDefScope();
        void EndScope();

        static std::vector<std::string> CreateScriptTokenList(const char* script);
        static bool DoesTokenNeedQuotationMarks(const std::string& token);

        void WriteEscapedString(const std::string_view& str) const;

        static const std::string& BoolValue(bool value);
        void WriteKey(const std::string& keyName) const;
        void WriteStringProperty(const std::string& propertyKey, const std::string& propertyValue) const;
        void WriteStringProperty(const std::string& propertyKey, const char* propertyValue) const;
        void WriteBoolProperty(const std::string& propertyKey, bool propertyValue, bool defaultValue) const;
        void WriteIntProperty(const std::string& propertyKey, int propertyValue, int defaultValue) const;
        void WriteFloatProperty(const std::string& propertyKey, float propertyValue, float defaultValue) const;
        void WriteColorProperty(const std::string& propertyKey, const float (&propertyValue)[4], const float (&defaultValue)[4]) const;
        void WriteKeywordProperty(const std::string& propertyKey, bool shouldWrite) const;
        void WriteFlagsProperty(const std::string& propertyKey, int flagsValue) const;

        std::ostream& m_stream;
        size_t m_indent;
    };
} // namespace menu

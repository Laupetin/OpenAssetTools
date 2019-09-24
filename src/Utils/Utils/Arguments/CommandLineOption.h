#pragma once
#include <string>
#include <vector>

class CommandLineOption
{
protected:
    CommandLineOption();

public:
    std::string m_short_name;
    std::string m_long_name;
    std::string m_description;
    std::string m_category;
    bool m_multi_use;
    std::vector<std::string> m_parameters;

    class Builder;
};

class CommandLineOption::Builder
{
    CommandLineOption m_option;

public:
    static Builder& Create();

    Builder& WithShortName(std::string shortName);
    Builder& WithLongName(std::string longName);
    Builder& WithDescription(std::string description);
    Builder& WithCategory(std::string category);
    Builder& WithParameter(std::string parameterName);
    Builder& Reusable();
    CommandLineOption* Build() const;
};
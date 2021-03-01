#include "CommandLineOption.h"

CommandLineOption::CommandLineOption()
{
    m_short_name = "";
    m_long_name = "";
    m_description = "";
    m_category = "";
    m_multi_use = false;
}

CommandLineOption::Builder& CommandLineOption::Builder::Create()
{
    return *new Builder();
}

CommandLineOption::Builder& CommandLineOption::Builder::WithShortName(std::string shortName)
{
    m_option.m_short_name = std::move(shortName);
    return *this;
}

CommandLineOption::Builder& CommandLineOption::Builder::WithLongName(std::string longName)
{
    m_option.m_long_name = std::move(longName);
    return *this;
}

CommandLineOption::Builder& CommandLineOption::Builder::WithDescription(std::string description)
{
    m_option.m_description = std::move(description);
    return *this;
}

CommandLineOption::Builder& CommandLineOption::Builder::WithCategory(std::string category)
{
    m_option.m_category = std::move(category);
    return *this;
}

CommandLineOption::Builder& CommandLineOption::Builder::WithParameter(std::string parameterName)
{
    m_option.m_parameters.push_back(std::move(parameterName));
    return *this;
}

CommandLineOption::Builder& CommandLineOption::Builder::Reusable()
{
    m_option.m_multi_use = true;
    return *this;
}

CommandLineOption* CommandLineOption::Builder::Build() const
{
    auto* result = new CommandLineOption(m_option);
    delete this;

    return result;
}

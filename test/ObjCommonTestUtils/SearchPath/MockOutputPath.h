#pragma once

#include "SearchPath/IOutputPath.h"

#include <sstream>
#include <string>
#include <vector>

class MockOutputFile
{
public:
    std::string m_name;
    std::string m_data;

    MockOutputFile();
    MockOutputFile(std::string name, std::string data);
};

class MockOutputPath final : public IOutputPath
{
public:
    std::unique_ptr<std::ostream> Open(const std::string& fileName) override;

    [[nodiscard]] const MockOutputFile* GetMockedFile(const std::string& name) const;

private:
    std::vector<MockOutputFile> m_files;
};

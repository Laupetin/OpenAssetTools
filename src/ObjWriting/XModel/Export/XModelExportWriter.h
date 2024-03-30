#pragma once

#include "XModel/AbstractXModelWriter.h"

#include <memory>
#include <ostream>

class XModelExportWriter : public AbstractXModelWriter
{
public:
    XModelExportWriter() = default;
    virtual ~XModelExportWriter() = default;
    XModelExportWriter(const XModelExportWriter& other) = default;
    XModelExportWriter(XModelExportWriter&& other) noexcept = default;
    XModelExportWriter& operator=(const XModelExportWriter& other) = default;
    XModelExportWriter& operator=(XModelExportWriter&& other) noexcept = default;

    virtual void Write(std::ostream& stream) = 0;

    static std::unique_ptr<XModelExportWriter> CreateWriterForVersion6(std::string gameName, std::string zoneName);
};

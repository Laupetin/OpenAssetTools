#pragma once

#include "Parsing/IPackValueSupplier.h"
#include "Parsing/IParserLineStream.h"
#include "Parsing/PostProcessing/IPostProcessor.h"
#include "Persistence/IDataRepository.h"
#include "ZoneCodeGeneratorArguments.h"

#include <string>

class HeaderFileReader
{
public:
    HeaderFileReader(const ZoneCodeGeneratorArguments* args, std::string filename);

    bool ReadHeaderFile(IDataRepository* repository);

private:
    bool OpenBaseStream();
    void SetupStreamProxies();
    void SetupPostProcessors();

    const ZoneCodeGeneratorArguments* m_args;
    std::string m_filename;

    std::vector<std::unique_ptr<IParserLineStream>> m_open_streams;
    const IPackValueSupplier* m_pack_value_supplier;
    IParserLineStream* m_stream;

    std::vector<std::unique_ptr<IPostProcessor>> m_post_processors;
};

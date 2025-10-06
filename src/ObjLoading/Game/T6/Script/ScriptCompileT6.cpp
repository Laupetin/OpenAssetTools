#include "ScriptCompileT6.h"

#include <xsk/arc/engine/t6_pc.hpp>


char* compileScriptT6(const std::string& gscName, char* gscCode, size_t gscCodeSize, size_t* out_CompiledSize)
{
    try
    {
        xsk::arc::t6::pc::context context = xsk::arc::t6::pc::context(xsk::arc::instance::server);

        std::vector<xsk::u8> scriptBuffer;
        scriptBuffer.resize(gscCodeSize);
        memcpy(&scriptBuffer[0], gscCode, gscCodeSize);

        auto outasm = context.compiler().compile(gscName, scriptBuffer);
        auto outbin = context.assembler().assemble(*outasm);

        char* compiledBuffer = new char[outbin.first.size];
        memcpy(compiledBuffer, outbin.first.data, outbin.first.size);

        *out_CompiledSize = outbin.first.size;
        return compiledBuffer;

    }
    catch (std::exception const& e)
    {
        printf(std::format("GSC Compile failed: {}\n", e.what()).c_str());
        return NULL;
    }
}
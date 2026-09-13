#include "Loading/ILoadingStream.h"
#include "Utils/MemoryManager.h"
#include "Zone/Stream/ZoneInputStream.h"
#include "Zone/XBlock.h"

#include <catch2/catch_test_macros.hpp>
#include <cstdint>
#include <cstring>
#include <memory>
#include <optional>
#include <vector>

namespace
{
    class ZeroLoadingStream final : public ILoadingStream
    {
    public:
        size_t Load(void* buffer, const size_t length) override
        {
            std::memset(buffer, 0, length);
            m_position += static_cast<int64_t>(length);
            return length;
        }

        int64_t Pos() override
        {
            return m_position;
        }

    private:
        int64_t m_position = 0;
    };

    TEST_CASE("Inserted pointer aliases resolve to their registered value", "[zone-loading][stream]")
    {
        XBlock block("test", 0, XBlockType::BLOCK_TYPE_NORMAL);
        block.Alloc(2 * sizeof(void*));
        std::vector<XBlock*> blocks{&block};

        ZeroLoadingStream loadingStream;
        MemoryManager memory;
        auto stream = ZoneInputStream::Create(sizeof(void*) * 8u, 4u, blocks, 0, loadingStream, memory, std::nullopt, sizeof(void*) * 8u);

        const auto lookup = stream->InsertPointerAliasLookup();
        int expected = 0;
        stream->SetInsertedPointerAliasLookup(lookup, &expected);

        auto* alias = reinterpret_cast<void*>(lookup + 1u);
        REQUIRE(stream->ResolveOffsetToAliasNative(&alias));
        REQUIRE(alias == &expected);
    }
} // namespace

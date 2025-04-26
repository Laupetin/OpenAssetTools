#pragma once

#include "Loading/ILoadingStream.h"
#include "Zone/Stream/IZoneInputStream.h"
#include "Zone/XBlock.h"

#include <memory>
#include <stack>
#include <vector>

class XBlockInputStream final : public IZoneInputStream
{
public:
    XBlockInputStream(std::vector<XBlock*>& blocks, ILoadingStream* stream, int blockBitCount, block_t insertBlock);

    void PushBlock(block_t block) override;
    block_t PopBlock() override;

    void* Alloc(unsigned align) override;

    void LoadDataRaw(void* dst, size_t size) override;
    void LoadDataInBlock(void* dst, size_t size) override;
    void IncBlockPos(size_t size) override;
    void LoadNullTerminated(void* dst) override;

    void** InsertPointer() override;

    void* ConvertOffsetToPointer(const void* offset) override;
    void* ConvertOffsetToAlias(const void* offset) override;

private:
    void Align(unsigned align);

    std::vector<XBlock*>& m_blocks;
    std::unique_ptr<size_t[]> m_block_offsets;

    std::stack<XBlock*> m_block_stack;
    std::stack<size_t> m_temp_offsets;
    ILoadingStream* m_stream;

    unsigned m_block_bit_count;
    XBlock* m_insert_block;
};

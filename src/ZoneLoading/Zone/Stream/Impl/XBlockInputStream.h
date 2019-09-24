#pragma once

#include "Zone/Stream/IZoneInputStream.h"
#include "Zone/XBlock.h"
#include "Loading/ILoadingStream.h"

#include <vector>
#include <stack>

class XBlockInputStream final : public IZoneInputStream
{
    std::vector<XBlock*>& m_blocks;
    size_t* m_block_offsets;
    unsigned int* m_block_in_stack;

    std::stack<XBlock*> m_block_stack;
    ILoadingStream* m_stream;

    int m_block_bit_count;
    XBlock* m_insert_block;

    void Align(int align);

public:
    XBlockInputStream(std::vector<XBlock*>& blocks, ILoadingStream* stream, int blockBitCount, block_t insertBlock);
    ~XBlockInputStream() override;

    void PushBlock(block_t block) override;
    block_t PopBlock() override;

    void* Alloc(int align) override;

    void LoadData(size_t size) override;
    void LoadNullTerminated() override;

    void** InsertPointer() override;

    void* ConvertOffsetToPointer(const void* offset) override;
    void* ConvertOffsetToAlias(const void* offset) override;
};

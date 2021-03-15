#pragma once
#include <stack>
#include <unordered_map>
#include <vector>

#include "Writing/InMemoryZoneData.h"
#include "Zone/XBlock.h"
#include "Zone/Stream/IZoneOutputStream.h"

class InMemoryZoneOutputStream final : public IZoneOutputStream
{
    static constexpr void* PTR_FOLLOWING = reinterpret_cast<void*>(-1);
    static constexpr void* PTR_INSERT = reinterpret_cast<void*>(-2);

    class ReusableEntry
    {
    public:
        void* m_start_ptr;
        void* m_end_ptr;
        uintptr_t m_start_zone_ptr;
        size_t m_entry_size;
        size_t m_entry_count;

        ReusableEntry(void* startPtr, size_t entrySize, size_t entryCount, uintptr_t startZonePtr);
    };

    InMemoryZoneData* m_zone_data;
    std::vector<XBlock*> m_blocks;

    std::stack<XBlock*> m_block_stack;
    std::stack<size_t> m_temp_sizes;

    int m_block_bit_count;
    XBlock* m_insert_block;

    std::unordered_map<std::type_index, std::vector<ReusableEntry>> m_reusable_entries;

    uintptr_t GetCurrentZonePointer();
    uintptr_t InsertPointer();

public:
    InMemoryZoneOutputStream(InMemoryZoneData* zoneData, std::vector<XBlock*> blocks, int blockBitCount, block_t insertBlock);

    void PushBlock(block_t block) override;
    block_t PopBlock() override;
    void Align(int align) override;
    void* WriteDataRaw(void* src, size_t size) override;
    void* WriteDataInBlock(void* src, size_t size) override;
    void IncBlockPos(size_t size) override;
    void WriteNullTerminated(void* src) override;
    bool ReusableShouldWrite(void** pPtr, size_t entrySize, size_t entryCount, std::type_index type) override;
};

#pragma once

#include <istream>

#include "Utils/ObjStream.h"
#include "IPakStreamManager.h"
#include "ObjContainer/IPak/IPakTypes.h"

class IPakEntryReadStream final : public objbuf
{
    static constexpr size_t IPAK_DECOMPRESS_BUFFER_SIZE = 0x8000;

    uint8_t* m_chunk_buffer;

    std::istream& m_stream;
    IPakStreamManagerActions* m_stream_manager_actions;

    int64_t m_file_offset;
    int64_t m_file_head;

    size_t m_entry_size;

    uint8_t m_decompress_buffer[IPAK_DECOMPRESS_BUFFER_SIZE];
    IPakDataBlockHeader* m_current_block;
    unsigned m_next_command;
    uint8_t* m_current_command_buffer;
    size_t m_current_command_length;
    size_t m_current_command_offset;

    int64_t m_pos;
    int64_t m_base_pos;
    int64_t m_end_pos;
    int64_t m_buffer_start_pos;
    int64_t m_buffer_end_pos;

    template <typename T>
    static T AlignForward(const T num, const T alignTo)
    {
        return (num + alignTo - 1) / alignTo * alignTo;
    }

    template <typename T>
    static T AlignBackwards(const T num, const T alignTo)
    {
        return num / alignTo * alignTo;
    }

    /**
     * \brief Reads the specified chunks from disk.
     * \param buffer The location to write the loaded data to. Must be able to hold the specified amount of data.
     * \param startPos The file offset at which the data to be loaded starts at.
     * \param chunkCount The amount of chunks to be loaded.
     * \return The amount of chunks that could be successfully loaded.
     */
    size_t ReadChunks(uint8_t* buffer, int64_t startPos, size_t chunkCount) const;

    /**
     * \brief Make sure the loaded chunk buffer window corresponds to the specified parameters and loads additional data if necessary.
     * \param startPos The offset in the file that should be the start of the chunk buffer.
     * \param chunkCount The amount of chunks that the buffer should hold. Can not exceed IPAK_CHUNK_COUNT_PER_READ.
     * \return \c true if the chunk buffer window could successfully be adjusted, \c false otherwise.
     */
    bool SetChunkBufferWindow(int64_t startPos, size_t chunkCount);

    /**
     * \brief Makes sure the specified block can be safely loaded.
     * \param blockHeader The block header to check.
     * \return \c true if the block can be safely loaded, \c false otherwise.
     */
    bool ValidateBlockHeader(const IPakDataBlockHeader* blockHeader) const;

    /**
     * \brief Makes sure that the specified block fits inside the loaded chunk buffer window and adjusts the chunk buffer window if necessary.
     * \param blockHeader The header of the block that needs to fit inside the loaded chunk buffer window.
     * \param blockOffsetInChunk The offset of the block inside the current chunk.
     * \return \c true if the chunk buffer window was either already valid or was successfully adjusted to have all block data loaded. \c false otherwise.
     */
    bool AdjustChunkBufferWindowForBlockHeader(const IPakDataBlockHeader* blockHeader, size_t blockOffsetInChunk);

    /**
     * \brief Ensures the next valid block is loaded.
     * \return \c true if a new block was loaded or \c false if no further valid block could be loaded.
     */
    bool NextBlock();

    /**
     * \brief Processes a command with the specified parameters at the current position.
     * \param commandSize The size of the command data
     * \param compressed The compression value of the command. Can be \c 0 for uncompressed or \c 1 for lzo compression. Any other value skips the specified size of data.
     * \return \c true if the specified command could be correctly processed or \c otherwise.
     */
    bool ProcessCommand(size_t commandSize, int compressed);

    /**
     * \brief Ensures that the next valid command is loaded.
     * \return \c true if a new command was loaded or \c false if no further valid command could be loaded.
     */
    bool AdvanceStream();

public:
    IPakEntryReadStream(std::istream& stream, IPakStreamManagerActions* streamManagerActions, uint8_t* chunkBuffer, int64_t startOffset, size_t entrySize);
    ~IPakEntryReadStream() override;

    _NODISCARD bool is_open() const override;
    bool close() override;

protected:
    std::streamsize showmanyc() override;
    int_type underflow() override;
    int_type uflow() override;
    std::streamsize xsgetn(char* ptr, std::streamsize count) override;
    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode mode) override;
    pos_type seekpos(pos_type pos, std::ios_base::openmode mode) override;
};

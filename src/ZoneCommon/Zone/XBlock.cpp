#include "XBlock.h"

#include <cassert>

XBlock::XBlock(const std::string& name, const int index, const Type type)
{
    m_name = name;
    m_index = index;
    m_type = type;
    m_buffer = nullptr;
    m_buffer_size = 0;
}

XBlock::~XBlock()
{
    delete[] m_buffer;
    m_buffer = nullptr;
}

void XBlock::Alloc(const size_t blockSize)
{
    delete[] m_buffer;

    if (blockSize > 0)
    {
        m_buffer = new uint8_t[blockSize];
        m_buffer_size = blockSize;
    }
    else
    {
        m_buffer = nullptr;
        m_buffer_size = 0;
    }
}

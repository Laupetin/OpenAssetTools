// ReSharper disable CppInconsistentNaming
// ReSharper disable IdentifierTypo
#pragma once

#include "Utils/ClassUtils.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <streambuf>

template<class Elem, class Traits> class basic_objbuf : public std::basic_streambuf<Elem, Traits>
{
public:
    _NODISCARD virtual bool is_open() const = 0;
    virtual bool close() = 0;
};

template<class Elem, class Traits> class basic_objstream : public std::basic_iostream<Elem, Traits>
{
public:
    using mybase = std::basic_iostream<Elem, Traits>;
    using myob = basic_objbuf<Elem, Traits>;
    using myios = std::basic_ios<Elem, Traits>;

    explicit basic_objstream(std::unique_ptr<myob> objbuf)
        : std::basic_iostream<Elem, Traits>(objbuf.get()),
          m_ob(std::move(objbuf))
    {
        assert(m_ob);
    }

    explicit basic_objstream(basic_objstream<Elem, Traits>&& right) noexcept
        : std::basic_iostream<Elem, Traits>(right),
          m_ob(std::move(right.m_ob))
    {
        assert(m_ob != nullptr);
    }

    basic_objstream(const basic_objstream& other) = delete;
    basic_objstream& operator=(const basic_objstream& other) = delete;

    ~basic_objstream() override
    {
        if (m_ob)
            m_ob->close();
    }

    void swap(basic_objstream& right) noexcept
    {
        if (this != std::addressof(right))
        {
            mybase::swap(right);
            m_ob = std::move(right.m_ob);
        }
    }

    basic_objstream& operator=(basic_objstream&& other) noexcept
    {
        swap(other);
        return *this;
    }

    // ReSharper disable once CppHidingFunction
    _NODISCARD myob* rdbuf() const
    {
        return const_cast<myob*>(m_ob.get());
    }

    _NODISCARD bool is_open() const
    {
        return m_ob->is_open();
    }

    void close()
    {
        if (!m_ob->close())
        {
            myios::setstate(std::ios_base::failbit);
        }
    }

protected:
    std::unique_ptr<myob> m_ob;
};

template<class Elem, class Traits> class basic_iobjstream : public std::basic_istream<Elem, Traits>
{
public:
    using mybase = std::basic_istream<Elem, Traits>;
    using myob = basic_objbuf<Elem, Traits>;
    using myios = std::basic_ios<Elem, Traits>;

    explicit basic_iobjstream(std::unique_ptr<myob> objbuf)
        : std::basic_istream<Elem, Traits>(objbuf.get()),
          m_ob(std::move(objbuf))
    {
        assert(m_ob);
    }

    explicit basic_iobjstream(basic_iobjstream<Elem, Traits>&& right) noexcept
        : std::basic_istream<Elem, Traits>(right),
          m_ob(std::move(right.m_ob))
    {
        assert(m_ob != nullptr);
    }

    basic_iobjstream(const basic_iobjstream& other) = delete;
    basic_iobjstream& operator=(const basic_iobjstream& other) = delete;

    ~basic_iobjstream() override = default;

    void swap(basic_iobjstream& right) noexcept
    {
        if (this != std::addressof(right))
        {
            mybase::swap(right);
            m_ob = std::move(right.m_ob);
        }
    }

    basic_iobjstream& operator=(basic_iobjstream&& other) noexcept
    {
        swap(other);
        return *this;
    }

    // ReSharper disable once CppHidingFunction
    _NODISCARD myob* rdbuf() const
    {
        return const_cast<myob*>(m_ob.get());
    }

    _NODISCARD bool is_open() const
    {
        return m_ob->is_open();
    }

    void close()
    {
        if (!m_ob->close())
        {
            myios::setstate(std::ios_base::failbit);
        }
    }

protected:
    std::unique_ptr<myob> m_ob;
};

template<class Elem, class Traits> class basic_oobjstream : public std::basic_ostream<Elem, Traits>
{
public:
    using mybase = std::basic_ostream<Elem, Traits>;
    using myob = basic_objbuf<Elem, Traits>;
    using myios = std::basic_ios<Elem, Traits>;

    explicit basic_oobjstream(std::unique_ptr<myob> objbuf)
        : std::basic_ostream<Elem, Traits>(objbuf.get()),
          m_ob(std::move(objbuf))
    {
        assert(m_ob);
    }

    explicit basic_oobjstream(basic_oobjstream<Elem, Traits>&& right) noexcept
        : std::basic_ostream<Elem, Traits>(right),
          m_ob(std::move(right.m_ob))
    {
        assert(m_ob != nullptr);
    }

    ~basic_oobjstream() override = default;

    basic_oobjstream(const basic_oobjstream& other) = delete;
    basic_oobjstream& operator=(const basic_oobjstream& other) = delete;

    void swap(basic_oobjstream& right) noexcept
    {
        if (this != std::addressof(right))
        {
            mybase::swap(right);
            m_ob = std::move(right.m_ob);
        }
    }

    basic_oobjstream& operator=(basic_oobjstream&& other) noexcept
    {
        swap(other);
        return *this;
    }

    // ReSharper disable once CppHidingFunction
    _NODISCARD myob* rdbuf() const
    {
        return const_cast<myob*>(m_ob.get());
    }

    _NODISCARD bool is_open() const
    {
        return m_ob->is_open();
    }

    void close()
    {
        if (!m_ob->close())
        {
            myios::setstate(std::ios_base::failbit);
        }
    }

protected:
    std::unique_ptr<myob> m_ob;
};

using objbuf = basic_objbuf<char, std::char_traits<char>>;
using objstream = basic_objstream<char, std::char_traits<char>>;
using iobjstream = basic_iobjstream<char, std::char_traits<char>>;
using oobjstream = basic_oobjstream<char, std::char_traits<char>>;

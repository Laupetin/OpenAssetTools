// ReSharper disable CppHidingFunction
// ReSharper disable IdentifierTypo
// ReSharper disable CppInconsistentNaming
#pragma once

// #include <fstream>
//
// #include "ObjStream.h"
//
// template <class Elem, class Traits>
// class basic_fobjbuf final : public basic_objbuf<Elem, Traits>, public std::basic_filebuf<Elem, Traits>
// {
// public:
//     using myfb = std::basic_filebuf<Elem, Traits>;
//
//     explicit basic_fobjbuf(myfb* fb)
//     {
//         m_fb.swap(*fb);
//     }
//
//     _NODISCARD bool is_open() const override
//     {
//         return m_fb.is_open();
//     }
//
//     bool close() override
//     {
//         return m_fb.close();
//     }
//
// private:
//     myfb m_fb;
// };
//
// template <class Elem, class Traits>
// class basic_fobjstream final : public basic_objstream<Elem, Traits>
// {
// public:
//     using mybase = basic_objstream<Elem, Traits>;
//     using myfb = basic_fobjbuf<Elem, Traits>;
//
//     using mybase::m_ob;
//
//     explicit basic_fobjstream(std::fstream&& stream)
//         : mybase(std::make_unique<myfb>(std::move(*stream.rdbuf())))
//     {
//         // reinterpret_cast<myfb*>(m_ob.get())->swapp(*stream.rdbuf());
//         // reinterpret_cast<myfb*>(m_ob.get())->swap(*stream.rdbuf());
//     }
//
//     basic_fobjstream(const basic_fobjstream& other) = delete;
//     basic_fobjstream(basic_fobjstream&& other) noexcept = default;
//     basic_fobjstream& operator=(const basic_fobjstream& other) = delete;
//     basic_fobjstream& operator=(basic_fobjstream&& other) noexcept = default;
// };
//
// template <class Elem, class Traits>
// class basic_ifobjstream final : public basic_iobjstream<Elem, Traits>
// {
// public:
//     using mybase = basic_iobjstream<Elem, Traits>;
//     using myfb = basic_fobjbuf<Elem, Traits>;
//
//     using mybase::m_ob;
//
//     explicit basic_ifobjstream(std::ifstream&& stream)
//         : mybase(std::make_unique<myfb>(std::move(*stream.rdbuf())))
//     {
//         // reinterpret_cast<myfb*>(m_ob.get())->swapp(*stream.rdbuf());
//         // m_ob->swapp(*stream.rdbuf());
//     }
//     ~basic_ifobjstream() = default;
//
//     basic_ifobjstream(const basic_ifobjstream& other) = delete;
//     basic_ifobjstream(basic_ifobjstream&& other) noexcept = default;
//     basic_ifobjstream& operator=(const basic_ifobjstream& other) = delete;
//     basic_ifobjstream& operator=(basic_ifobjstream&& other) noexcept = default;
// };
//
// template <class Elem, class Traits>
// class basic_ofobjstream final : public basic_oobjstream<Elem, Traits>
// {
// public:
//     using mybase = basic_oobjstream<Elem, Traits>;
//     using myfb = basic_fobjbuf<Elem, Traits>;
//
//     using mybase::m_ob;
//
//     explicit basic_ofobjstream(std::ofstream file)
//         : mybase(std::make_unique<myfb>(file.rdbuf()))
//     {
//     }
//
//     basic_ofobjstream(const basic_ofobjstream& other) = delete;
//     basic_ofobjstream(basic_ofobjstream&& other) noexcept = default;
//     basic_ofobjstream& operator=(const basic_ofobjstream& other) = delete;
//     basic_ofobjstream& operator=(basic_ofobjstream&& other) noexcept = default;
// };
//
// using fobjbuf = basic_fobjbuf<char, std::char_traits<char>>;
// using fobjstream = basic_fobjstream<char, std::char_traits<char>>;
// using ifobjstream = basic_ifobjstream<char, std::char_traits<char>>;
// using ofobjstream = basic_ofobjstream<char, std::char_traits<char>>;
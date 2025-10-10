#pragma once

#include <type_traits>
#include <variant>

using NoResult = std::monostate;

// Can be replaced by std::expected with c++23
template<typename TResult, typename TError> class Result
{
public:
    Result(TResult result)
        : m_data(std::variant<TResult, TError>(std::in_place_index<0>, std::move(result)))
    {
    }

    static Result Ok(TResult result)
    {
        return Result(std::in_place_index<0>, std::move(result));
    }

    static Result Bad(TError error)
    {
        return Result(std::in_place_index<1>, std::move(error));
    }

    constexpr operator bool() const noexcept
    {
        return m_data.index() == 0;
    }

    constexpr bool has_value() const noexcept
    {
        return m_data.index() == 0;
    }

    constexpr std::add_lvalue_reference_t<TResult> value() &
    {
        return std::get<0, TResult>(m_data);
    }

    constexpr std::add_const_t<std::add_lvalue_reference_t<TResult>> value() const&
    {
        return std::get<0, TResult>(m_data);
    }

    constexpr std::add_rvalue_reference_t<TResult> value() &&
    {
        return std::move(std::get<0, TResult>(m_data));
    }

    constexpr std::add_const_t<std::add_rvalue_reference_t<TResult>> value() const&&
    {
        return std::move(std::get<0, TResult>(m_data));
    }

    constexpr std::add_lvalue_reference_t<TResult> operator*() &
    {
        return std::get<0, TResult>(m_data);
    }

    constexpr std::add_const_t<std::add_lvalue_reference_t<TResult>> operator*() const&
    {
        return std::get<0, TResult>(m_data);
    }

    constexpr std::add_rvalue_reference_t<TResult> operator*() &&
    {
        return std::move(std::get<0, TResult>(m_data));
    }

    constexpr std::add_const_t<std::add_rvalue_reference_t<TResult>> operator*() const&&
    {
        return std::move(std::get<0, TResult>(m_data));
    }

    constexpr std::add_pointer_t<TResult> operator->()
    {
        return std::get<0, TResult>(m_data);
    }

    constexpr std::add_const_t<std::add_pointer_t<TResult>> operator->() const
    {
        return std::get<0, TResult>(m_data);
    }

    constexpr std::add_lvalue_reference_t<TError> error() &
    {
        return std::get<1, TError>(m_data);
    }

    constexpr std::add_const_t<std::add_lvalue_reference_t<TError>> error() const&
    {
        return std::get<1, TError>(m_data);
    }

    constexpr std::add_rvalue_reference_t<TError> error() &&
    {
        return std::move(std::get<1, TError>(m_data));
    }

    constexpr std::add_const_t<std::add_rvalue_reference_t<TError>> error() const&&
    {
        return std::move(std::get<1, TError>(m_data));
    }

private:
    explicit Result(std::variant<TResult, TError> data)
        : m_data(std::move(data))
    {
    }

    std::variant<TResult, TError> m_data;
};

#define ENSURE_RESULT_VAR(var)                                                                                                                                 \
    if (!(var))                                                                                                                                                \
        return (var);
#define ENSURE_RESULT(expr)                                                                                                                                    \
    {                                                                                                                                                          \
        const auto result = (expr);                                                                                                                            \
        if (!result)                                                                                                                                           \
            return result;                                                                                                                                     \
    }

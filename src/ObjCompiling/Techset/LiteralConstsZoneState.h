#pragma once

#include "Asset/IZoneAssetCreationState.h"

#include <array>
#include <concepts>
#include <unordered_map>

namespace techset
{
    template<std::floating_point T> class LiteralConst
    {
    public:
        using game_ptr_t = const T (*)[4];

        LiteralConst() = default;

        LiteralConst(T v0, T v1, T v2, T v3)
            : m_value({v0, v1, v2, v3})
        {
        }

        explicit LiteralConst(std::array<T, 4> value)
            : m_value(value)
        {
        }

        friend bool operator<(const LiteralConst& lhs, const LiteralConst& rhs)
        {
            if (lhs.m_value[0] < rhs.m_value[0])
                return true;
            if (lhs.m_value[0] > rhs.m_value[0])
                return false;
            if (lhs.m_value[1] < rhs.m_value[1])
                return true;
            if (lhs.m_value[1] > rhs.m_value[1])
                return false;
            if (lhs.m_value[2] < rhs.m_value[2])
                return true;
            if (lhs.m_value[2] > rhs.m_value[2])
                return false;
            if (lhs.m_value[3] < rhs.m_value[3])
                return true;
            if (lhs.m_value[3] > rhs.m_value[3])
                return false;
            return false;
        }

        friend bool operator==(const LiteralConst& lhs, const LiteralConst& rhs)
        {
            return lhs.m_value == rhs.m_value;
        }

        friend bool operator!=(const LiteralConst& lhs, const LiteralConst& rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<=(const LiteralConst& lhs, const LiteralConst& rhs)
        {
            return !(rhs < lhs);
        }

        friend bool operator>(const LiteralConst& lhs, const LiteralConst& rhs)
        {
            return rhs < lhs;
        }

        friend bool operator>=(const LiteralConst& lhs, const LiteralConst& rhs)
        {
            return !(lhs < rhs);
        }

        game_ptr_t GamePtr() const
        {
            return reinterpret_cast<game_ptr_t>(m_value.data());
        }

        std::array<T, 4> m_value;
    };

    template<std::floating_point T> struct LiteralConstHash
    {
        std::size_t operator()(const LiteralConst<T>& v) const noexcept
        {
            std::size_t seed = 0x1DDB76B3;
            seed ^= (seed << 6) + (seed >> 2) + 0x0354144B + std::hash<T>()(v.m_value[0]);
            seed ^= (seed << 6) + (seed >> 2) + 0x66847B5C + std::hash<T>()(v.m_value[1]);
            seed ^= (seed << 6) + (seed >> 2) + 0x77399D60 + std::hash<T>()(v.m_value[2]);
            seed ^= (seed << 6) + (seed >> 2) + 0x477AF9AB + std::hash<T>()(v.m_value[3]);
            return seed;
        }
    };

    template<std::floating_point T> class LiteralConstsZoneState : public IZoneAssetCreationState
    {
    public:
        using literal_t = LiteralConst<T>;

        LiteralConstsZoneState()
            : m_memory(nullptr)
        {
        }

        void Inject(ZoneAssetCreationInjection& inject) override
        {
            m_memory = &inject.m_zone.Memory();
        }

        literal_t* GetAllocatedLiteral(const literal_t& literal)
        {
            const auto& existingEntry = m_allocated_literals.find(literal);

            if (existingEntry != m_allocated_literals.end())
                return existingEntry->second;

            auto* newLiteral = m_memory->Alloc<literal_t>();
            newLiteral->m_value[0] = literal.m_value[0];
            newLiteral->m_value[1] = literal.m_value[1];
            newLiteral->m_value[2] = literal.m_value[2];
            newLiteral->m_value[3] = literal.m_value[3];
            m_allocated_literals.emplace(literal, newLiteral);

            return newLiteral;
        }

    private:
        std::unordered_map<literal_t, literal_t*, LiteralConstHash<T>> m_allocated_literals;
        MemoryManager* m_memory;
    };
} // namespace techset

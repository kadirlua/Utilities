#pragma once

#include <type_traits>

#define ENABLE_BITMASK_OPERATORS(x)  \
template<>                           \
struct is_bitmask_enum<x> {   \
    static const bool enable = true; \
};

template<typename Enum>
struct is_bitmask_enum {
    static const bool enable = false;
};

template<class Enum>
inline constexpr bool is_bitmask_enum_v = is_bitmask_enum<Enum>::enable;

// ----- Bitwise operators ----------------------------------------------------

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator |(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
        );
}

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator &(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs)
        );
}

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator ^(Enum lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    return static_cast<Enum> (
        static_cast<underlying>(lhs) ^
        static_cast<underlying>(rhs)
        );
}

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator ~(Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    return static_cast<Enum> (
        ~static_cast<underlying>(rhs)
        );
}

// ----- Bitwise assignment operators -----------------------------------------

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator |=(Enum& lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    lhs = static_cast<Enum> (
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
        );
    return lhs;
}

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator &=(Enum& lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    lhs = static_cast<Enum> (
        static_cast<underlying>(lhs) &
        static_cast<underlying>(rhs)
        );
    return lhs;
}

template<typename Enum>
typename std::enable_if_t<is_bitmask_enum_v<Enum>, Enum>
inline constexpr operator ^=(Enum& lhs, Enum rhs) noexcept {
    using underlying = typename std::underlying_type_t<Enum>;
    lhs = static_cast<Enum> (
        static_cast<underlying>(lhs) ^
        static_cast<underlying>(rhs)
        );
    return lhs;
}

// ----- Bitwise mask checks --------------------------------------------------

template<typename Enum>
struct BitmaskEnum {
    const Enum value;
    static const Enum none = static_cast<Enum>(0);

    using underlying = typename std::underlying_type_t<Enum>;

    BitmaskEnum(Enum value) noexcept : value(value) {
        static_assert(is_bitmask_enum_v<Enum>);
    }

    // Convert back to enum if required
    inline constexpr operator Enum() const noexcept {
        return value;
    }

    // Convert to true if there is any bit set in the bitmask
    inline constexpr operator bool() const noexcept {
        return Any();
    }

    // Returns true if any bit is set
    inline constexpr bool Any() const noexcept {
        return value != none;
    }

    // Returns true if all bits are clear
    inline constexpr bool None() const noexcept {
        return value == none;
    }

    // Returns true if any bit in the given mask is set
    inline constexpr bool AnyOf(const Enum& mask) const noexcept {
        return (value & mask) != none;
    }

    // Returns true if all bits in the given mask are set
    inline constexpr bool AllOf(const Enum& mask) const noexcept {
        return (value & mask) == mask;
    }

    // Returns true if none of the bits in the given mask are set
    inline constexpr bool NoneOf(const Enum& mask) const noexcept {
        return (value & mask) == none;
    }

    // Returns true if any bits excluding the mask are set
    inline constexpr bool AnyExcept(const Enum& mask) const noexcept {
        return (value & ~mask) != none;
    }

    // Returns true if no bits excluding the mask are set
    inline constexpr bool NoneExcept(const Enum& mask) const noexcept {
        return (value & ~mask) == none;
    }
};
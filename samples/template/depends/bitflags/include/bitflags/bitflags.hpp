/*
 * Copyright (c) 2020, Marin Peko
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef BITFLAGS_HPP
#define BITFLAGS_HPP

#include <cstdint>
#include <utility>
#if __cplusplus >= 201703L
#include <string_view>
#endif

#if defined(__has_cpp_attribute)
#    if __has_cpp_attribute(nodiscard)
#        if __cplusplus >= 201703L
#            define NODISCARD [[nodiscard]]
#        endif
#    endif
#endif
#ifndef NODISCARD
#    define NODISCARD
#endif

#if __cplusplus >= 201402L
#    define NON_CONST_CONSTEXPR constexpr
#else
#    define NON_CONST_CONSTEXPR
#endif

namespace bf {

namespace internal {

/**
 * struct flag_helper
 *
 * Helper struct that contains comparison operators and
 * bitwise operators overloads used by any of the flag types.
 *
 * NOTE: This struct is for internal use only.
 */
template <typename FlagT>
struct flag_helper {
    constexpr flag_helper() = default;
    constexpr flag_helper(flag_helper&& rhs) = default;
    constexpr flag_helper(flag_helper const& rhs) = default;

    flag_helper& operator=(flag_helper&& rhs) = default;
    flag_helper& operator=(flag_helper const& rhs) = default;

    ~flag_helper() = default;

    /**
     * Comparison operators overloads
     *
     *     FlagT <op> FlagT
     */

    NODISCARD friend constexpr bool operator==(FlagT const& lhs, FlagT const& rhs) noexcept {
        return lhs.bits == rhs.bits;
    }

    NODISCARD friend constexpr bool operator!=(FlagT const& lhs, FlagT const& rhs) noexcept {
        return lhs.bits != rhs.bits;
    }

    /**
     * Bitwise operators overloads
     *
     *     <op> FlagT
     *
     *     FlagT <op>  FlagT
     */

    NODISCARD friend constexpr FlagT operator~(FlagT const& rhs) noexcept {
        return ~rhs.bits;
    }

    NODISCARD friend constexpr FlagT operator&(FlagT const& lhs, FlagT const& rhs) noexcept {
        return lhs.bits & rhs.bits;
    }

    NODISCARD friend constexpr FlagT operator|(FlagT const& lhs, FlagT const& rhs) noexcept {
        return lhs.bits | rhs.bits;
    }

    NODISCARD friend constexpr FlagT operator^(FlagT const& lhs, FlagT const& rhs) noexcept {
        return lhs.bits ^ rhs.bits;
    }
};

/**
 * struct raw_flag
 *
 * Flag type that does not contain string representation.
 *
 * NOTE: This struct is for internal use only.
 */
template <typename TagT, typename T = std::uint8_t>
struct raw_flag : flag_helper<raw_flag<TagT, T>> {
    T bits;

    constexpr raw_flag() noexcept
        : bits(0)
    {}

    constexpr raw_flag(T bits) noexcept
        : bits(bits)
    {}

    NODISCARD explicit constexpr operator T() const noexcept {
        return bits;
    }

    /**
     * Bitwise operators overloads
     *
     *     raw_flag <op>= raw_flag
     */

    NON_CONST_CONSTEXPR raw_flag& operator&=(raw_flag const& rhs) noexcept {
        bits &= rhs.bits;
        return *this;
    }

    NON_CONST_CONSTEXPR raw_flag& operator|=(raw_flag const& rhs) noexcept {
        bits |= rhs.bits;
        return *this;
    }

    NON_CONST_CONSTEXPR raw_flag& operator^=(raw_flag const& rhs) noexcept {
        bits ^= rhs.bits;
        return *this;
    }
};

/**
 * struct flag
 *
 * Flag type that contains string representation.
 *
 * NOTE: This struct is for internal use only.
 */
template <typename TagT, typename T = std::uint8_t>
struct flag : flag_helper<flag<TagT, T>> {
    T bits;
#if __cplusplus >= 201703L
    std::string_view name;
#else
    char const * name;
#endif

    constexpr flag() noexcept
        : bits(0)
    {}

    constexpr flag(T bits) noexcept
        : bits(bits)
    {}

#if __cplusplus >= 201703L
    constexpr flag(T bits, std::string_view name) noexcept
#else
    constexpr flag(T bits, char const * const name) noexcept
#endif
        : bits(bits)
        , name(name)
    {}

    NODISCARD explicit constexpr operator T() const noexcept {
        return bits;
    }

    /**
     * Bitwise operators overloads
     *
     *     flag <op>= flag
     */

    NON_CONST_CONSTEXPR flag& operator&=(flag const& rhs) noexcept {
        bits &= rhs.bits;
        return *this;
    }

    NON_CONST_CONSTEXPR flag& operator|=(flag const& rhs) noexcept {
        bits |= rhs.bits;
        return *this;
    }

    NON_CONST_CONSTEXPR flag& operator^=(flag const& rhs) noexcept {
        bits ^= rhs.bits;
        return *this;
    }
};

/**
 * struct min
 *
 * Provides member typedef type which is defined as minimal unsigned
 * integral type capable of storing N number of integers.
 *
 * NOTE: This struct is for internal use only.
 */
template <std::size_t N>
struct min {
#if __cplusplus >= 201402L
    template <bool B, typename T, typename U>
    using conditional_t = typename std::conditional_t<B, T, U>;
#else
    template <bool B, typename T, typename U>
    using conditional_t = typename std::conditional<B, T, U>::type;
#endif

    using type =
        conditional_t<
            (N <= 8), std::uint8_t,
            conditional_t<
                (N <= 16), std::uint16_t,
                conditional_t<
                    (N <= 32), std::uint32_t, std::uint64_t
                >
            >
        >;
};

template <std::size_t N>
using min_t = typename min<N>::type;

/**
 * Shifts integer of type T by specified offset.
 * If the offset is less than 0, 0 is returned.
 *
 * NOTE: This function is for internal use only.
 *
 * @param offset Offset to be used for shifting
 *
 * @return Result
 */
template <typename T>
constexpr T shift(int const offset) {
    return offset < 0
        ? static_cast<T>(0)
        : static_cast<T>(1U << offset);
}

} // internal

template <
    typename ImplT,
    typename T,
#if __cplusplus >= 201703L
    template <typename, typename> typename FlagT
#else
    template <typename, typename> class FlagT
#endif
>
class bitflags;

/**
 * Bitwise operators overloads
 *
 *     <op> bitflags<ImplT, T, FlagT>
 *
 *     bitflags<ImplT, T, FlagT> <op> FlagT<ImplT, T>
 */

template <
    typename ImplT,
    typename T,
#if __cplusplus >= 201703L
    template <typename, typename> typename FlagT
#else
    template <typename, typename> class FlagT
#endif
>
NODISCARD constexpr T operator~(bitflags<ImplT, T, FlagT> const& rhs) noexcept;

template <
    typename ImplT,
    typename T,
#if __cplusplus >= 201703L
    template <typename, typename> typename FlagT
#else
    template <typename, typename> class FlagT
#endif
>
NODISCARD constexpr T operator&(bitflags<ImplT, T, FlagT> const& lhs, FlagT<ImplT, T> const& rhs) noexcept;

template <
    typename ImplT,
    typename T,
#if __cplusplus >= 201703L
    template <typename, typename> typename FlagT
#else
    template <typename, typename> class FlagT
#endif
>
NODISCARD constexpr T operator|(bitflags<ImplT, T, FlagT> const& lhs, FlagT<ImplT, T> const& rhs) noexcept;

template <
    typename ImplT,
    typename T,
#if __cplusplus >= 201703L
    template <typename, typename> typename FlagT
#else
    template <typename, typename> class FlagT
#endif
>
NODISCARD constexpr T operator^(bitflags<ImplT, T, FlagT> const& lhs, FlagT<ImplT, T> const& rhs) noexcept;

/**
 * class bitflags
 *
 * Typesafe bitmask flag generator that manages set of flags.
 * Flags should be defined only for integral types.
 */
template <
    typename ImplT,
    typename T = internal::min_t<ImplT::end_ - ImplT::begin_ + 1>,
#if __cplusplus >= 201703L
    template <
        typename,
        typename
    > typename FlagT = internal::flag
#else
    template <
        typename,
        typename
    > class FlagT = internal::flag
#endif
>
class bitflags : public ImplT {
public:
    using underlying_type = T;

    constexpr bitflags() = default;
    constexpr bitflags(bitflags&& rhs) = default;
    constexpr bitflags(bitflags const& rhs) = default;

    constexpr bitflags(FlagT<ImplT, T>&& rhs) noexcept
        : curr_(std::move(rhs))
    {}

    constexpr bitflags(FlagT<ImplT, T> const& rhs) noexcept
        : curr_(rhs)
    {}

    constexpr bitflags(T bits) noexcept
        : curr_(bits)
    {}

#if __cplusplus >= 201703L
    constexpr bitflags(T bits, std::string_view name) noexcept
#else
    constexpr bitflags(T bits, char const * const name) noexcept
#endif
        : curr_(bits, name)
    {}

    bitflags& operator=(bitflags&& rhs) = default;
    bitflags& operator=(bitflags const& rhs) = default;

    bitflags& operator=(T bits) noexcept {
        curr_.bits = bits;
        curr_.name = "";
        return *this;
    }

    bitflags& operator=(FlagT<ImplT, T>&& rhs) noexcept {
        curr_ = std::move(rhs);
        return *this;
    }

    bitflags& operator=(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ = rhs;
        return *this;
    }

    ~bitflags() = default;

    NODISCARD constexpr operator T() const noexcept {
        return curr_.bits;
    }

    NODISCARD constexpr bool operator==(FlagT<ImplT, T> const& rhs) const noexcept {
        return curr_ == rhs;
    }

    NODISCARD constexpr bool operator!=(FlagT<ImplT, T> const& rhs) const noexcept {
        return curr_ != rhs;
    }

    /**
     * Bitwise operators overloads
     *
     *     <op> bitflags<ImplT, T, FlagT>
     *
     *     bitflags<ImplT, T, FlagT> <op>  FlagT<ImplT, T>
     *
     *     bitflags<ImplT, T, FlagT> <op>= FlagT<ImplT, T>
     */

    NODISCARD friend constexpr bitflags operator~(bitflags const& rhs) noexcept {
        return ~rhs.curr_;
    }

    NODISCARD friend constexpr bitflags operator&(bitflags const& lhs, FlagT<ImplT, T> const& rhs) noexcept {
        return lhs.curr_ & rhs;
    }

    NODISCARD friend constexpr bitflags operator|(bitflags const& lhs, FlagT<ImplT, T> const& rhs) noexcept {
        return lhs.curr_ | rhs;
    }

    NODISCARD friend constexpr bitflags operator^(bitflags const& lhs, FlagT<ImplT, T> const& rhs) noexcept {
        return lhs.curr_ ^ rhs;
    }

    NON_CONST_CONSTEXPR bitflags& operator&=(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ &= rhs;
        return *this;
    }

    NON_CONST_CONSTEXPR bitflags& operator|=(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ |= rhs;
        return *this;
    }

    NON_CONST_CONSTEXPR bitflags& operator^=(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ ^= rhs;
        return *this;
    }

    /**
     * Gets an underlying bits of current set of flags.
     *
     * @return Underlying bits
     */
    NODISCARD constexpr T bits() const noexcept {
        return curr_.bits;
    }

    /**
     * Gets an empty set of flags.
     *
     * @return Empty set of flags
     */
    NODISCARD static constexpr FlagT<ImplT, T> empty() noexcept {
        return T{};
    }

    /**
     * Gets the set of all defined flags.
     *
     * @return Set of all defined flags
     */
    NODISCARD static constexpr FlagT<ImplT, T> all() noexcept {
        return ~T{};
    }

    /**
     * Checks whether no flag is currently set.
     *
     * @return True if no flag is currently set, otherwise false
     */
    NODISCARD constexpr bool is_empty() const noexcept {
        return curr_ == T{};
    }

    /**
     * Checks whether all flags are currently set.
     *
     * @return True if all flags are currently set, otherwise false
     */
    NODISCARD constexpr bool is_all() const noexcept {
        return curr_ == ~T{};
    }

    /**
     * Checks whether specified flag is contained within the current
     * set of flags. Zero flags are treated as always present.
     *
     * @param rhs Flag to check
     *
     * @return True if the specified flags is contained within the
     *         current set of flags, otherwise false
     */
    NODISCARD constexpr bool contains(FlagT<ImplT, T> const& rhs) const noexcept {
        return static_cast<T>(curr_ & rhs) || rhs == empty();
    }

    /**
     * Checks whether all the specified flags are contained within the
     * current set of flags. Zero flags are treated as always present.
     *
     * @param rhs_1 First flag to check
     * @param rhs_n Other flags to check
     *
     * @return True if all the specified flags are contained within the
     *         current set of flags, otherwise false
     */
    template <typename ... U>
    NODISCARD constexpr bool contains(FlagT<ImplT, T> const& rhs_1, U const& ... rhs_n) const noexcept {
        return contains(rhs_1) && contains(rhs_n...);
    }

    /**
     * Sets specified flag.
     *
     * @param rhs Flag to be set
     */
    NON_CONST_CONSTEXPR void set(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ |= rhs;
    }

    /**
     * Unsets specified flag.
     *
     * @param rhs Flag to be unset
     */
    NON_CONST_CONSTEXPR void remove(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ &= ~rhs;
    }

    /**
     * Sets specified flag if not already present.
     * Otherwise, unsets the specified flag.
     *
     * @param rhs Flag to be toggled
     */
    NON_CONST_CONSTEXPR void toggle(FlagT<ImplT, T> const& rhs) noexcept {
        curr_ ^= rhs;
    }

    /**
     * Clears all flags currently set.
     */
    NON_CONST_CONSTEXPR void clear() noexcept {
        curr_ = T{};
    }

private:
    FlagT<ImplT, T> curr_;
};

} // bf

/**
 * Macros used for creating set of raw flags,
 * i.e. flags without string representation.
 */

#define BEGIN_RAW_BITFLAGS(NAME)                            \
    template <typename T>                                   \
    struct NAME##Impl {                                     \
        using flag = bf::internal::raw_flag<NAME##Impl, T>; \
        static constexpr int begin_ = __LINE__;

#define END_RAW_BITFLAGS(NAME)                                                   \
        static constexpr int end_   = __LINE__;                                  \
    };                                                                           \
    using NAME = bf::bitflags<                                                   \
        NAME##Impl< bf::bitflags< NAME##Impl<std::uint8_t> >::underlying_type >, \
        bf::bitflags< NAME##Impl<std::uint8_t> >::underlying_type,               \
        bf::internal::raw_flag                                                   \
    >;

#define RAW_FLAG(NAME) \
    static constexpr flag NAME{ bf::internal::shift<T>(__LINE__ - begin_ - 2) };

/**
 * Macros used for creating set of ordinary flags,
 * i.e. flags with string representation.
 */

#define BEGIN_BITFLAGS(NAME)                            \
    template <typename T>                               \
    struct NAME##Impl {                                 \
        using flag = bf::internal::flag<NAME##Impl, T>; \
        static constexpr int begin_ = __LINE__;

#define END_BITFLAGS(NAME)                                                      \
        static constexpr int end_   = __LINE__;                                 \
    };                                                                          \
    using NAME = bf::bitflags<                                                  \
        NAME##Impl< bf::bitflags< NAME##Impl<std::uint8_t> >::underlying_type > \
    >;

#define FLAG(NAME) \
    static constexpr flag NAME{ bf::internal::shift<T>(__LINE__ - begin_ - 2), #NAME };

#endif // BITFLAGS_HPP
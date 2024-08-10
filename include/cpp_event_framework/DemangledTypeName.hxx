/**
 * @file DemangledTypeName.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 10-08-2024
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <array> // std::array
#include <cassert>
#include <string_view>
#include <utility> // std::index_sequence

// See https://rodusek.com/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
// (Boost license)

namespace cpp_event_framework
{
template <std::size_t... Idxs>
constexpr auto SubstringAsArray(std::string_view str, std::index_sequence<Idxs...> /*idx*/)
{
    return std::array{str[Idxs]...};
}

template <typename T>
constexpr auto TypeNameArray()
{
#if defined(__clang__)
    constexpr auto kPrefix = std::string_view{"[T = "};
    constexpr auto kSuffix = std::string_view{"]"};
    constexpr auto kFunction = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
    constexpr auto kPrefix = std::string_view{"with T = "};
    constexpr auto kSuffix = std::string_view{"]"};
    constexpr auto kFunction = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
    constexpr auto kPrefix = std::string_view{"type_name_array<"};
    constexpr auto kSuffix = std::string_view{">(void)"};
    constexpr auto kFunction = std::string_view{__FUNCSIG__};
#else
#error Unsupported compiler
#endif

    constexpr auto kStart = kFunction.find(kPrefix) + kPrefix.size();
    constexpr auto kEnd = kFunction.rfind(kSuffix);

    static_assert(kStart < kEnd);

    constexpr auto kName = kFunction.substr(kStart, (kEnd - kStart));
    return SubstringAsArray(kName, std::make_index_sequence<kName.size()>{});
}

template <typename T>
struct TypeNameHolder
{
    static constexpr auto kValue = TypeNameArray<T>();
};

template <typename T>
constexpr auto GetDemangledTypeName()
{
    constexpr auto& kValue = TypeNameHolder<T>::kValue;
    return std::string_view{kValue.data(), kValue.size()};
}
} // namespace cpp_event_framework
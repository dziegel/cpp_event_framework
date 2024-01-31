/**
 * @file Concepts.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-10-2021
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <cstddef>
#include <type_traits>

namespace cpp_event_framework
{
template <typename T>
concept Mutex = std::is_constructible_v<T> && std::is_destructible_v<T> && requires(T a) {
    {
        a.lock()
    };
    {
        a.unlock()
    };
};

template <typename T>
concept Semaphore = std::is_constructible_v<T, std::ptrdiff_t> && std::is_destructible_v<T> && requires(T a) {
    {
        a.acquire()
    };
    {
        a.release()
    };
};
} // namespace cpp_event_framework

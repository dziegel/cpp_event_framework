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

#include <type_traits>

namespace cpp_event_framework
{
template <typename T>
concept BasicLockable = requires(T a) {
    {
        a.lock()
    } -> std::convertible_to<void>;
    {
        a.unlock()
    } -> std::convertible_to<void>;
};

template <typename T>
concept Mutex = std::is_constructible_v<T> && std::is_destructible_v<T> && BasicLockable<T>;

template <typename T>
concept BasicSemaphore = requires(T a) {
    {
        a.acquire()
    } -> std::convertible_to<void>;
    {
        a.release()
    } -> std::convertible_to<void>;
};

template <typename T>
concept Semaphore = std::is_constructible_v<T, ptrdiff_t> && std::is_destructible_v<T> && BasicSemaphore<T>;
} // namespace cpp_event_framework

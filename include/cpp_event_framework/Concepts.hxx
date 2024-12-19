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

#include <cassert>
#include <cstddef>
#include <memory_resource>
#include <type_traits>

namespace cpp_event_framework
{
/**
 * @brief Concept for Mutex: DefaultConstructible, DefaultDesctructible, BasicLockable
 */
template <typename T>
concept Mutex = std::is_constructible_v<T> && std::is_destructible_v<T> && requires(T a) {
    { a.lock() };
    { a.unlock() };
};

/**
 * @brief Concept for semaphore: DefaultConstructible, DefaultDesctructible
 */
template <typename T>
concept Semaphore = std::is_constructible_v<T, std::ptrdiff_t> && std::is_destructible_v<T> && requires(T a) {
    { a.acquire() };
    { a.release() };
};

/**
 * @brief Concept for assertion function
 */
template <typename T>
concept AssertionProvider = requires(T) {
    { T::Assert(bool()) };
};

/**
 * @brief Default assertion provider that uses C-library assert()
 */
class DefaultAssertionProvider
{
public:
    static void Assert(bool condition)
    {
        assert(condition);
    }
};

/**
 * @brief Concept for a provider of a polymorphic allocator (std::pmr::memory_resource)
 */
template <typename T>
concept PolymorphicAllocatorProvider = requires(T a) {
    { a.GetAllocator() } -> std::convertible_to<std::pmr::memory_resource*>;
};
} // namespace cpp_event_framework

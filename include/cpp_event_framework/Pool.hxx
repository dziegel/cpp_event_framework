/**
 * @file Pool.hxx
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
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

namespace cpp_event_framework
{
/**
 * @brief Pool of elements, especially useful for signals
 */
template <typename MutexType = std::mutex, size_t kAlignment = sizeof(uint64_t)>
class Pool
{
public:
    /**
     * @brief Shared pointer alias
     */
    using SPtr = std::shared_ptr<Pool>;
    /**
     * @brief Weak pointer alias
     */
    using WPtr = std::weak_ptr<Pool>;
    /**
     * @brief Unique pointer alias
     */
    using UPtr = std::unique_ptr<Pool>;

    /**
     * @brief Construct a new Pool object
     *
     * @param element_size Size of each pool element
     * @param count Number of pool elements
     * @param name Pool name (logging)
     */
    Pool(size_t element_size, size_t count, std::string name)
        : size_(count), element_size_(((element_size + kAlignment) / kAlignment) * kAlignment), name_(std::move(name))
    {
        pool_mem_.resize(element_size_ * size_);
        for (size_t i = 0; i < count; i++)
        {
            pool_.push(&pool_mem_.at(i * element_size_));
        }
    }

    ~Pool() = default;

    Pool(const Pool& rhs) = delete;
    Pool(Pool&& rhs) = delete;
    Pool& operator=(const Pool& rhs) = delete;
    Pool& operator=(Pool&& rhs) = delete;

    /**
     * @brief Allocate element from pool
     *
     * @return void*
     */
    void* Allocate()
    {
        std::lock_guard lock(mutex_);
        auto* result = pool_.front();
        pool_.pop();
        return result;
    }

    /**
     * @brief Return element to pool
     *
     * @param p Element to destroy
     */
    void Deallocate(void* p) noexcept
    {
        std::lock_guard lock(mutex_);
        pool_.push(p);
    }

    /**
     * @brief Pool fill level (number of elements currently in pool)
     *
     * @return size_t
     */
    size_t FillLevel()
    {
        std::lock_guard lock(mutex_);
        return pool_.size();
    }

    /**
     * @brief Get pool size (max. number of elements)
     *
     * @return size_t
     */
    [[nodiscard]] size_t Size() const
    {
        return size_;
    }

    /**
     * @brief Get size of pool elements
     *
     * @return size_t
     */
    [[nodiscard]] size_t ElementSize() const
    {
        return element_size_;
    }

    /**
     * @brief Get pool name
     *
     * @return const std::string&
     */
    [[nodiscard]] const std::string& Name() const
    {
        return name_;
    }

    /**
     * @brief std::allocator for Pools
     *
     * @tparam T
     */
    template <class T>
    struct Allocator
    {
        /**
         * @brief Allocator traits
         */
        typedef T value_type;

        /**
         * @brief Construct a new Allocator object
         *
         * @param pool Pool to use
         */
        Allocator(Pool& pool) noexcept : pool_(pool)
        {
        }

        /**
         * @brief Allocator traits
         */
        template <class U>
        constexpr Allocator(const Allocator<U>& other) noexcept : pool_(other.pool_)
        {
        }

        /**
         * @brief Allocator traits
         */
        T* allocate(std::size_t n)
        {
            assert(n == 1);
            assert(sizeof(T) <= pool_.ElementSize());
            return static_cast<T*>(pool_.Allocate());
        }

        /**
         * @brief Allocator traits
         */
        void deallocate(T* p, std::size_t n) noexcept
        {
            assert(n == 1);
            pool_.Deallocate(p);
        }

        /**
         * @brief Used pool
         *
         */
        Pool& pool_;
    };

    /**
     * @brief Helper function to create shared-pointer managed instance
     *
     * @return SPtr
     */
    static SPtr MakeShared(size_t element_size, size_t count, std::string name)
    {
        return std::make_shared<Pool>(element_size, count, std::move(name));
    }

private:
    std::vector<uint8_t> pool_mem_;
    std::queue<void*> pool_;
    MutexType mutex_;
    size_t size_ = 0;
    size_t element_size_ = 0;
    std::string name_;
};

/**
 * @brief Template magic to get max. element size for normal objects
 */
template <typename... ElementList>
struct PoolSize;

/**
 * @brief Specialization with no template parameter
 */
template <>
struct PoolSize<>
{
    /**
     * @brief Hook for no template parameter (size 0)
     */
    static constexpr size_t kValue = 0;
};

/**
 * @brief Template magic to get max. element size for normal objects
 */
template <typename Element, typename... ElementList>
struct PoolSize<Element, ElementList...>
{
    /**
     * @brief Max element size
     */
    static constexpr size_t kValue = std::max(sizeof(Element), PoolSize<ElementList...>::kValue);
};

/**
 * @brief Template magic to get max. element size for shared objects
 */
template <typename... ElementList>
struct SptrPoolSize;

/**
 * @brief Template magic to get max. element size for shared objects
 */
template <>
struct SptrPoolSize<>
{
    /**
     * @brief Hook for no template parameter (size 0)
     */
    static constexpr size_t kValue = 0;
};

/**
 * @brief Template magic to get max. element size for shared objects
 */
template <typename Element, typename... ElementList>
struct SptrPoolSize<Element, ElementList...>
{
    /**
     * @brief Max element size
     */
    static constexpr size_t kValue =
        std::max(sizeof(std::_Sp_counted_ptr_inplace<Element, Pool<>::Allocator<Element>, std::__default_lock_policy>),
                 SptrPoolSize<ElementList...>::kValue);
};
} // namespace cpp_event_framework

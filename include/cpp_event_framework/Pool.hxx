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
#include <memory_resource>
#include <mutex>
#include <queue>
#include <string>
#include <vector>

#include <cpp_event_framework/Concepts.hxx>

namespace cpp_event_framework
{
/**
 * @brief Pool of elements, especially useful for signals
 *
 * @tparam MutexType Mutex type to use - e.g. to be able to supply own RT-capable implementation.
 *         NamedRequirements: DefaultConstructible, Destructible, BasicLockable
 * @tparam Alignment Alignment requirement
 */
template <Mutex MutexType = std::mutex, size_t Alignment = sizeof(uint64_t)>
class Pool : public std::pmr::memory_resource
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
        : size_(count), element_size_(((element_size + Alignment) / Alignment) * Alignment), name_(std::move(name))
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
     * @brief std::pmr::memory_resource::do_allocate
     */
    void* do_allocate(size_t bytes, size_t /*alignment*/) override
    {
        assert(bytes <= element_size_);

        std::lock_guard lock(mutex_);
        auto* result = pool_.front();
        pool_.pop();
        return result;
    }

    /**
     * @brief std::pmr::memory_resource::do_deallocate
     */
    void do_deallocate(void* p, size_t /*bytes*/, size_t /*alignment*/) override
    {
        std::lock_guard lock(mutex_);
        pool_.push(p);
    }

    /**
     * @brief std::pmr::memory_resource::do_is_equal
     */
    bool do_is_equal(const memory_resource& other) const noexcept override
    {
        return *this == other;
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
} // namespace cpp_event_framework

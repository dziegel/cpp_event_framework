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

#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory_resource>
#include <mutex>

namespace cpp_event_framework
{
/**
 * @brief Pool of elements, especially useful for signals, with COMPILE TIME memory allocation
 */
template <uint32_t NumElements, size_t ElementSize, typename MutexType = std::mutex,
          size_t kAlignment = sizeof(uint64_t)>
class StaticPool : public std::pmr::memory_resource
{
private:
    static constexpr size_t kAlignedElementSize = ((ElementSize + kAlignment) / kAlignment) * kAlignment;
    struct QueueElement
    {
        union
        {
            std::array<uint64_t, kAlignedElementSize / sizeof(uint64_t)> element;
            QueueElement* next;
        };
    };

    std::array<QueueElement, NumElements> pool_mem_ = {};
    MutexType mutex_;
    QueueElement* first_ = nullptr;
    QueueElement* last_ = nullptr;
    const char* name_ = nullptr;
    std::atomic<size_t> fill_level_ = NumElements;

public:
    /**
     * @brief Construct a new Pool object
     *
     * @param name Pool name (logging)
     */
    explicit StaticPool(const char* name) : first_(&pool_mem_.at(0)), last_(first_), name_(name)
    {
        for (size_t i = 1; i < NumElements; i++)
        {
            last_->next = &pool_mem_.at(i);
            last_ = last_->next;
        }
    }

    ~StaticPool() = default;

    StaticPool(const StaticPool& rhs) = delete;
    StaticPool(StaticPool&& rhs) = delete;
    StaticPool& operator=(const StaticPool& rhs) = delete;
    StaticPool& operator=(StaticPool&& rhs) = delete;

    /**
     * @brief std::pmr::memory_resource::do_allocate
     */
    void* do_allocate(size_t bytes, size_t /*alignment*/) override
    {
        assert(bytes <= ElementSize);

        std::lock_guard lock(mutex_);
        auto* result = first_;
        first_ = result->next;
        fill_level_--;
        return result;
    }

    /**
     * @brief std::pmr::memory_resource::do_deallocate
     */
    void do_deallocate(void* p, size_t /*bytes*/, size_t /*alignment*/) override
    {
        auto ptr = static_cast<QueueElement*>(p);

        std::lock_guard lock(mutex_);
        last_->next = ptr;
        last_ = ptr;
        fill_level_++;
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
        return fill_level_;
    }

    /**
     * @brief Get pool name
     *
     * @return const std::string&
     */
    [[nodiscard]] const char* Name() const
    {
        return name_;
    }
};
} // namespace cpp_event_framework
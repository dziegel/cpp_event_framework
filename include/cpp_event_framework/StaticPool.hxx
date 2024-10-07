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
#include <ostream>

#include <cpp_event_framework/Concepts.hxx>

namespace cpp_event_framework
{
/**
 * @brief Pool of elements, especially useful for signals, with COMPILE TIME memory allocation
 *
 * @tparam NumElements Number of elements in pool
 * @tparam ElementSize Size of a pool element
 * @tparam MutexType Mutex type to use - e.g. to be able to supply own RT-capable implementation.
 *         NamedRequirements: DefaultConstructible, Destructible, BasicLockable
 * @tparam Alignment Alignment requirement
 */
template <uint32_t NumElements, size_t ElemSize, Mutex MutexType = std::mutex, size_t Alignment = sizeof(uint64_t)>
class StaticPool final : public std::pmr::memory_resource
{
private:
    static constexpr size_t kAlignedElementSize = ((ElemSize + Alignment) / Alignment) * Alignment;
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
        assert(bytes <= kAlignedElementSize);

        std::scoped_lock lock(mutex_);
        assert(FillLevel() != 0);

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

        std::scoped_lock lock(mutex_);
        if (first_ == nullptr)
        {
            first_ = ptr;
        }

        if (last_ != nullptr)
        {
            last_->next = ptr;
        }
        else
        {
            last_ = ptr;
        }

        fill_level_++;
        assert(FillLevel() <= NumElements);
    }

    /**
     * @brief std::pmr::memory_resource::do_is_equal
     */
    bool do_is_equal(const memory_resource& other) const noexcept override
    {
        return *this == other;
    }

    /**
     * @brief Get pool size (max. number of elements)
     *
     * @return size_t
     */
    [[nodiscard]] size_t Size() const
    {
        return NumElements;
    }

    /**
     * @brief Get size of pool elements
     *
     * @return size_t
     */
    [[nodiscard]] size_t ElementSize() const
    {
        return ElemSize;
    }

    /**
     * @brief Pool fill level (number of elements currently in pool)
     *
     * @return size_t
     */
    [[nodiscard]] size_t FillLevel() const
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

    /**
     * @brief Stream operator for logging
     */
    friend inline std::ostream& operator<<(std::ostream& ostream, const StaticPool& pool)
    {
        return ostream << pool.Name() << " [" << pool.FillLevel() << "/" << pool.Size() << "]";
    }
};
} // namespace cpp_event_framework

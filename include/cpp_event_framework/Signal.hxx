/**
 * @file Signal.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-10-2021
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <cassert>
#include <cstdint>
#include <memory>
#include <ostream>

namespace cpp_event_framework
{
/**
 * @brief Signal
 */
class Signal
{
public:
    /**
     * @brief Shared pointer alias
     */
    using SPtr = std::shared_ptr<Signal>;
    /**
     * @brief Weak pointer alias
     */
    using WPtr = std::weak_ptr<Signal>;

    /**
     * @brief ID type alias
     */
    using IdType = uint32_t;

    // Not copyable, not movable - always use shared pointers!
    Signal(const Signal& rhs) = delete;
    Signal(Signal&& rhs) = delete;
    Signal& operator=(const Signal& rhs) = delete;
    Signal& operator=(Signal&& rhs) = delete;

    // No new - always use shared pointers!
    static void* operator new(size_t) = delete;

    /**
     * @brief Get event id
     */
    [[nodiscard]] IdType Id() const
    {
        return id_;
    }

    /**
     * @brief Get event name
     */
    [[nodiscard]] virtual const char* Name() const = 0;

    /**
     * @brief Cast from generic signal
     */
    static SPtr FromSignal(const SPtr& event)
    {
        return event;
    }

    /**
     * @brief Stream operator for logging
     */
    friend inline std::ostream& operator<<(std::ostream& os, const Signal::SPtr& event)
    {
        return os << event->Name();
    }

protected:
    /**
     * @brief Construct a new Signal object
     */
    explicit Signal(IdType id) : id_(id)
    {
    }
    /**
     * @brief Destroy the Signal object
     */
    virtual ~Signal() = default;

private:
    const IdType id_;
};

/**
 * @brief Use this allocator to use a Pool as event source
 *
 * @tparam T Name of inhering class
 * @tparam Pool Pool type to use
 */
template <typename T, typename Pool = Pool<>>
struct PoolAllocator
{
    /**
     * @brief Set the Pool to use
     */
    static void SetPool(typename Pool::SPtr apool)
    {
        assert(pool == nullptr);
        pool = std::move(apool);
    }

    /**
     * @brief Get allocator
     */
    template <typename Element>
    static auto GetAllocator()
    {
        assert(pool != nullptr);
        return typename Pool::template Allocator<Element>(*pool.get());
    }

    /**
     * @brief Pool
     */
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
    static typename Pool::SPtr pool;
};

template <typename T, typename Pool>
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
typename Pool::SPtr PoolAllocator<T, Pool>::pool = nullptr;

/**
 * @brief Use this allocator to allocate from heap
 */
struct HeapAllocator
{
    /**
     * @brief Get the Allocator object
     */
    template <typename Element>
    static auto GetAllocator()
    {
        return std::allocator<Element>();
    }
};

/**
 * @brief Signal event template
 *
 * @tparam T Name of inheriting class
 * @tparam id Signal ID
 * @tparam AllocatorType Allocator to use
 * @tparam BaseType Base class to inherit from
 */
template <typename T, Signal::IdType id, typename BaseType = Signal, typename AllocatorType = HeapAllocator>
class SignalBase : public BaseType
{
public:
    /**
     * @brief Shared pointer alias
     */
    using SPtr = std::shared_ptr<T>;

    /**
     * @brief Used allocator class
     */
    using Allocator = AllocatorType;

    /**
     * @brief Signal ID
     */
    static constexpr Signal::IdType kId = id;

    /**
     * @brief Helper function to create shared-pointer managed instance
     *
     * @param args Constructor args
     */
    template <typename... Args>
    static SPtr MakeShared(Args... args)
    {
        return std::allocate_shared<T>(Allocator::template GetAllocator<T>(), args...);
    }

    /**
     * @brief Convert from generic event to specific event class
     */
    static SPtr FromSignal(const Signal::SPtr& event)
    {
        assert(Check(event));
        return std::static_pointer_cast<T>(event);
    }

    /**
     * @brief Get event name
     */
    [[nodiscard]] const char* Name() const override
    {
        return typeid(T).name();
        // return abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    }

    /**
     * @brief Check if event is of this kind
     */
    static bool Check(const Signal::SPtr& event)
    {
        return event->Id() == kId;
    }

protected:
    /**
     * @brief Helper to shorten possible constructor base class call
     */
    using Base = SignalBase<T, id, BaseType, AllocatorType>;

    /**
     * @brief Construct a new SignalBase object
     */
    SignalBase() : BaseType(kId)
    {
    }

    /**
     * @brief Constructor that passes arguments to base class
     *
     * @tparam Args Arg types
     * @param args arguments
     */
    template <typename... Args>
    SignalBase(Args... args) : BaseType(kId, args...)
    {
    }
};

/**
 * @brief Template to declare next signal (auto event id and use same allocator)
 *
 * @tparam T Name of inheriting class
 * @tparam Previous Previous signal class
 * @tparam BaseType Base class to inherit from
 */
template <typename T, typename Previous, typename BaseType = Signal>
class NextSignal : public SignalBase<T, Previous::kId + 1, BaseType, typename Previous::Allocator>
{
public:
    /**
     * @brief Previous signal class
     */
    using Prev = Previous;

protected:
    NextSignal() = default;

    /**
     * @brief Helper to shorten possible constructor base class call
     */
    using Base = NextSignal<T, Previous, BaseType>;

    /**
     * @brief Constructor that passes arguments to base class
     *
     * @tparam Args Arg types
     * @param args arguments
     */
    template <typename... Args>
    NextSignal(Args... args) : SignalBase<T, Previous::kId + 1, BaseType, typename Previous::Allocator>(args...)
    {
    }
};

/**
 * @brief Template magic to get max. element size for normal objects
 */
template <typename... ElementList>
struct PoolElementSize;

/**
 * @brief Specialization with no template parameter
 */
template <>
struct PoolElementSize<>
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
struct PoolElementSize<Element, ElementList...>
{
    /**
     * @brief Max element size
     */
    static constexpr size_t kValue = std::max(sizeof(Element), PoolElementSize<ElementList...>::kValue);
};

/**
 * @brief Template magic to get max. element size for shared objects
 */
template <typename... ElementList>
struct SptrPoolElementSize;

/**
 * @brief Template magic to get max. element size for shared objects
 */
template <>
struct SptrPoolElementSize<>
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
struct SptrPoolElementSize<Element, ElementList...>
{
    /**
     * @brief Max element size
     */
    static constexpr size_t kValue =
        std::max(sizeof(std::_Sp_counted_ptr_inplace<Element, std::pmr::polymorphic_allocator<Element>,
                                                     std::__default_lock_policy>),
                 SptrPoolElementSize<ElementList...>::kValue);
};

/**
 * @brief Helper class to calculate pool sizes
 *
 * @tparam Args List of all signals in this pool
 */
template <typename... Args>
struct SignalPoolElementSizeCalculator
{
    /**
     * @brief Max element (heap size)
     */
    static constexpr uint32_t kHeapSize = PoolElementSize<Args...>::kValue;

    /**
     * @brief Max element (heap size)
     */
    static constexpr uint32_t kSptrSize = SptrPoolElementSize<Args...>::kValue;
};

template <typename... Args>
const uint32_t SignalPoolElementSizeCalculator<Args...>::kHeapSize;

template <typename... Args>
const uint32_t SignalPoolElementSizeCalculator<Args...>::kSptrSize;
} // namespace cpp_event_framework

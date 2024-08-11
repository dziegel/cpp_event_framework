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
#include <memory_resource>
#include <ostream>

#include <cpp_event_framework/Concepts.hxx>
#include <cpp_event_framework/DemangledTypeName.hxx>

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
     * @brief Get demangled event name
     */
    [[nodiscard]] virtual const char* DemangledName() const = 0;

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
    friend inline std::ostream& operator<<(std::ostream& ostream, const Signal::SPtr& event)
    {
        return ostream << event->DemangledName();
    }

protected:
    /**
     * @brief Construct a new Signal object
     */
    explicit Signal(IdType signal_id) : id_(signal_id)
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
 * @brief Concept for a class derived from Signal
 */
template <typename T>
concept SignalSubclass = std::is_base_of_v<Signal, T>;

/**
 * @brief Use this allocator to use a custom allocator (e.g. pool) as event source
 *
 * @tparam T Name of inhering class
 */
template <typename T>
class CustomAllocator
{
public:
    /**
     * @brief Set the allocator (plain pointer)
     */
    static void SetAllocator(std::pmr::memory_resource* alloc)
    {
        assert(allocator == nullptr);
        allocator = alloc;
    }

    /**
     * @brief Set the allocator (shared pointer)
     */
    static void SetAllocator(std::shared_ptr<std::pmr::memory_resource> alloc)
    {
        assert(allocator == nullptr);
        shared_allocator = std::move(alloc);
        allocator = shared_allocator.get();
    }

    /**
     * @brief Default heap-based allocator
     */
    static std::pmr::memory_resource* GetAllocator()
    {
        return allocator;
    }

private:
    static std::pmr::memory_resource* allocator;
    static std::shared_ptr<std::pmr::memory_resource> shared_allocator;
};
template <typename T>
std::pmr::memory_resource* CustomAllocator<T>::allocator = nullptr;

template <typename T>
std::shared_ptr<std::pmr::memory_resource> CustomAllocator<T>::shared_allocator = nullptr;

/**
 * @brief Signal event template
 *
 * @tparam T Name of inheriting class
 * @tparam id Signal ID
 * @tparam AllocatorType Allocator to use
 * @tparam BaseType Base class to inherit from
 */
template <typename T, Signal::IdType id, SignalSubclass BaseType = Signal,
          PolymorphicAllocatorProvider AllocatorType = HeapAllocator>
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
    static SPtr MakeShared(Args&&... args)
    {
        return std::allocate_shared<T, std::pmr::polymorphic_allocator<T>>(Allocator::GetAllocator(),
                                                                           std::forward<Args>(args)...);
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
    }

    /**
     * @brief Get demangled event name
     */
    [[nodiscard]] const char* DemangledName() const override
    {
        return GetDemangledTypeName<T>();
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
template <typename T, SignalSubclass Previous, SignalSubclass BaseType = Signal>
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
} // namespace cpp_event_framework

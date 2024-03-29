/**
 * @file EventQueue.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <semaphore>

#include <cpp_active_objects/IActiveObject.hxx>
#include <cpp_active_objects/IEventQueue.hxx>
#include <cpp_event_framework/Concepts.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects
{
/**
 * @brief A thread-safe event queue
 *
 * @tparam SemaphoreType Sempahore type to use - e.g. to be able to supply own RT-capable implementation
 *         NamedRequirements: DefaultConstructible. No named requirements for release() and acquire() available.
 * @tparam MutexType Mutex type to use - e.g. to be able to supply own RT-capable implementation.
 *         NamedRequirements: DefaultConstructible, Destructible, BasicLockable
 */
template <cpp_event_framework::Semaphore SemaphoreType = std::binary_semaphore,
          cpp_event_framework::Mutex MutexType = std::mutex>
class EventQueue final : public IEventQueue
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<EventQueue>;

    EventQueue() = default;
    ~EventQueue() = default;

    // Non-copyable, non-movable
    EventQueue(const EventQueue& rhs) = delete;
    EventQueue(EventQueue&& rhs) = delete;
    EventQueue& operator=(const EventQueue& rhs) = delete;
    EventQueue& operator=(EventQueue&& rhs) = delete;

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     */
    void EnqueueBack(IActiveObject::SPtr target, cpp_event_framework::Signal::SPtr event) override
    {
        {
            std::scoped_lock lock(mutex_);
            queue_.emplace_back(std::move(target), std::move(event));
        }
        sem_.release();
    }

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     */
    void EnqueueFront(IActiveObject::SPtr target, cpp_event_framework::Signal::SPtr event) override
    {
        {
            std::scoped_lock lock(mutex_);
            queue_.emplace_front(std::move(target), std::move(event));
        }
        sem_.release();
    }

    /**
     * @brief Dequeue an entry, possibly blocking until there is an entry in the queue
     *
     * @return QueueEntry Queue entry
     */
    QueueEntry Dequeue() override
    {
        sem_.acquire();

        std::scoped_lock lock(mutex_);
        auto result = std::move(queue_.front());
        queue_.pop_front();
        return result;
    }

private:
    std::list<QueueEntry> queue_;
    SemaphoreType sem_{0};
    MutexType mutex_;
};
} // namespace cpp_active_objects

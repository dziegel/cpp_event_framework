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
#include <memory_resource>
#include <mutex>
#include <semaphore>

#include <cpp_active_objects_embedded/IActiveObject.hxx>
#include <cpp_active_objects_embedded/IEventQueue.hxx>
#include <cpp_event_framework/Concepts.hxx>
#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/StaticPool.hxx>

namespace cpp_active_objects_embedded
{
/**
 * @brief A thread-safe event queue
 *
 * @tparam SemaphoreType Sempahore type to use - e.g. to be able to supply own RT-capable implementation
 * @tparam MutexType Mutex type to use - e.g. to be able to supply own RT-capable implementation
 */
template <size_t NumEntries, cpp_event_framework::Semaphore SemaphoreType = std::binary_semaphore,
          cpp_event_framework::Mutex MutexType = std::mutex>
class EventQueue final : public IEventQueue
{
public:
    EventQueue() : memory_pool_("EventQueue"), queue_(&memory_pool_)
    {
    }

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     */
    void EnqueueBack(IActiveObject* target, cpp_event_framework::Signal::SPtr event) override
    {
        {
            std::scoped_lock lock(mutex_);
            queue_.emplace_back(target, std::move(event));
        }
        sem_.release();
    }

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     */
    void EnqueueFront(IActiveObject* target, cpp_event_framework::Signal::SPtr event) override
    {
        {
            std::scoped_lock lock(mutex_);
            queue_.emplace_front(target, std::move(event));
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
        auto result = queue_.front();
        queue_.pop_front();
        return result;
    }

private:
    cpp_event_framework::StaticPool<NumEntries, sizeof(std::_List_node<QueueEntry>)> memory_pool_;
    std::list<QueueEntry, std::pmr::polymorphic_allocator<QueueEntry>> queue_;
    SemaphoreType sem_{0};
    MutexType mutex_;
};
} // namespace cpp_active_objects_embedded

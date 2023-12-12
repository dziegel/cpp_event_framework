/**
 * @file ThreadSafeEventQueue.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>
#include <mutex>
#include <semaphore>

#include <cpp_active_objects/IActiveObject.hxx>
#include <cpp_active_objects/IEventQueue.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects
{
/**
 * @brief A thread-safe event queue
 *
 * @tparam SemaphoreType Sempahore type to use - e.g. to be able to supply own RT-capable implementation
 * @tparam MutexType Mutex type to use - e.g. to be able to supply own RT-capable implementation
 */
template <typename SemaphoreType = std::binary_semaphore, typename MutexType = std::mutex>
class ThreadSafeEventQueue final : public IEventQueue
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<ThreadSafeEventQueue>;

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     * @param priority Sort priority in queue, lower numbers = higher priority = "more to the front"
     */
    void Enqueue(IActiveObject::SPtr target, cpp_event_framework::Signal::SPtr event,
                 PriorityType priority = 0) override
    {
        {
            std::scoped_lock lock(mutex_);
            if (priority >= 0)
            {
                queue_.emplace_back(std::move(target), std::move(event), priority);
            }
            else
            {
                queue_.emplace_front(std::move(target), std::move(event), priority);
            }
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
    std::deque<QueueEntry> queue_;
    SemaphoreType sem_{0};
    MutexType mutex_;
};
} // namespace cpp_active_objects
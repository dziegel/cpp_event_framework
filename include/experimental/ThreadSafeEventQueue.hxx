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

#include <cpp_event_framework/Signal.hxx>
#include <experimental/IActiveObject.hxx>
#include <experimental/IEventQueue.hxx>

namespace cpp_event_framework
{
template <typename SemaphoreType = std::counting_semaphore, typename MutexType = std::mutex>
class ThreadSafeEventQueue final : public IEventQueue
{
public:
    using SPtr = std::shared_ptr<ThreadSafeEventQueue>;

    void PushBack(IActiveObject::SPtr target, Signal::SPtr event) override
    {
        {
            std::scoped_lock lock(mutex_);
            queue_.emplace_back(std::move(target), std::move(event));
        }
        sem_.release();
    }

    void PushFront(IActiveObject::SPtr target, Signal::SPtr event) override
    {
        {
            std::scoped_lock lock(mutex_);
            queue_.emplace_front(std::move(target), std::move(event));
        }
        sem_.release();
    }

    std::pair<IActiveObject::SPtr, Signal::SPtr> Dequeue() override
    {
        sem_.acquire();

        std::scoped_lock lock(mutex_);
        auto result = queue_.front();
        queue_.pop_front();
        return result;
    }

private:
    std::deque<std::pair<IActiveObject::SPtr, Signal::SPtr>> queue_;
    SemaphoreType sem_{0};
    MutexType mutex_;
};
} // namespace cpp_event_framework

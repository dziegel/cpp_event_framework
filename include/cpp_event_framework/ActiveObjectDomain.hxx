/**
 * @file ActiveObjectDomain.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>
#include <thread>

#include <cpp_event_framework/IActiveObject.hxx>
#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/ThreadSafeEventQueue.hxx>

namespace cpp_event_framework
{
class ActiveObjectDomain
{
public:
    using SPtr = std::shared_ptr<ActiveObjectDomain>;

    ActiveObjectDomain() : ActiveObjectDomain(std::make_shared<ThreadSafeEventQueue<>>())
    {
    }

    explicit ActiveObjectDomain(IEventQueue::SPtr queue) : queue_(std::move(queue)), thread_([this]() { Run(); })
    {
    }

    ~ActiveObjectDomain()
    {
        queue_->PushFront(nullptr, nullptr);
        thread_.join();
    }

    void RegisterObject(const IActiveObject::SPtr& active_object)
    {
        active_object->SetQueue(queue_);
    }

    std::jthread& Thread()
    {
        return thread_;
    }

private:
    void Run()
    {
        while (true)
        {
            const auto [target, event] = queue_->Dequeue();
            if (target == nullptr)
            {
                return;
            }
            target->Dispatch(event);
        }
    }

    IEventQueue::SPtr queue_;
    std::jthread thread_;
};
} // namespace cpp_event_framework

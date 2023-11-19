/**
 * @file ActiveObjectDomainBase.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>

#include <cpp_event_framework/IActiveObjectDomain.hxx>
#include <cpp_event_framework/IEventQueue.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_event_framework
{
class ActiveObjectDomainBase : public IActiveObjectDomain
{
public:
    using SPtr = std::shared_ptr<ActiveObjectDomainBase>;

    void RegisterObject(const IActiveObject::SPtr& active_object) final
    {
        active_object->SetQueue(queue_);
    }

protected:
    explicit ActiveObjectDomainBase(IEventQueue::SPtr queue) : queue_(std::move(queue))
    {
    }

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
};
} // namespace cpp_event_framework

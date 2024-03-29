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

#include <cpp_active_objects/IActiveObjectDomain.hxx>
#include <cpp_active_objects/IEventQueue.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects
{
/**
 * @brief Base class of an active object domain
 *
 */
class ActiveObjectDomainBase : public IActiveObjectDomain
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<ActiveObjectDomainBase>;

    /**
     * @brief Assign an active object to this domain
     *
     * @param active_object
     */
    void RegisterObject(const IActiveObject::SPtr& active_object) final
    {
        active_object->SetQueue(queue_);
    }

protected:
    /**
     * @brief Constructor
     *
     * @param queue Queue to use
     */
    explicit ActiveObjectDomainBase(IEventQueue::SPtr queue) : queue_(std::move(queue))
    {
    }

    /**
     * @brief Dequeue and dispatch event queue entries
     *
     */
    void Run()
    {
        while (true)
        {
            const auto entry = queue_->Dequeue();
            if (entry.target == nullptr)
            {
                return;
            }
            entry.target->Dispatch(entry.event);
        }
    }

    /**
     * @brief Enqueue dummy entry to exit Run() loop
     *
     */
    void Stop()
    {
        queue_->EnqueueBack(nullptr, nullptr);
    }

private:
    IEventQueue::SPtr queue_;
};
} // namespace cpp_active_objects

/**
 * @file ActiveObjectBase.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>

#include <cpp_active_objects/IActiveObject.hxx>
#include <cpp_active_objects/IEventQueue.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects
{
/**
 * @brief Base class for active objects
 *
 */
class ActiveObjectBase : public IActiveObject
{
public:
    /**
     * @brief Assign object to a queue
     *
     * @param queue
     */
    void SetQueue(const IEventQueue::SPtr& queue) final
    {
        assert(queue_ == nullptr);
        queue_ = queue;
    }

    /**
     * @brief Enqueue (back) a signal to be dispateched by this object
     *
     * @param event
     * @param priority
     */
    void Take(const cpp_event_framework::Signal::SPtr& event, int priority = 0) final
    {
        assert(queue_ != nullptr);
        queue_->Enqueue(std::static_pointer_cast<IActiveObject>(shared_from_this()), event, priority);
    }

private:
    IEventQueue::SPtr queue_;
};
} // namespace cpp_active_objects

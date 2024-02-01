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

#include <cpp_active_objects_embedded/IActiveObject.hxx>
#include <cpp_active_objects_embedded/IEventQueue.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects_embedded
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
    void SetQueue(IEventQueue* queue) final
    {
        assert(queue_ == nullptr);
        queue_ = queue;
    }

    /**
     * @brief Enqueue (back) a signal to be dispatched by this object
     *
     * @param event
     */
    void Take(const cpp_event_framework::Signal::SPtr& event) final
    {
        assert(queue_ != nullptr);
        queue_->EnqueueBack(this, event);
    }

    /**
     * @brief Enqueue (back) a signal to be dispatched by this object
     *
     * @param event
     */
    void TakeHighPrio(const cpp_event_framework::Signal::SPtr& event) final
    {
        assert(queue_ != nullptr);
        queue_->EnqueueFront(this, event);
    }

private:
    IEventQueue* queue_ = nullptr;
};
} // namespace cpp_active_objects_embedded

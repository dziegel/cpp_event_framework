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

#include <experimental/IActiveObject.hxx>
#include <experimental/IEventQueue.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_event_framework
{
class ActiveObjectBase : public IActiveObject
{
public:
    void SetQueue(const IEventQueue::SPtr& queue) final
    {
        assert(queue_ == nullptr);
        queue_ = queue;
    }

    void Take(const Signal::SPtr& event) final
    {
        assert(queue_ != nullptr);
        queue_->PushBack(shared_from_this(), event);
    }

    void TakeHighPrio(const Signal::SPtr& event) final
    {
        assert(queue_ != nullptr);
        queue_->PushFront(shared_from_this(), event);
    }

private:
    IEventQueue::SPtr queue_;
};
} // namespace cpp_event_framework

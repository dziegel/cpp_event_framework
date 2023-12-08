/**
 * @file IActiveObject.hxx
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-11-19
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <memory>

#include <cpp_event_framework/Signal.hxx>
#include <experimental/IEventTarget.hxx>

namespace cpp_event_framework
{
class IEventQueue;

class IActiveObject : public IEventTarget
{
public:
    using SPtr = std::shared_ptr<IActiveObject>;

    ~IActiveObject() override = default;

    /**
     * @brief Set the Queue object
     *
     * @param queue
     */
    virtual void SetQueue(const std::shared_ptr<IEventQueue>& queue) = 0;

    /**
     * @brief Dispatch event in active object domain
     *
     * @param event
     */
    virtual void Dispatch(const Signal::SPtr& event) = 0;
};
} // namespace cpp_event_framework

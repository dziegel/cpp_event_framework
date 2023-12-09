/**
 * @file IEventQueue.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>

#include <cpp_event_framework/Signal.hxx>

namespace cpp_event_framework
{
class IActiveObject;

/**
 * @brief Event queue interface
 *
 */
class IEventQueue
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<IEventQueue>;

    /**
     * @brief Destroy the EventQueue
     *
     */
    virtual ~IEventQueue() = default;

    /**
     * @brief Enqueue an event at BACK to be dispateched by a target
     *
     * @param target
     * @param event
     */
    virtual void PushBack(std::shared_ptr<IActiveObject> target, Signal::SPtr event) = 0;
    /**
     * @brief Enqueue an event at FRONT to be dispateched by a target
     *
     * @param target
     * @param event
     */
    virtual void PushFront(std::shared_ptr<IActiveObject> target, Signal::SPtr event) = 0;

    /**
     * @brief Dequeue an ActiveObject-Event pair
     *
     * @return std::pair<std::shared_ptr<IActiveObject>, Signal::SPtr>
     */
    virtual std::pair<std::shared_ptr<IActiveObject>, Signal::SPtr> Dequeue() = 0;
};
} // namespace cpp_event_framework

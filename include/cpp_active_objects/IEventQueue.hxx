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

namespace cpp_active_objects
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
     * @brief Type of queue entrie
     */
    struct QueueEntry
    {
        /**
         * @brief Dispatch target
         */
        std::shared_ptr<IActiveObject> target;
        /**
         * @brief Event
         */
        cpp_event_framework::Signal::SPtr event;
    };

    /**
     * @brief Destroy the EventQueue
     *
     */
    virtual ~IEventQueue() = default;

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     */
    virtual void EnqueueBack(std::shared_ptr<IActiveObject> target, cpp_event_framework::Signal::SPtr event) = 0;

    /**
     * @brief Enqueue an event to be dispatched by a target
     *
     * @param target
     * @param event
     */
    virtual void EnqueueFront(std::shared_ptr<IActiveObject> target, cpp_event_framework::Signal::SPtr event) = 0;

    /**
     * @brief Dequeue an ActiveObject-Event pair
     *
     * @return std::pair<std::shared_ptr<IActiveObject>, Signal::SPtr>
     */
    virtual QueueEntry Dequeue() = 0;
};
} // namespace cpp_active_objects

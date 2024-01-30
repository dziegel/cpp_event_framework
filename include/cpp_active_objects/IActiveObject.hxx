/**
 * @file IActiveObject.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>

#include <cpp_active_objects/IEventTarget.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects
{
class IEventQueue;

/**
 * @brief Interface of an active object
 * Can be assigned to a queue and dispatches events
 *
 */
class IActiveObject : public IEventTarget
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
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
     * @param priority
     */
    virtual void Dispatch(const cpp_event_framework::Signal::SPtr& event) = 0;
};
} // namespace cpp_active_objects

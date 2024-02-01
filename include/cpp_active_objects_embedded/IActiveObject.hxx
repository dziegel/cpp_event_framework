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

#include <cpp_active_objects_embedded/IEventTarget.hxx>
#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects_embedded
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
    ~IActiveObject() override = default;

    /**
     * @brief Set the Queue object
     *
     * @param queue
     */
    virtual void SetQueue(IEventQueue* queue) = 0;

    /**
     * @brief Dispatch event in active object domain
     *
     * @param event
     */
    virtual void Dispatch(const cpp_event_framework::Signal::SPtr& event) = 0;
};
} // namespace cpp_active_objects_embedded

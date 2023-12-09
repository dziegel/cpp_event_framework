/**
 * @file IEventTarget.hxx
 * @author Dirk Ziegelmeier (dirḱ@ziegelmeier.net)
 * @brief
 * @date 2023-11-19
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>

#include <cpp_event_framework/Signal.hxx>

namespace cpp_active_objects
{
class IEventQueue;

/**
 * @brief An object taking events (from any thread)
 *
 */
class IEventTarget : public std::enable_shared_from_this<IEventTarget>
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<IEventTarget>;

    /**
     * @brief PriorityType alias
     */
    using PriorityType = int;

    virtual ~IEventTarget() = default;

    /**
     * @brief Take an event from ANY thread
     *
     * @param event
     * @param priority Sort priority in queue, lower numbers = higher priority = "more to the front"
     */
    virtual void Take(const cpp_event_framework::Signal::SPtr& event, PriorityType priority = 0) = 0;
};
} // namespace cpp_active_objects

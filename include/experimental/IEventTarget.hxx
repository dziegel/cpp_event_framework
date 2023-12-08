/**
 * @file IEventTarget.hxx
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

namespace cpp_event_framework
{
class IEventQueue;

class IEventTarget : public std::enable_shared_from_this<IEventTarget>
{
public:
    using SPtr = std::shared_ptr<IEventTarget>;

    virtual ~IEventTarget() = default;

    /**
     * @brief Take an event from ANY thread
     *
     * @param event
     */
    virtual void Take(const Signal::SPtr& event) = 0;

    /**
     * @brief Take an high-prio event from ANY thread (enqueued to front)
     *
     * @param event
     */
    virtual void TakeHighPrio(const Signal::SPtr& event) = 0;
};
} // namespace cpp_event_framework

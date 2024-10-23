/**
 * @file Hsm.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <memory>
#include <ranges>
#include <vector>

#include <cpp_active_objects_embedded/ActiveObjectBase.hxx>
#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>

namespace cpp_active_objects_embedded
{
/**
 * @brief Base class for a statemachine using active-object pattern
 * Supports deferred events.
 *
 * @tparam Fsm Statemachine to aggregate
 */
template <typename Fsm>
class Hsm : public ActiveObjectBase
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<Hsm>;

    Hsm()
    {
        fsm_.on_defer_event_ = [this](Fsm::StateRef, Fsm::Event event) { DeferEvent(event); };
        fsm_.on_recall_deferred_events_ = [this](Fsm::StateRef) { RecallEvents(); };
    }

    /**
     * @brief Dispatch event in active object domain
     *
     * @param event
     */
    void Dispatch(const cpp_event_framework::Signal::SPtr& event) override
    {
        fsm_.React(event);
    }

protected:
    /**
     * @brief Statemachine
     *
     */
    Fsm fsm_;

private:
    std::vector<cpp_event_framework::Signal::SPtr> deferred_events_;

    void DeferEvent(Fsm::Event event)
    {
        deferred_events_.emplace_back(event);
    }

    void RecallEvents()
    {
        for (const auto& event : std::ranges::reverse_view(deferred_events_))
        {
            TakeHighPrio(event);
        }
        deferred_events_.clear();
    }
};
} // namespace cpp_active_objects_embedded

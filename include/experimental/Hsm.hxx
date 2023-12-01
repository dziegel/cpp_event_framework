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

#include <vector>

#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>
#include <experimental/ActiveObjectBase.hxx>

namespace cpp_event_framework
{
template <typename Fsm>
class Hsm : public ActiveObjectBase
{
public:
    Hsm()
    {
        fsm_.on_defer_event_ = [this](Fsm::Ref /*fsm*/, Fsm::StateRef, Fsm::Event event) { OnDeferEvent(event); };
        fsm_.on_recall_deferred_events_ = [this](Fsm::Ref /*fsm*/, Fsm::StateRef) { OnRecallEvents(); };
    }

    void Dispatch(const Signal::SPtr& event) final
    {
        fsm_.React(event);
    }

protected:
    Fsm fsm_;

    void OnDeferEvent(Fsm::Event event)
    {
        deferred_events_.emplace_back(event);
    }

    void OnRecallEvents()
    {
        for (const auto& event : deferred_events_)
        {
            TakeHighPrio(event);
        }
        deferred_events_.clear();
    }

private:
    std::vector<Signal::SPtr> deferred_events_;
};
} // namespace cpp_event_framework

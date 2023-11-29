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
#include <vector>

#include <experimental/ActiveObjectBase.hxx>
#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>

namespace cpp_event_framework
{
template <typename ImplType>
class Hsm : public ActiveObjectBase
{
public:
    Hsm()
    {
        hsm_.on_defer_event_ = [this](Fsm::StateRef, Fsm::Event event) { deferred_events_.emplace_back(event); };
        hsm_.on_recall_deferred_events_ = [this]()
        {
            for (const auto& event : deferred_events_)
            {
                TakeHighPrio(event);
            }
            deferred_events_.clear();
        };
    }

    void Dispatch(const Signal::SPtr& event) final
    {
        hsm_.React(event);
    }

protected:
    using Fsm = Statemachine<ImplType, Signal::SPtr>;
    Fsm hsm_;

private:
    std::vector<Signal::SPtr> deferred_events_;
};
} // namespace cpp_event_framework

#pragma once

#include <cpp_active_objects_embedded/Hsm.hxx>

#include "Fsm.hxx"
#include "IFsmImpl.hxx"

namespace example::activeobject_embedded
{
// Implements Fsm action interface
class FsmImpl final : public cpp_active_objects_embedded::Hsm<Fsm>, private IFsmImpl
{
public:
    FsmImpl();

    FsmBase::StatePtr CurrentState()
    {
        return fsm_.CurrentState();
    }

private:
    void State1Entry() override;

    void State2ToState1TransitionAction(FsmBase::Event event) override;

    bool SomeGuardFunction(FsmBase::Event event) override;
};
} // namespace example::activeobject_embedded

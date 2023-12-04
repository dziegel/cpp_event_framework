#pragma once

#include "Fsm.hxx"
#include "IFsmImpl.hxx"

namespace example::activeobject
{
// Implements Fsm action interface
class FsmImpl : public cpp_event_framework::Hsm<example::activeobject::Fsm>, public IFsmImpl
{
public:
    FsmImpl();

    void State1Entry() override;

    void State2ToState1TransitionAction(FsmBase::Event event) override;

    bool SomeGuardFunction(FsmBase::Event event) override;

    example::activeobject::FsmBase::StatePtr CurrentState()
    {
        return fsm_.CurrentState();
    }
};
} // namespace example::activeobject

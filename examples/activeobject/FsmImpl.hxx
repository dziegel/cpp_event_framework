#pragma once

#include <cpp_active_objects/Hsm.hxx>

#include "Fsm.hxx"
#include "IFsmImpl.hxx"

namespace example::activeobject
{
// Implements Fsm action interface
class FsmImpl : public cpp_active_objects::Hsm<example::activeobject::Fsm>, public IFsmImpl
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

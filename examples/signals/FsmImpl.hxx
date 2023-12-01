#pragma once

#include "Fsm.hxx"
#include "IFsmImpl.hxx"

namespace example::signals
{
// Implements Fsm action interface
class FsmImpl : public IFsmImpl
{
public:
    FsmImpl();

    void Run();

    void State1Entry() override;

    void State2ToState1TransitionAction(FsmBase::Event event) override;

    bool SomeGuardFunction(FsmBase::Event event) override;

private:
    Fsm fsm_;
};
} // namespace example::signals

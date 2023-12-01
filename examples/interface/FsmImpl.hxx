#pragma once

#include "Fsm.hxx"
#include "IFsmImpl.hxx"

namespace example::interface
{
// Implements Fsm action interface
class FsmImpl : public IFsmImpl
{
public:
    FsmImpl();

    void Run();

    void State1Entry() override;

    void State2ToState1TransitionAction(FsmBase::Event) override;

    bool SomeGuardFunction(FsmBase::Event) override;

private:
    Fsm fsm_;
};
} // namespace example::interface

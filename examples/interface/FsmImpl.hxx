#pragma once

#include "Fsm.hxx"
#include "IFsmImpl.hxx"

namespace example::interface
{
// Implements Fsm action interface
class FsmImpl final : private IFsmImpl
{
public:
    FsmImpl();

    void Run();

private:
    Fsm fsm_;

    void State1Entry() override;

    void State2ToState1TransitionAction(FsmBase::Event event) override;

    bool SomeGuardFunction(FsmBase::Event event) override;
};
} // namespace example::interface

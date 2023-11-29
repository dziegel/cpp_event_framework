#pragma once

#include "IFsmImpl.hxx"
#include "Fsm.hxx"

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

private:
    Fsm fsm_;
};
}

#include <iostream>

#include "FsmImpl.hxx"

namespace example::interface
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "Fsm", Fsm::kInitialState);
    fsm_.Start();
}

void FsmImpl::State1Entry()
{
    std::cout << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event event)
{
    std::cout << "State2ToState1TransitionAction" << std::endl;
}
}

#include <iostream>

#include "FsmImpl.hxx"

namespace example::activeobject
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "ActiveObject", Fsm::kInitialState);

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " enter state " << state << "\n"; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " exit state " << state << "\n"; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " state " << state << " handle event " << event << "\n"; };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " unhandled event " << event << " in state " << state << "\n"; };

    fsm_.Start();
}

void FsmImpl::State1Entry()
{
    std::cout << fsm_ << "State1Entry\n";
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " State2ToState1TransitionAction\n";
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " SomeGuardFunction\n";
    return true;
}
} // namespace example::activeobject

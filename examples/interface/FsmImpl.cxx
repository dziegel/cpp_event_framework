#include <iostream>

#include "FsmImpl.hxx"

namespace example::interface
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "FsmInterface", Fsm::kInitialState);

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " enter state " << state << std::endl; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " exit state " << state << std::endl; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " state " << state << " handle event " << static_cast<int>(event) << std::endl; };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " unhandled event " << static_cast<int>(event) << " in state " << state << std::endl; };

    fsm_.Start();
}

void FsmImpl::Run()
{
    fsm_.React(EEvent::kGo2);
    fsm_.React(EEvent::kGo1);
}

void FsmImpl::State1Entry()
{
    std::cout << fsm_ << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " State2ToState1TransitionAction" << std::endl;
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " SomeGuardFunction" << std::endl;
    return true;
}
} // namespace example::interface

void InterfaceStatemachineExampleMain()
{
    example::interface::FsmImpl s;
    s.Run();
}

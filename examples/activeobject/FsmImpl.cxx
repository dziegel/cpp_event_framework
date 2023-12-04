#include <iostream>
#include <memory>

#include "FsmImpl.hxx"

namespace example::activeobject
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "ActiveObject", Fsm::kInitialState);

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " enter state " << state.Name() << std::endl; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " exit state " << state.Name() << std::endl; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm.Name() << " state " << state.Name() << " handle event " << event->Name() << std::endl; };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm.Name() << " unhandled event " << event->Name() << " in state " << state.Name() << std::endl; };

    fsm_.Start();
}

void FsmImpl::State1Entry()
{
    std::cout << fsm_.Name() << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << fsm_.Name() << " State2ToState1TransitionAction" << std::endl;
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << fsm_.Name() << " SomeGuardFunction" << std::endl;
    return true;
}
} // namespace example::activeobject

#include <iostream>

#include "FsmImpl.hxx"

namespace example::interface
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "Fsm", Fsm::kInitialState);

    fsm_.on_state_entry_ = [this](Fsm::StateRef state)
    { std::cout << fsm_.Name() << " enter state " << state.Name() << std::endl; };

    fsm_.on_state_exit_ = [this](Fsm::StateRef state)
    { std::cout << fsm_.Name() << " exit state " << state.Name() << std::endl; };

    fsm_.on_handle_event_ = [this](Fsm::StateRef state, Fsm::Event event) {
        std::cout << fsm_.Name() << " state " << state.Name() << " handle event " << static_cast<int>(event)
                  << std::endl;
    };

    fsm_.on_unhandled_event_ = [this](Fsm::StateRef state, Fsm::Event event)
    {
        std::cout << fsm_.Name() << " unhandled event " << static_cast<int>(event) << " in state " << state.Name()
                  << std::endl;
    };

    fsm_.Start();
}

void FsmImpl::Run()
{
    fsm_.React(EEvent::kGo2);
    fsm_.React(EEvent::kGo1);
}

void FsmImpl::State1Entry()
{
    std::cout << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << "State2ToState1TransitionAction" << std::endl;
}
}

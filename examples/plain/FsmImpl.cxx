#include <iostream>

#include "FsmImpl.hxx"

namespace example::plain
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

FsmImpl::~FsmImpl() = default;

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

// FSM code

Fsm::Transition Fsm::State1Handler(Fsm::ImplPtr /* impl */, Fsm::Event event)
{
    switch (event)
    {
    case EEvent::kGo2:
        return Fsm::TransitionTo(Fsm::kState2);
    default:
        return Fsm::NoTransition();
    }
}

Fsm::Transition Fsm::State2Handler(Fsm::ImplPtr /* impl */, Fsm::Event event)
{
    switch (event)
    {
    case EEvent::kGo1:
        return kState2State1Transition;
    default:
        return Fsm::UnhandledEvent();
    }
}
    
const Fsm::State Fsm::kState1("State1", &Fsm::State1Handler);
const Fsm::State Fsm::kState2("State2", &Fsm::State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &Fsm::kState1; // initial state of the statemachine
const Fsm::Transition Fsm::kState2State1Transition(Fsm::kState1, &Fsm::Impl::State2ToState1TransitionAction);
}

void SimpleStatemachineExampleMain()
{
    example::plain::FsmImpl s;
    s.Run();
}

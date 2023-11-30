#include <iostream>

#include "FsmImpl.hxx"

namespace example::plain
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "Fsm", Fsm::kInitialState);

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " enter state " << state.Name() << std::endl; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " exit state " << state.Name() << std::endl; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event) {
        std::cout << fsm.Name() << " state " << state.Name() << " handle event " << static_cast<int>(event)
                  << std::endl;
    };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    {
        std::cout << fsm.Name() << " unhandled event " << static_cast<int>(event) << " in state " << state.Name()
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

Fsm::Transition Fsm::State1Handler(ImplPtr /* impl */, Event event)
{
    switch (event)
    {
    case EEvent::kGo2:
        return TransitionTo(kState2);
    default:
        return NoTransition();
    }
}

Fsm::Transition Fsm::State2Handler(ImplPtr /* impl */, Event event)
{
    switch (event)
    {
    case EEvent::kGo1:
        return kState2State1Transition;
    default:
        return UnhandledEvent();
    }
}

const Fsm::State Fsm::kState1("State1", &State1Handler);
const Fsm::State Fsm::kState2("State2", &State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &kState1; // initial state of the statemachine
const Fsm::Transition Fsm::kState2State1Transition(kState1, &Fsm::Impl::State2ToState1TransitionAction);
} // namespace example::plain

void SimpleStatemachineExampleMain()
{
    example::plain::FsmImpl s;
    s.Run();
}

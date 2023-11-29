#include "Fsm.hxx"

namespace example::pimpl
{
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

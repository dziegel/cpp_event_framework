#include "Fsm.hxx"

namespace example::pimpl
{
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

Fsm::Transition Fsm::State2Handler(ImplPtr impl, Event event)
{
    switch (event)
    {
    case EEvent::kGo1:
        if (impl->SomeGuardFunction(event))
        {
            return TransitionTo(kState1, &Impl::State2ToState1TransitionAction);
        }
        return NoTransition();
    default:
        return UnhandledEvent();
    }
}

const Fsm::State Fsm::kState1("State1", &State1Handler);
const Fsm::State Fsm::kState2("State2", &State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &kState1; // initial state of the statemachine
} // namespace example::pimpl

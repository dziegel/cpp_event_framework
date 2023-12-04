#include "Fsm.hxx"

namespace example::activeobject
{
Fsm::Transition Fsm::State1Handler(ImplPtr /* impl */, Event event)
{
    switch (event->Id())
    {
    case Go2::kId:
        return TransitionTo(kState2);
    default:
        return NoTransition();
    }
}

Fsm::Transition Fsm::State2Handler(ImplPtr impl, Event event)
{
    switch (event->Id())
    {
    case Go1::kId:
        if (impl->SomeGuardFunction(event))
        {
            return kState2State1Transition;
        }
        return NoTransition();
    default:
        return UnhandledEvent();
    }
}

const Fsm::State Fsm::kState1("State1", &State1Handler);
const Fsm::State Fsm::kState2("State2", &State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &kState1; // initial state of the statemachine
const Fsm::Transition Fsm::kState2State1Transition(kState1, &Impl::State2ToState1TransitionAction);
} // namespace example::activeobject

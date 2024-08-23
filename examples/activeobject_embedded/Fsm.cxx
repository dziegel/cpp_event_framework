#include "Fsm.hxx"

namespace example::activeobject_embedded
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
            return TransitionTo(kState1, &Impl::State2ToState1TransitionAction);
        }
        return NoTransition();
    default:
        return UnhandledEvent();
    }
}

const Fsm::State Fsm::kState1("State1", &State1Handler);
const Fsm::State Fsm::kState2("State2", &State2Handler);
} // namespace example::activeobject_embedded

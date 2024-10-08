#include "Fsm.hxx"

namespace example::activeobject
{
static Fsm::Transition State1Handler(Fsm::ImplPtr /* impl */, Fsm::Event event)
{
    switch (event->Id())
    {
    case Go2::kId:
        return Fsm::TransitionTo(Fsm::kState2);
    default:
        return Fsm::NoTransition();
    }
}

static Fsm::Transition State2Handler(Fsm::ImplPtr impl, Fsm::Event event)
{
    switch (event->Id())
    {
    case Go1::kId:
        if (impl->SomeGuardFunction(event))
        {
            return Fsm::TransitionTo(Fsm::kState1, &Fsm::Impl::State2ToState1TransitionAction);
        }
        return Fsm::NoTransition();
    default:
        return Fsm::UnhandledEvent();
    }
}

const Fsm::State Fsm::kState1("State1", &State1Handler);
const Fsm::State Fsm::kState2("State2", &State2Handler);
} // namespace example::activeobject

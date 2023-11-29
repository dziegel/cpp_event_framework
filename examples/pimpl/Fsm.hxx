#pragma once

#include "FsmImpl.hxx"

namespace example::pimpl
{
class Fsm : public FsmBase
{
public:
    // States
    static const State kState1;
    static const State kState2;
    static const StatePtr kInitialState;

    // Transitions
    static const Transition kState2State1Transition;

    // Handlers
    static Fsm::Transition State1Handler(Fsm::ImplPtr impl, Fsm::Event event);
    static Fsm::Transition State2Handler(Fsm::ImplPtr impl, Fsm::Event event);
};
}

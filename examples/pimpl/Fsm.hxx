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

    // Handlers
    static Transition State1Handler(ImplPtr impl, Event event);
    static Transition State2Handler(ImplPtr impl, Event event);
};
} // namespace example::pimpl

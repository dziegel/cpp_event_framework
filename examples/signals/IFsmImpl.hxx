#pragma once

#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>

namespace example::signals
{
class Go1 : public cpp_event_framework::SignalBase<Go1, 0>
{
};

class Go2 : public cpp_event_framework::NextSignal<Go2, Go1>
{
};

class IFsmImpl;
using FsmBase = cpp_event_framework::Statemachine<IFsmImpl, const cpp_event_framework::Signal::SPtr&>;

// Interface to implementation, contains entry/exit/transition actions
class IFsmImpl
{
public:
    virtual void State1Entry() = 0;

    virtual void State2ToState1TransitionAction(FsmBase::Event) = 0;

    virtual bool SomeGuardFunction(FsmBase::Event) = 0;
};
} // namespace example::signals
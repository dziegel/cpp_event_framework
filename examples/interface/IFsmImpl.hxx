#pragma once

#include <cpp_event_framework/Statemachine.hxx>

namespace example::interface
{
enum class EEvent : uint32_t
{
    kGo1,
    kGo2
};

class IFsmImpl;
using FsmBase = cpp_event_framework::Statemachine<IFsmImpl, EEvent>;

// Interface to implementation, contains entry/exit/transition actions
class IFsmImpl
{
public:
    virtual void State1Entry() = 0;

    virtual void State2ToState1TransitionAction(FsmBase::Event) = 0;
};
} // namespace example::interface
#pragma once

#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>

namespace example::activeobject_embedded
{
// Declare pool allocator
class EventPoolAllocator : public cpp_event_framework::CustomAllocator<EventPoolAllocator>
{
};

// Declare signals that allocate via EventPoolAllocator
class Go1 : public cpp_event_framework::SignalBase<Go1, 0, cpp_event_framework::Signal, EventPoolAllocator>
{
};

class Go2 : public cpp_event_framework::NextSignal<Go2, Go1>
{
};

// Calculate pool element size (max size)
using EventPoolElementSizeCalculator = cpp_event_framework::SignalPoolElementSizeCalculator<Go1, Go2>;

class IFsmImpl;
using FsmBase = cpp_event_framework::Statemachine<IFsmImpl, const cpp_event_framework::Signal::SPtr&>;

// Interface to implementation, contains entry/exit/transition actions
class IFsmImpl
{
public:
    virtual ~IFsmImpl() = default;

    virtual void State1Entry() = 0;

    virtual void State2ToState1TransitionAction(FsmBase::Event event) = 0;

    virtual bool SomeGuardFunction(FsmBase::Event event) = 0;
};
} // namespace example::activeobject_embedded
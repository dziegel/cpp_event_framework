#pragma once

#include <cpp_event_framework/Statemachine.hxx>

namespace example::plain
{
enum class EEvent : uint32_t
{
    kGo1,
    kGo2
};

class FsmImpl;
using FsmBase = cpp_event_framework::Statemachine<FsmImpl, EEvent>;

class Fsm : public FsmBase
{
public:
    // States
    static const State kState1;
    static const State kState2;

    // Handlers
    static Transition State1Handler(ImplPtr impl, Event event);
    static Transition State2Handler(ImplPtr impl, Event event);

    inline void Start()
    {
        FsmBase::Start(&kState1);
    }
};

class FsmImpl
{
public:
    FsmImpl();
    ~FsmImpl();

    void Run();

private:
    Fsm fsm_;

    // Give Fsm access to private entry/exit/transition actions
    // (This is not needed if actions are public functions)
    friend class Fsm;

    void State1Entry();

    void State2ToState1TransitionAction(FsmBase::Event event);

    bool SomeGuardFunction(FsmBase::Event event);
};
} // namespace example::plain

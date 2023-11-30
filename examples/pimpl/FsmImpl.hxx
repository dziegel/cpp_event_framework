#pragma once

#include <memory>

#include <cpp_event_framework/Statemachine.hxx>

namespace example::pimpl
{
enum class EEvent : uint32_t
{
    kGo1,
    kGo2
};

class FsmImpl;
using FsmBase = cpp_event_framework::Statemachine<FsmImpl, EEvent>;
class Fsm;

class FsmImpl
{
public:
    FsmImpl();
    ~FsmImpl();

    void Run();

private:
    struct Private;
    std::unique_ptr<Private> private_;

    // Give Fsm access to private entry/exit/transition actions
    // (This is not needed if actions are public functions)
    friend class Fsm;

    void State1Entry();

    void State2ToState1TransitionAction(FsmBase::Event);
};
} // namespace example::pimpl

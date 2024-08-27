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

// Needed for "friend class Fsm;" below
class Fsm;

class FsmImpl
{
public:
    FsmImpl();
    ~FsmImpl();

    void Run();

    void State1Entry();

    void State2ToState1TransitionAction(FsmBase::Event event);

    bool SomeGuardFunction(FsmBase::Event event);

private:
    struct Private;
    std::unique_ptr<Private> private_;
};
} // namespace example::pimpl

#include <iostream>

#include <cpp_event_framework/Pool.hxx>

#include "FsmImpl.hxx"

namespace example::signals
{
FsmImpl::FsmImpl()
{
    // Create a pool with elemen size calculated via EventPoolElementSizeCalculator, 10 elements
    auto pool = cpp_event_framework::Pool<>::MakeShared(EventPoolElementSizeCalculator::kSptrSize, 10, "EventPool");

    // Tell EventPoolAllocator to use pool created above
    EventPoolAllocator::SetAllocator(pool);

    fsm_.Init(this, "FsmSignals");

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " enter state " << state << "\n"; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " exit state " << state << "\n"; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " state " << state << " handle event " << event << "\n"; };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " unhandled event " << event << " in state " << state << "\n"; };

    fsm_.Start(Fsm::kInitialState);
}

void FsmImpl::Run()
{
    fsm_.React(Go2::MakeShared());
    fsm_.React(Go1::MakeShared());
}

void FsmImpl::State1Entry()
{
    std::cout << fsm_ << "State1Entry\n";
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " State2ToState1TransitionAction\n";
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " SomeGuardFunction\n";
    return true;
}
} // namespace example::signals

void SignalsStatemachineExampleMain()
{
    example::signals::FsmImpl s;
    s.Run();
}

#include <iostream>

#include "FsmImpl.hxx"

namespace example::signals
{
FsmImpl::FsmImpl()
{
    // Create a pool with elemen size calculated via PoolSizeCalculator, 10 elements
    auto pool = cpp_event_framework::Pool<>::MakeShared(PoolSizeCalculator::kSptrSize, 10, "EventPool");

    // Tell EventPoolAllocator to use pool created above
    EventPoolAllocator::SetPool(pool);

    fsm_.Init(this, "FsmSignals", Fsm::kInitialState);

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " enter state " << state.Name() << std::endl; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " exit state " << state.Name() << std::endl; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm.Name() << " state " << state.Name() << " handle event " << event->Name() << std::endl; };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm.Name() << " unhandled event " << event->Name() << " in state " << state.Name() << std::endl; };

    fsm_.Start();
}

void FsmImpl::Run()
{
    fsm_.React(Go2::MakeShared());
    fsm_.React(Go1::MakeShared());
}

void FsmImpl::State1Entry()
{
    std::cout << fsm_.Name() << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << fsm_.Name() << " State2ToState1TransitionAction" << std::endl;
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << fsm_.Name() << " SomeGuardFunction" << std::endl;
    return true;
}
} // namespace example::signals

void SignalsStatemachineExampleMain()
{
    example::signals::FsmImpl s;
    s.Run();
}

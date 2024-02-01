#include <iostream>

#include "FsmImpl.hxx"

namespace example::plain
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "FsmPlain");

    fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " enter state " << state << std::endl; };

    fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " exit state " << state << std::endl; };

    fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " state " << state << " handle event " << static_cast<int>(event) << std::endl; };

    fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " unhandled event " << static_cast<int>(event) << " in state " << state << std::endl; };

    fsm_.Start(Fsm::kInitialState);
}

FsmImpl::~FsmImpl() = default;

void FsmImpl::Run()
{
    fsm_.React(EEvent::kGo2);
    fsm_.React(EEvent::kGo1);
}

void FsmImpl::State1Entry()
{
    std::cout << fsm_ << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " State2ToState1TransitionAction" << std::endl;
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << fsm_ << " SomeGuardFunction" << std::endl;
    return true;
}

// FSM code

Fsm::Transition Fsm::State1Handler(ImplPtr /* impl */, Event event)
{
    switch (event)
    {
    case EEvent::kGo2:
        return TransitionTo(kState2);
    default:
        return NoTransition();
    }
}

Fsm::Transition Fsm::State2Handler(ImplPtr impl, Event event)
{
    switch (event)
    {
    case EEvent::kGo1:
        if (impl->SomeGuardFunction(event))
        {
            return kState2State1Transition;
        }
        return NoTransition();
    default:
        return UnhandledEvent();
    }
}

const Fsm::State Fsm::kState1("State1", &State1Handler);
const Fsm::State Fsm::kState2("State2", &State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &kState1; // initial state of the statemachine
const Fsm::Transition Fsm::kState2State1Transition(kState1, &Impl::State2ToState1TransitionAction);
} // namespace example::plain

void SimpleStatemachineExampleMain()
{
    example::plain::FsmImpl s;
    s.Run();
}

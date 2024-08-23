#include <iostream>

#include "Fsm.hxx"
#include "FsmImpl.hxx"

namespace example::pimpl
{
struct FsmImpl::Private
{
    Fsm fsm;
};

FsmImpl::FsmImpl() : private_(std::make_unique<FsmImpl::Private>())
{
    private_->fsm.Init(this, "FsmPimpl");

    private_->fsm.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " enter state " << state << std::endl; };

    private_->fsm.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm << " exit state " << state << std::endl; };

    private_->fsm.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " state " << state << " handle event " << static_cast<int>(event) << std::endl; };

    private_->fsm.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    { std::cout << fsm << " unhandled event " << static_cast<int>(event) << " in state " << state << std::endl; };

    private_->fsm.Start();
}

FsmImpl::~FsmImpl() = default;

void FsmImpl::Run()
{
    private_->fsm.React(EEvent::kGo2);
    private_->fsm.React(EEvent::kGo1);
}

void FsmImpl::State1Entry()
{
    std::cout << private_->fsm << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << private_->fsm << " State2ToState1TransitionAction" << std::endl;
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    std::cout << private_->fsm << " SomeGuardFunction" << std::endl;
    return true;
}
} // namespace example::pimpl

void PimplStatemachineExampleMain()
{
    example::pimpl::FsmImpl s;
    s.Run();
}

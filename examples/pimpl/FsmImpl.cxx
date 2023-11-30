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
    private_->fsm.Init(this, "Fsm", Fsm::kInitialState);

    private_->fsm.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " enter state " << state.Name() << std::endl; };

    private_->fsm.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
    { std::cout << fsm.Name() << " exit state " << state.Name() << std::endl; };

    private_->fsm.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event) {
        std::cout << fsm.Name() << " state " << state.Name() << " handle event " << static_cast<int>(event)
                  << std::endl;
    };

    private_->fsm.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
    {
        std::cout << fsm.Name() << " unhandled event " << static_cast<int>(event) << " in state " << state.Name()
                  << std::endl;
    };

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
    std::cout << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << "State2ToState1TransitionAction" << std::endl;
}
} // namespace example::pimpl

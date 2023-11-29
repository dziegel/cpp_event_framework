#include <iostream>

#include "FsmImpl.hxx"
#include "Fsm.hxx"

namespace example::pimpl
{
struct FsmImpl::Private
{
    Fsm fsm;
};

FsmImpl::FsmImpl() : private_(std::make_unique<FsmImpl::Private>())
{
    private_->fsm.Init(this, "Fsm", Fsm::kInitialState);

    private_->fsm.on_state_entry_ = [this](Fsm::StateRef state)
    { std::cout << private_->fsm.Name() << " enter state " << state.Name() << std::endl; };

    private_->fsm.on_state_exit_ = [this](Fsm::StateRef state)
    { std::cout << private_->fsm.Name() << " exit state " << state.Name() << std::endl; };

    private_->fsm.on_handle_event_ = [this](Fsm::StateRef state, Fsm::Event event) {
        std::cout << private_->fsm.Name() << " state " << state.Name() << " handle event " << static_cast<int>(event)
                  << std::endl;
    };

    private_->fsm.on_unhandled_event_ = [this](Fsm::StateRef state, Fsm::Event event)
    {
        std::cout << private_->fsm.Name() << " unhandled event " << static_cast<int>(event) << " in state " << state.Name()
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
}

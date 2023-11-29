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
    private_->fsm.Start();
}

FsmImpl::~FsmImpl() = default;

void FsmImpl::State1Entry()
{
    std::cout << "State1Entry" << std::endl;
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    std::cout << "State2ToState1TransitionAction" << std::endl;
   
}
}

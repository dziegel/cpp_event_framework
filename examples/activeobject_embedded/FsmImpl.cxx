#include "FsmImpl.hxx"

namespace example::activeobject_embedded
{
FsmImpl::FsmImpl()
{
    fsm_.Init(this, "ActiveObjectEmbedded");

    // No logging because printf or std::ostream use heap memory

    fsm_.Start(Fsm::kInitialState);
}

void FsmImpl::State1Entry()
{
    // No logging because printf or std::ostream use heap memory
}

void FsmImpl::State2ToState1TransitionAction(FsmBase::Event /*event*/)
{
    // No logging because printf or std::ostream use heap memory
}

bool FsmImpl::SomeGuardFunction(FsmBase::Event /*event*/)
{
    // No logging because printf or std::ostream use heap memory
    return true;
}
} // namespace example::activeobject_embedded

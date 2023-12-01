#include <iostream>
#include <memory>

#include "../examples/signals/Fsm.hxx"
#include "../examples/signals/IFsmImpl.hxx"

#include <experimental/Hsm.hxx>
#include <experimental/ThreadedActiveObjectDomain.hxx>

using namespace std::chrono_literals;

class AnFsm : public cpp_event_framework::Hsm<example::signals::Fsm>, public example::signals::IFsmImpl
{
public:
    AnFsm()
    {
        fsm_.Init(this, "ActiveObject", example::signals::Fsm::kInitialState);

        fsm_.on_state_entry_ = [](example::signals::Fsm::Ref fsm, example::signals::Fsm::StateRef state)
        { std::cout << fsm.Name() << " enter state " << state.Name() << std::endl; };

        fsm_.on_state_exit_ = [](example::signals::Fsm::Ref fsm, example::signals::Fsm::StateRef state)
        { std::cout << fsm.Name() << " exit state " << state.Name() << std::endl; };

        fsm_.on_handle_event_ = [](example::signals::Fsm::Ref fsm, example::signals::Fsm::StateRef state,
                                   example::signals::Fsm::Event event)
        { std::cout << fsm.Name() << " state " << state.Name() << " handle event " << event->Name() << std::endl; };

        fsm_.on_unhandled_event_ = [](example::signals::Fsm::Ref fsm, example::signals::Fsm::StateRef state,
                                      example::signals::Fsm::Event event) {
            std::cout << fsm.Name() << " unhandled event " << event->Name() << " in state " << state.Name()
                      << std::endl;
        };

        fsm_.Start();
    }

    void State1Entry() override
    {
        std::cout << fsm_.Name() << "State1Entry" << std::endl;
    }

    void State2ToState1TransitionAction(example::signals::FsmBase::Event /*event*/) override
    {
        std::cout << fsm_.Name() << " State2ToState1TransitionAction" << std::endl;
    }

    bool SomeGuardFunction(example::signals::FsmBase::Event /*event*/) override
    {
        std::cout << fsm_.Name() << " SomeGuardFunction" << std::endl;
        return true;
    }

    example::signals::FsmBase::StatePtr CurrentState()
    {
        return fsm_.CurrentState();
    }
};

void ActiveObjectFrameworkMain()
{
    auto domain = std::make_shared<cpp_event_framework::ThreadedActiveObjectDomain>();

    auto fsm = std::make_shared<AnFsm>();
    domain->RegisterObject(fsm);

    assert(fsm->CurrentState() == &example::signals::Fsm::kState1);
    fsm->Take(example::signals::Go2::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(fsm->CurrentState() == &example::signals::Fsm::kState2);
    fsm->Take(example::signals::Go1::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(fsm->CurrentState() == &example::signals::Fsm::kState1);
}

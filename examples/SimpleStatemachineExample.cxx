#include <cstdint>
#include <iostream>

#include <cpp_event_framework/Statemachine.hxx>

enum class EEvent : uint32_t
{
    kGo1,
    kGo2
};

class ClassContainingAStatemachine;
class Fsm : public cpp_event_framework::Statemachine<ClassContainingAStatemachine, EEvent>
{
public:
    static const State kState1;
    static const State kState2;
    static const StatePtr kInitialState;
};

class ClassContainingAStatemachine
{
public:
    ClassContainingAStatemachine()
    {
        fsm_.Init(this, "Fsm", Fsm::kInitialState);
        fsm_.on_state_entry_ = [this](Fsm::StatePtr state)
        { std::cout << fsm_.Name() << " enter state " << state->name_ << std::endl; };

        fsm_.on_state_exit_ = [this](Fsm::StatePtr state)
        { std::cout << fsm_.Name() << " exit state " << state->name_ << std::endl; };

        fsm_.on_handle_event_ = [this](Fsm::StatePtr state, Fsm::Event event) {
            std::cout << fsm_.Name() << " state " << state->name_ << " handle event " << static_cast<int>(event)
                      << std::endl;
        };

        fsm_.on_unhandled_event_ = [this](Fsm::Event event)
        { std::cout << fsm_.Name() << " unhandled event " << static_cast<int>(event) << std::endl; };

        fsm_.Start();
    }

    void Run()
    {
        fsm_.React(EEvent::kGo2);
        fsm_.React(EEvent::kGo1);
    }

private:
    friend class Fsm;
    Fsm fsm_;

    Fsm::Transition State1Handler(Fsm::StatePtr /* state */, Fsm::Event event)
    {
        switch (event)
        {
        case EEvent::kGo2:
            return Fsm::TransitionTo(Fsm::kState2);
        default:
            return Fsm::NoTransition();
        }
    }

    Fsm::Transition State2Handler(Fsm::StatePtr /* state */, Fsm::Event event)
    {
        switch (event)
        {
        case EEvent::kGo1:
            return Fsm::TransitionTo(Fsm::kState1,
                                     [](Fsm::OwnerPtr, Fsm::Event) { std::cout << "Transition action" << std::endl; });
        default:
            return Fsm::UnhandledEvent();
        }
    }
};

const Fsm::State Fsm::kState1("State1", &Fsm::Owner::State1Handler);
const Fsm::State Fsm::kState2("State2", &Fsm::Owner::State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &Fsm::kState1; // initial state of the statemachine

void SimpleStatemachineExampleMain()
{
    ClassContainingAStatemachine s;
    s.Run();
}

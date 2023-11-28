#include <cstdint>
#include <iostream>

#include <cpp_event_framework/Statemachine.hxx>

enum class EEvent : uint32_t
{
    kGo1,
    kGo2
};

class StatemachineImplementation;
class Fsm : public cpp_event_framework::Statemachine<StatemachineImplementation, EEvent>
{
public:
    static const State kState1;
    static const State kState2;
    static const StatePtr kInitialState;

    static Fsm::Transition State1Handler(Fsm::ImplPtr /* impl */, Fsm::Event event)
    {
        switch (event)
        {
        case EEvent::kGo2:
            return Fsm::TransitionTo(Fsm::kState2);
        default:
            return Fsm::NoTransition();
        }
    }

    static Fsm::Transition State2Handler(Fsm::ImplPtr /* impl */, Fsm::Event event)
    {
        switch (event)
        {
        case EEvent::kGo1:
            return Fsm::TransitionTo(Fsm::kState1,
                                     [](Fsm::ImplPtr, Fsm::Event) { std::cout << "Transition action" << std::endl; });
        default:
            return Fsm::UnhandledEvent();
        }
    }
};

class StatemachineImplementation
{
public:
    StatemachineImplementation()
    {
        fsm_.Init(this, "Fsm", Fsm::kInitialState);
        fsm_.on_state_entry_ = [this](Fsm::StateRef state)
        { std::cout << fsm_.Name() << " enter state " << state.Name() << std::endl; };

        fsm_.on_state_exit_ = [this](Fsm::StateRef state)
        { std::cout << fsm_.Name() << " exit state " << state.Name() << std::endl; };

        fsm_.on_handle_event_ = [this](Fsm::StateRef state, Fsm::Event event) {
            std::cout << fsm_.Name() << " state " << state.Name() << " handle event " << static_cast<int>(event)
                      << std::endl;
        };

        fsm_.on_unhandled_event_ = [this](Fsm::StateRef state, Fsm::Event event)
        {
            std::cout << fsm_.Name() << " unhandled event " << static_cast<int>(event) << " in state " << state.Name()
                      << std::endl;
        };

        fsm_.Start();
    }

    void Run()
    {
        fsm_.React(EEvent::kGo2);
        fsm_.React(EEvent::kGo1);
    }

private:
    // Allow private functions of class StatemachineImplementation to be used by FSM
    friend class Fsm;

    // Implementation can aggregate the statemachine!
    Fsm fsm_;
};

const Fsm::State Fsm::kState1("State1", &Fsm::State1Handler);
const Fsm::State Fsm::kState2("State2", &Fsm::State2Handler);
const Fsm::StatePtr Fsm::kInitialState = &Fsm::kState1; // initial state of the statemachine

void SimpleStatemachineExampleMain()
{
    StatemachineImplementation s;
    s.Run();
}

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
                                     [](Fsm::Owner*, Fsm::Event) { std::cout << "Transition action" << std::endl; });
        default:
            return Fsm::UnhandledEvent();
        }
    }
};

const Fsm::State Fsm::kState1("State1", std::mem_fn(&Fsm::Owner::State1Handler));
const Fsm::State Fsm::kState2("State2", std::mem_fn(&Fsm::Owner::State2Handler));
const Fsm::StatePtr Fsm::kInitialState = &Fsm::kState1; // initial state of the statemachine

#include <iostream>
#include <ostream>

#include <cpp_event_framework/Pool.hxx>
#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>

class EvtGoYellow : public cpp_event_framework::SignalBase<EvtGoYellow, 0>
{
};

class EvtGoRed : public cpp_event_framework::NextSignal<EvtGoRed, EvtGoYellow>
{
};

class EvtGoGreen : public cpp_event_framework::NextSignal<EvtGoGreen, EvtGoRed>
{
};

class EvtTurnOn : public cpp_event_framework::NextSignal<EvtTurnOn, EvtGoGreen>
{
};

class EvtTurnOff : public cpp_event_framework::NextSignal<EvtTurnOff, EvtTurnOn>
{
};

struct StatemachineFixture;
class Fsm : public cpp_event_framework::Statemachine<StatemachineFixture, const cpp_event_framework::Signal::SPtr&>
{
public:
    static const State kOff;
    static const HistoryState kOn;
    static const State kGreen;
    static const State kYellow;
    static const State kRed;
    static const State kRedYellow;

    static const StatePtr kInitial;

    static const Transition::ActionContainer<2> kYellowRedTransitionActions;
    static const Transition kYellowRedTransition;
};

struct StatemachineFixture
{
    void SetUp()
    {
        fsm_.on_state_entry_ = [this](Fsm::StatePtr state)
        { std::cout << fsm_.Name() << " enter state " << state->Name() << std::endl; };

        fsm_.on_state_exit_ = [this](Fsm::StatePtr state)
        { std::cout << fsm_.Name() << " exit state " << state->Name() << std::endl; };

        fsm_.on_handle_event_ = [this](Fsm::StatePtr state, Fsm::Event event)
        { std::cout << fsm_.Name() << " state " << state->Name() << " handle event " << event->Name() << std::endl; };

        fsm_.on_unhandled_event_ = [this](Fsm::Event event)
        {
            on_unhandled_event_called_ = true;
            std::cout << fsm_.Name() << " unhandled event " << event->Name() << std::endl;
        };

        fsm_.Init(this, "Fsm", Fsm::kInitial);
    }

private:
    friend class Fsm; // provide access to private functions of this class
    Fsm fsm_;

    bool off_entry_called_ = false;
    bool off_exit_called_ = false;
    bool on_entry_called_ = false;
    bool on_exit_called_ = false;
    bool yellow_red_transition1_called_ = false;
    bool yellow_red_transition2_called_ = false;
    bool on_unhandled_event_called_ = false;

    void CheckAllFalse() const
    {
        assert(off_entry_called_ == false);
        assert(off_exit_called_ == false);
        assert(on_entry_called_ == false);
        assert(on_exit_called_ == false);
        assert(yellow_red_transition1_called_ == false);
        assert(yellow_red_transition2_called_ == false);
        assert(on_unhandled_event_called_ == false);
    }

    void FsmOffEntry(Fsm::StatePtr /*state*/)
    {
        off_entry_called_ = true;
        std::cout << "Off entry" << std::endl;
    }

    void FsmOffExit(Fsm::StatePtr /*state*/)
    {
        off_exit_called_ = true;
        std::cout << "Off exit" << std::endl;
    }

    void FsmOnEntry(Fsm::StatePtr /*state*/)
    {
        on_entry_called_ = true;
        std::cout << "On entry" << std::endl;
    }

    void FsmOnExit(Fsm::StatePtr /*state*/)
    {
        on_exit_called_ = true;
        std::cout << "On exit" << std::endl;
    }

    Fsm::Transition FsmOffHandler(Fsm::StatePtr /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtTurnOn::kId:
            return Fsm::TransitionTo(Fsm::kOn);
        case EvtTurnOff::kId:
            return Fsm::NoTransition();
        case EvtGoYellow::kId: // fall through
        case EvtGoRed::kId:
            return Fsm::DeferEvent();
        default:
            return Fsm::UnhandledEvent();
        }
    }

    Fsm::Transition FsmOnHandler(Fsm::StatePtr /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtTurnOff::kId:
            return Fsm::TransitionTo(Fsm::kOff);
        case EvtTurnOn::kId:
            return Fsm::NoTransition();
        default:
            return Fsm::UnhandledEvent();
        }
    }

    Fsm::Transition FsmGreenHandler(Fsm::StatePtr /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtGoYellow::kId:
            return Fsm::TransitionTo(Fsm::kYellow);
        case EvtGoGreen::kId:
            return Fsm::NoTransition();
        default:
            return Fsm::UnhandledEvent();
        }
    }

    Fsm::Transition FsmYellowHandler(Fsm::StatePtr /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtGoRed::kId:
            return Fsm::kYellowRedTransition;
        case EvtGoYellow::kId:
            return Fsm::NoTransition();
        default:
            return Fsm::UnhandledEvent();
        }
    }

    void FsmYellowRedTransitionAction1(Fsm::Event /*event*/)
    {
        yellow_red_transition1_called_ = true;
        std::cout << "Don't walk 1" << std::endl;
    }

    void FsmYellowRedTransitionAction2(Fsm::Event /*event*/)
    {
        yellow_red_transition2_called_ = true;
        std::cout << "Don't walk 2" << std::endl;
    }

    Fsm::Transition FsmRedHandler(Fsm::StatePtr /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtGoYellow::kId:
            return Fsm::TransitionTo(Fsm::kRedYellow);
        case EvtGoRed::kId:
            return Fsm::NoTransition();
        default:
            return Fsm::UnhandledEvent();
        }
    }

    Fsm::Transition FsmRedYellowHandler(Fsm::StatePtr /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtGoGreen::kId:
            return Fsm::TransitionTo(Fsm::kGreen, [](Fsm::OwnerPtr /*owner*/, Fsm::Event /*event*/)
                                     { std::cout << "Walk" << std::endl; });
        case EvtGoYellow::kId:
            return Fsm::NoTransition();
        default:
            return Fsm::UnhandledEvent();
        }
    }

public:
    void Main()
    {
        CheckAllFalse();
        fsm_.Start();
        assert(fsm_.CurrentState() == &Fsm::kOff);
        assert(off_entry_called_ == true);
        off_entry_called_ = false;
        CheckAllFalse();

        fsm_.React(EvtTurnOn::MakeShared());
        assert(off_exit_called_ == true);
        assert(on_entry_called_ == true);
        assert(fsm_.CurrentState() == &Fsm::kGreen);
        off_exit_called_ = false;
        on_entry_called_ = false;
        CheckAllFalse();

        fsm_.React(EvtGoYellow::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kYellow);
        CheckAllFalse();

        fsm_.React(EvtGoRed::MakeShared());
        assert(yellow_red_transition1_called_ == true);
        assert(yellow_red_transition2_called_ == true);
        assert(fsm_.CurrentState() == &Fsm::kRed);
        yellow_red_transition1_called_ = false;
        yellow_red_transition2_called_ = false;
        CheckAllFalse();

        fsm_.React(EvtGoYellow::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kRedYellow);
        CheckAllFalse();

        fsm_.React(EvtGoGreen::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kGreen);
        CheckAllFalse();

        fsm_.React(EvtTurnOff::MakeShared());
        assert(on_exit_called_ == true);
        assert(off_entry_called_ == true);
        assert(fsm_.CurrentState() == &Fsm::kOff);
        on_exit_called_ = false;
        off_entry_called_ = false;
        CheckAllFalse();

        fsm_.React(EvtGoGreen::MakeShared());
        assert(on_unhandled_event_called_ == true);
        on_unhandled_event_called_ = false;
        CheckAllFalse();
    }

    void History()
    {
        fsm_.Start();
        assert(fsm_.CurrentState() == &Fsm::kOff);

        fsm_.React(EvtTurnOn::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kGreen);

        fsm_.React(EvtGoYellow::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kYellow);

        fsm_.React(EvtTurnOff::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kOff);

        fsm_.React(EvtTurnOn::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kYellow);
    }
};

const Fsm::State Fsm::kOff("Off", &Fsm::Owner::FsmOffHandler, nullptr, nullptr, &Fsm::Owner::FsmOffEntry,
                           &Fsm::Owner::FsmOffExit);
const Fsm::HistoryState Fsm::kOn("On", &Fsm::Owner::FsmOnHandler, nullptr, &Fsm::kGreen, &Fsm::Owner::FsmOnEntry,
                                 &Fsm::Owner::FsmOnExit);
const Fsm::State Fsm::kGreen("Green", &Fsm::Owner::FsmGreenHandler, &Fsm::kOn);
const Fsm::State Fsm::kYellow("Yellow", &Fsm::Owner::FsmYellowHandler, &Fsm::kOn);
const Fsm::State Fsm::kRed("Red", &Fsm::Owner::FsmRedHandler, &Fsm::kOn);
const Fsm::State Fsm::kRedYellow("RedYellow", &Fsm::Owner::FsmRedYellowHandler, &Fsm::kOn);

const Fsm::StatePtr Fsm::kInitial = &Fsm::kOff;

const Fsm::Transition::ActionContainer<2> Fsm::kYellowRedTransitionActions = {
    &Fsm::Owner::FsmYellowRedTransitionAction1, &Fsm::Owner::FsmYellowRedTransitionAction2};
const Fsm::Transition Fsm::kYellowRedTransition(kRed, Fsm::kYellowRedTransitionActions);

void StatemachineFixtureMain()
{
    StatemachineFixture f;

    f.SetUp();
    f.Main();

    f.SetUp();
    f.History();
}

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

class StatemachineFixture;
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

class StatemachineFixture
{
public:
    void SetUp()
    {
        fsm_.on_state_entry_ = [this](Fsm::StateRef state)
        { std::cout << fsm_.Name() << " enter state " << state.Name() << std::endl; };

        fsm_.on_state_exit_ = [this](Fsm::StateRef state)
        { std::cout << fsm_.Name() << " exit state " << state.Name() << std::endl; };

        fsm_.on_handle_event_ = [this](Fsm::StateRef state, Fsm::Event event)
        { std::cout << fsm_.Name() << " state " << state.Name() << " handle event " << event->Name() << std::endl; };

        fsm_.on_defer_event_ = [this](Fsm::StateRef state, Fsm::Event event)
        {
            on_defer_event_called_ = true;
            std::cout << fsm_.Name() << " state " << state.Name() << " defer event " << event->Name() << std::endl;
        };

        fsm_.on_recall_deferred_events_ = [this](Fsm::StateRef state)
        {
            on_recall_event_called_ = true;
            std::cout << fsm_.Name() << " state " << state.Name() << " recall deferred events" << std::endl;
        };

        fsm_.on_unhandled_event_ = [this](Fsm::StateRef state, Fsm::Event event)
        {
            on_unhandled_event_called_ = true;
            std::cout << fsm_.Name() << " unhandled event " << event->Name() << " in state " << state.Name()
                      << std::endl;
        };

        fsm_.Init(this, "Fsm", Fsm::kInitial);
    }

private:
    // Allow private functions of class StatemachineFixture to be used by FSM
    friend class Fsm;

    // Implementation can aggregate the statemachine!
    Fsm fsm_;

    bool off_entry_called_ = false;
    bool off_exit_called_ = false;
    bool on_entry_called_ = false;
    bool on_exit_called_ = false;
    bool yellow_red_transition1_called_ = false;
    bool yellow_red_transition2_called_ = false;
    bool on_unhandled_event_called_ = false;
    bool on_defer_event_called_ = false;
    bool on_recall_event_called_ = false;

    void CheckAllFalse() const
    {
        assert(off_entry_called_ == false);
        assert(off_exit_called_ == false);
        assert(on_entry_called_ == false);
        assert(on_exit_called_ == false);
        assert(yellow_red_transition1_called_ == false);
        assert(yellow_red_transition2_called_ == false);
        assert(on_unhandled_event_called_ == false);
        assert(on_defer_event_called_ == false);
        assert(on_recall_event_called_ == false);
    }

    void FsmOffEntry(Fsm::StateRef /*state*/)
    {
        off_entry_called_ = true;
        std::cout << "Off entry" << std::endl;
    }

    void FsmOffExit(Fsm::StateRef /*state*/)
    {
        off_exit_called_ = true;
        std::cout << "Off exit" << std::endl;
    }

    void FsmOnEntry(Fsm::StateRef /*state*/)
    {
        on_entry_called_ = true;
        std::cout << "On entry" << std::endl;
    }

    void FsmOnExit(Fsm::StateRef /*state*/)
    {
        on_exit_called_ = true;
        std::cout << "On exit" << std::endl;
    }

    Fsm::Transition FsmOffHandler(Fsm::StateRef /*state*/, Fsm::Event event)
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

    Fsm::Transition FsmOnHandler(Fsm::StateRef /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtTurnOff::kId:
            return Fsm::TransitionTo(Fsm::kOff);
        case EvtTurnOn::kId:
            fsm_.RecallEvents();
            return Fsm::NoTransition();
        default:
            return Fsm::UnhandledEvent();
        }
    }

    Fsm::Transition FsmGreenHandler(Fsm::StateRef /*state*/, Fsm::Event event)
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

    Fsm::Transition FsmYellowHandler(Fsm::StateRef /*state*/, Fsm::Event event)
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

    Fsm::Transition FsmRedHandler(Fsm::StateRef /*state*/, Fsm::Event event)
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

    Fsm::Transition FsmRedYellowHandler(Fsm::StateRef /*state*/, Fsm::Event event)
    {
        switch (event->Id())
        {
        case EvtGoGreen::kId:
            return Fsm::TransitionTo(Fsm::kGreen, [](Fsm::ImplPtr /*impl*/, Fsm::Event /*event*/)
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

        fsm_.React(EvtTurnOn::MakeShared());
        assert(on_recall_event_called_ == true);
        assert(fsm_.CurrentState() == &Fsm::kGreen);
        on_recall_event_called_ = false;
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

        fsm_.React(EvtGoRed::MakeShared());
        assert(on_defer_event_called_ == true);
        on_defer_event_called_ = false;
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

const Fsm::State Fsm::kOff("Off", &Fsm::Impl::FsmOffHandler, nullptr, nullptr, &Fsm::Impl::FsmOffEntry,
                           &Fsm::Impl::FsmOffExit);
const Fsm::HistoryState Fsm::kOn("On", &Fsm::Impl::FsmOnHandler, nullptr, &Fsm::kGreen, &Fsm::Impl::FsmOnEntry,
                                 &Fsm::Impl::FsmOnExit);
const Fsm::State Fsm::kGreen("Green", &Fsm::Impl::FsmGreenHandler, &Fsm::kOn);
const Fsm::State Fsm::kYellow("Yellow", &Fsm::Impl::FsmYellowHandler, &Fsm::kOn);
const Fsm::State Fsm::kRed("Red", &Fsm::Impl::FsmRedHandler, &Fsm::kOn);
const Fsm::State Fsm::kRedYellow("RedYellow", &Fsm::Impl::FsmRedYellowHandler, &Fsm::kOn);

const Fsm::StatePtr Fsm::kInitial = &Fsm::kOff;

const Fsm::Transition::ActionContainer<2> Fsm::kYellowRedTransitionActions = {
    &Fsm::Impl::FsmYellowRedTransitionAction1, &Fsm::Impl::FsmYellowRedTransitionAction2};
const Fsm::Transition Fsm::kYellowRedTransition(kRed, Fsm::kYellowRedTransitionActions);

void StatemachineFixtureMain()
{
    StatemachineFixture f;

    f.SetUp();
    f.Main();

    f.SetUp();
    f.History();
}

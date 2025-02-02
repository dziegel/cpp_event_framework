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

class EvtSelfTransition : public cpp_event_framework::NextSignal<EvtSelfTransition, EvtTurnOff>
{
};

class StatemachineImpl;
class Fsm : public cpp_event_framework::Statemachine<StatemachineImpl, const cpp_event_framework::Signal::SPtr&>
{
public:
    static const State kOff;
    static const HistoryState kOn;
    static const State kGreen;
    static const State kYellow;
    static const State kRed;
    static const State kRedYellow;

private:
    static const std::array<State::EntryExitType, 2> FsmOffEntryActions;
    static const std::array<State::EntryExitType, 2> FsmOffExitActions;

    static Transition FsmOffHandler(ImplPtr /*impl*/, Event event);

    static Transition FsmOnHandler(ImplPtr /*impl*/, Event event);

    static Transition FsmGreenHandler(ImplPtr /*impl*/, Event event);

    static Transition FsmYellowHandler(ImplPtr /*impl*/, Event event);

    static Transition FsmRedHandler(ImplPtr /*impl*/, Event event);

    static Transition FsmRedYellowHandler(ImplPtr /*impl*/, Event event);
};

class StatemachineImpl
{
public:
    void SetUp()
    {
        fsm_.on_state_change_ = [](Fsm::Ref fsm, Fsm::Event event, Fsm::StateRef old_state, Fsm::StateRef new_state)
        { std::cout << fsm << " state changed " << old_state << " --- " << event << " ---> " << new_state << "\n"; };

        fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
        { std::cout << fsm << " enter state " << state << "\n"; };

        fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
        { std::cout << fsm << " exit state " << state << "\n"; };

        fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
        { std::cout << fsm << " state " << state << " handle event " << event << "\n"; };

        fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
        {
            fsm.Implementation()->on_unhandled_event_called_ = true;
            std::cout << fsm << " unhandled event " << event << " in state " << state << "\n";
        };

        fsm_.on_defer_event_ = [this](Fsm::StateRef state, Fsm::Event event)
        {
            on_defer_event_called_ = true;
            std::cout << "state " << state << " defer event " << event << "\n";
        };

        fsm_.on_recall_deferred_events_ = [this](Fsm::StateRef state)
        {
            on_recall_event_called_ = true;
            std::cout << "state " << state << " recall deferred events\n";
        };

        fsm_.Init(this, "Fsm");
    }

    bool off_entry_called_ = false;
    bool off_entry2_called_ = false;
    bool off_exit_called_ = false;
    bool off_exit2_called_ = false;
    bool on_entry_called_ = false;
    bool on_exit_called_ = false;
    bool yellow_red_transition1_called_ = false;
    bool yellow_red_transition2_called_ = false;
    bool on_unhandled_event_called_ = false;
    bool on_defer_event_called_ = false;
    bool on_recall_event_called_ = false;

private:
    // Allow private functions of class StatemachineFixture to be used by FSM
    friend class Fsm;

    // Implementation can aggregate the statemachine!
    Fsm fsm_;

    void CheckAllFalse() const
    {
        assert(off_entry_called_ == false);
        assert(off_entry2_called_ == false);
        assert(off_exit_called_ == false);
        assert(off_exit2_called_ == false);
        assert(on_entry_called_ == false);
        assert(on_exit_called_ == false);
        assert(yellow_red_transition1_called_ == false);
        assert(yellow_red_transition2_called_ == false);
        assert(on_unhandled_event_called_ == false);
        assert(on_defer_event_called_ == false);
        assert(on_recall_event_called_ == false);
    }

    void FsmOffEntry()
    {
        off_entry_called_ = true;
        std::cout << "Off entry\n";
    }
    void FsmOffEntry2()
    {
        off_entry2_called_ = true;
        std::cout << "Off entry2\n";
    }

    void FsmOffExit()
    {
        off_exit_called_ = true;
        fsm_.RecallEvents();
        std::cout << "Off exit\n";
    }
    void FsmOffExit2()
    {
        off_exit2_called_ = true;
        std::cout << "Off exit2\n";
    }

    void FsmOnEntry()
    {
        on_entry_called_ = true;
        std::cout << "On entry\n";
    }

    void FsmOnExit()
    {
        on_exit_called_ = true;
        std::cout << "On exit\n";
    }

    void FsmYellowRedTransitionAction1(Fsm::Event /*event*/)
    {
        yellow_red_transition1_called_ = true;
        std::cout << "Don't walk 1\n";
    }

    void FsmYellowRedTransitionAction2(Fsm::Event /*event*/)
    {
        yellow_red_transition2_called_ = true;
        std::cout << "Don't walk 2\n";
    }

    void Walk(Fsm::Event /*event*/)
    {
        std::cout << "Walk\n";
    }

public:
    void Main()
    {
        assert(Fsm::FindCommonParent(&Fsm::kGreen, &Fsm::kOn) == &Fsm::kOn);
        assert(Fsm::FindCommonParent(&Fsm::kOn, &Fsm::kGreen) == &Fsm::kOn);
        assert(Fsm::FindCommonParent(&Fsm::kOn, &Fsm::kOff) == nullptr);
        assert(Fsm::FindCommonParent(&Fsm::kOff, &Fsm::kOn) == nullptr);
        assert(Fsm::FindCommonParent(&Fsm::kGreen, &Fsm::kRed) == &Fsm::kOn);
        assert(Fsm::FindCommonParent(&Fsm::kRed, &Fsm::kGreen) == &Fsm::kOn);

        CheckAllFalse();
        fsm_.Start(&Fsm::kOff);
        assert(fsm_.CurrentState() == &Fsm::kOff);
        assert(off_entry_called_ == true);
        off_entry_called_ = false;
        assert(off_entry2_called_ == true);
        off_entry2_called_ = false;
        CheckAllFalse();

        fsm_.React(EvtSelfTransition::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kOff);
        assert(off_entry_called_ == true);
        off_entry_called_ = false;
        assert(off_entry2_called_ == true);
        off_entry2_called_ = false;
        assert(off_exit_called_ == true);
        off_exit_called_ = false;
        assert(off_exit2_called_ == true);
        off_exit2_called_ = false;
        assert(on_recall_event_called_ == true);
        on_recall_event_called_ = false;
        CheckAllFalse();

        fsm_.React(EvtTurnOn::MakeShared());
        assert(off_exit_called_ == true);
        off_exit_called_ = false;
        assert(off_exit2_called_ == true);
        off_exit2_called_ = false;
        assert(on_entry_called_ == true);
        on_entry_called_ = false;
        assert(on_recall_event_called_ == true);
        on_recall_event_called_ = false;
        assert(fsm_.CurrentState() == &Fsm::kGreen);
        CheckAllFalse();

        fsm_.React(EvtTurnOn::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kGreen);
        CheckAllFalse();

        fsm_.React(EvtGoYellow::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kYellow);
        CheckAllFalse();

        fsm_.React(EvtGoRed::MakeShared());
        assert(yellow_red_transition1_called_ == true);
        yellow_red_transition1_called_ = false;
        assert(yellow_red_transition2_called_ == true);
        yellow_red_transition2_called_ = false;
        assert(fsm_.CurrentState() == &Fsm::kRed);
        CheckAllFalse();

        fsm_.React(EvtGoYellow::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kRedYellow);
        CheckAllFalse();

        fsm_.React(EvtSelfTransition::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kRedYellow);
        CheckAllFalse();

        fsm_.React(EvtGoGreen::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kGreen);
        CheckAllFalse();

        fsm_.React(EvtGoRed::MakeShared());
        assert(fsm_.CurrentState() == &Fsm::kRed);
        CheckAllFalse();

        fsm_.React(EvtTurnOff::MakeShared());
        assert(on_exit_called_ == true);
        on_exit_called_ = false;
        assert(off_entry_called_ == true);
        off_entry_called_ = false;
        assert(off_entry2_called_ == true);
        off_entry2_called_ = false;
        assert(fsm_.CurrentState() == &Fsm::kOff);
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
        fsm_.Start(&Fsm::kOff);
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

const std::array<Fsm::State::EntryExitType, 2> Fsm::FsmOffEntryActions =
    std::to_array<Fsm::State::EntryExitType>({&Fsm::Impl::FsmOffEntry, &Fsm::Impl::FsmOffEntry2});
const std::array<Fsm::State::EntryExitType, 2> Fsm::FsmOffExitActions =
    std::to_array<Fsm::State::EntryExitType>({&Fsm::Impl::FsmOffExit, &Fsm::Impl::FsmOffExit2});
const Fsm::State Fsm::kOff("Off", &FsmOffHandler, nullptr, nullptr, FsmOffEntryActions, FsmOffExitActions);
const Fsm::HistoryState Fsm::kOn("On", &FsmOnHandler, nullptr, &kGreen, &Fsm::Impl::FsmOnEntry, &Fsm::Impl::FsmOnExit);
const Fsm::State Fsm::kGreen("Green", &FsmGreenHandler, &kOn);
const Fsm::State Fsm::kYellow("Yellow", &FsmYellowHandler, &kOn);
const Fsm::State Fsm::kRed("Red", &FsmRedHandler, &kOn);
const Fsm::State Fsm::kRedYellow("RedYellow", &FsmRedYellowHandler, &kOn);

Fsm::Transition Fsm::FsmOffHandler(ImplPtr /*impl*/, Event event)
{
    switch (event->Id())
    {
    case EvtTurnOn::kId:
        return TransitionTo(kOn);
    case EvtTurnOff::kId:
        return NoTransition();
    case EvtGoYellow::kId: // fall through
    case EvtGoRed::kId:
        return DeferEvent();
    case EvtSelfTransition::kId:
        // Self transition, entry + exit must be called
        return TransitionTo(kOff);
    default:
        return UnhandledEvent();
    }
}

Fsm::Transition Fsm::FsmOnHandler(ImplPtr /*impl*/, Event event)
{
    switch (event->Id())
    {
    case EvtTurnOff::kId:
        return TransitionTo(kOff);
    case EvtTurnOn::kId:
        return NoTransition();
    case EvtGoRed::kId:
        return TransitionTo(kRed);
    default:
        return UnhandledEvent();
    }
}

Fsm::Transition Fsm::FsmGreenHandler(ImplPtr /*impl*/, Event event)
{
    switch (event->Id())
    {
    case EvtGoYellow::kId:
        return TransitionTo(kYellow);
    case EvtGoGreen::kId:
        return NoTransition();
    case EvtSelfTransition::kId:
        return TransitionTo(kGreen);
    default:
        return UnhandledEvent();
    }
}

Fsm::Transition Fsm::FsmYellowHandler(ImplPtr /*impl*/, Event event)
{
    switch (event->Id())
    {
    case EvtGoRed::kId:
        static const auto kYellowRedTransitionActions = std::to_array<Fsm::ActionType>(
            {&Fsm::Impl::FsmYellowRedTransitionAction1, &Fsm::Impl::FsmYellowRedTransitionAction2});
        return TransitionTo(kRed, kYellowRedTransitionActions);
    case EvtGoYellow::kId:
        return NoTransition();
    case EvtSelfTransition::kId:
        return TransitionTo(kYellow);
    default:
        return UnhandledEvent();
    }
}

Fsm::Transition Fsm::FsmRedHandler(ImplPtr /*impl*/, Event event)
{
    switch (event->Id())
    {
    case EvtGoYellow::kId:
        return TransitionTo(kRedYellow);
    case EvtGoRed::kId:
        return NoTransition();
    case EvtSelfTransition::kId:
        return TransitionTo(kRed);
    default:
        return UnhandledEvent();
    }
}

Fsm::Transition Fsm::FsmRedYellowHandler(ImplPtr /*impl*/, Event event)
{
    switch (event->Id())
    {
    case EvtGoGreen::kId:
        return TransitionTo(kGreen, &Fsm::Impl::Walk);
    case EvtGoYellow::kId:
        return NoTransition();
    case EvtSelfTransition::kId:
        return TransitionTo(kRedYellow);
    default:
        return UnhandledEvent();
    }
}

void StatemachineFixtureMain()
{
    StatemachineImpl fsm;

    fsm.SetUp();
    fsm.Main();

    fsm.SetUp();
    fsm.History();
}

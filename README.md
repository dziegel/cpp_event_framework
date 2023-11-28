# cpp_event_framework

Header-only C++ event and statemachine framework

## Overview

### Events

- Shared pointers for easy handling
- Allocatable from heap or pools
- Simple to declare
- Events have IDs that can be used in switch/case
- Possibility to add data members to signal class
- Events have names for logging

### Statemachine

- Hierarchical state support. If a state does not handle an event, it is passed to parent state.
- Entry/Exit funtions
- Transition actions
- History support
- Unhandled event support
- Deferred event support (needs external framework)
- Independent of event type (can be int, enum, shared pointer...)
- Designed to call member functions of a C++ class or interface
- It is fairly simple to write statemachines "by hand" without a code generator
- Suitable for small systems: state and transition declarations can be const and in RO section
- Allow non-capturing lambdas as transition action
- Logging support (state entry/exit/handler events)
- States have names for logging

## Introduction to events

### Basic usage

A simple event class which is allocated from heap is declared via a class that inherits from cpp_event_framework::SignalBase template:

    class SimpleTestEvent : public cpp_event_framework::SignalBase<SimpleTestEvent, 0>
    {
    };

The integer "0" is the ID of the event and can be accessed via:

    SimpleTestEvent::kId                // static context
    SimpleTestEvent::MakeShared()->Id() // instance context

Event instances have names:

    std::cout << "This is " << anEventInstance->Name() << " with ID " << anEventInstance->Id() << std::endl;

To simplify creation of following events, a NextSignal template is available:

    class SimpleTestEvent2 : public cpp_event_framework::NextSignal<SimpleTestEvent2, SimpleTestEvent>
    {
    };

Using this template, events can be "chained" - now SimpleTestEvent2 automatically gets ID 1.

### Event attributes

Events may have attributes, too:

    class PayloadTestEvent : public cpp_event_framework::NextSignal<PayloadTestEvent, SimpleTestEvent2>
    {
    public:
        const std::vector<uint8_t> payload_;
    
        PayloadTestEvent(std::vector<uint8_t> payload) : payload_(std::move(payload))
        {
        }
    };

Events may inherit from base classes to simplify creation of similar events:

    class EventTestBaseClass : public cpp_event_framework::Signal
    {
    public:
        const int val_ = 0;
    
    protected:
        EventTestBaseClass(uint32_t id, int val) : Signal(id), val_(val)
        {
        }
    };
    
    class TestEventWithBaseClass
        : public cpp_event_framework::NextSignal<TestEventWithBaseClass, PayloadTestEvent, EventTestBaseClass>
    {
    public:
        TestEventWithBaseClass(int val) : Base(val)
        {
        }
    };

### Instantiating events

Events are instantiated via static MakeShared() function:

    auto s1 = SimpleTestEvent::MakeShared();
    auto s2 = TestEventWithBaseClass::MakeShared(3);

Instantiating via new() is not possible because operator new has been deleted:

    auto s3 = new SimpleTestEvent(); // compile error

### Casting events

To convert an event back from Signal base class to its actual type use the FromSignal class function. Note
there is an assertion in there that checks that the event ID matches the event class!

    cpp_event_framework::Signal::SPtr event = SimpleTestEvent2::MakeShared();
    assert(event->Id() == SimpleTestEvent2::kId);

    auto te_ok  = SimpleTestEvent2::FromSignal(event); // ok
    auto te_bad = SimpleTestEvent::FromSignal(event);  // exception thrown

### Usage in statemachines example

Example of event usage in a switch/case statement (e.g. for use in statemachines):

    static void DispatchEvent(const cpp_event_framework::Signal::SPtr& event)
    {
        std::cout << "Dispatching " << event->Name() << std::endl;
        switch (event->Id())
        {
        case SimpleTestEvent::kId:
            std::cout << "SimpleTestEvent" << std::endl;
            break;
        case SimpleTestEvent2::kId:
            std::cout << "SimpleTestEvent2" << std::endl;
            break;
        case PayloadTestEvent::kId:
        {
            auto te3 = PayloadTestEvent::FromSignal(event);
            assert(te3->payload_.at(1) == 2);
            break;
        }
        };
    }

    static void UsageInSwitchCase()
    {
        DispatchEvent(SimpleTestEvent::MakeShared());
        DispatchEvent(SimpleTestEvent2::MakeShared());
        DispatchEvent(PayloadTestEvent::MakeShared(std::vector<uint8_t>({1, 2, 3})));
    }

### Event pools

It is also possible to use event pools. The first step is to declare a pool allocator:

    class EventPoolAllocator : public cpp_event_framework::PoolAllocator<>
    {
    };

To create a pool, a pool must know the maximum event size of all events that will be created from it.
A helper template is available for this, its argument list must contain ALL signals:

    using PoolSizeCalculator =
        cpp_event_framework::SignalPoolElementSizeCalculator<PooledSimpleTestEvent, PooledSimpleTestEvent2>;

Using the size calculator, a pool can be instantiated and allocators can be assigned to it:

    auto pool = cpp_event_framework::Pool<>::MakeShared(PoolSizeCalculator::kSptrSize, 10, "MyPool");
    EventPoolAllocator::SetPool(pool);

Using a pool allocator, events can be now declared that are allocated via pools. Note the NextSignal template
manages the event ID AND inherits the allocator from the previous signal!
In the following example, PooledSimpleTestEvent and PooledSimpleTestEvent2 are allocated via EventPoolAllocator.

    class PooledSimpleTestEvent
        : public cpp_event_framework::SignalBase<PooledSimpleTestEvent, 3, cpp_event_framework::Signal, EventPoolAllocator>
    {
    };
    
    class PooledSimpleTestEvent2 : public cpp_event_framework::NextSignal<PooledSimpleTestEvent2, PooledSimpleTestEvent>
    {
    };

The actual pool fill level can be checked like this:

    assert(pool->FillLevel() == 10);
    {
        auto event = PooledSimpleTestEvent::MakeShared();
        assert(pool->FillLevel() == 9);

        auto event2 = PooledSimpleTestEvent2::MakeShared();
        assert(pool->FillLevel() == 8);
    }
    assert(pool->FillLevel() == 10);

## Introduction to statemachine framework

### Step-by-step walkthrough of a simple statemachine

1) Declare events:

        enum class EEvent : uint32_t
        {
            kGo1,
            kGo2
        };

2) Forward declare the class that will contain the statemachine:

        class StatemachineImplementation;

3) Declare statemachine class and its states and state handlers:

        class Fsm : public cpp_event_framework::Statemachine<StatemachineImplementation, EEvent>
        {
        public:
            static const Fsm::State kState1;
            static const Fsm::State kState2;
            static const Transition kState2ToState1Transition;

            static Transition State1Handler(ImplPtr /* impl */, Event event)
            {
                switch (event)
                {
                case EEvent::kGo2:
                    return TransitionTo(Fsm::kState2);
                default:
                    return NoTransition();
                }
            }
        
            static Transition State2Handler(ImplPtr /* impl */, Event event)
            {
                switch (event)
                {
                case EEvent::kGo1:
                    return kState2ToState1Transition;
                default:
                    return UnhandledEvent();
                }
            }
        };

4) Declare class that contains the statemachine:

        class StatemachineImplementation
        {
        private:
            // Allow private functions of class StatemachineImplementation to be used by FSM
            friend class Fsm;

            // Implementation can aggregate the statemachine if desired
            Fsm fsm_;

            void State2ToState1Action(Fsm::Event /*event*/)
            {
                [...]
            }
        };

5) Declare statemachine states by giving them a name and a pointer to a state handler function, declare transitions with actions:

        const Fsm::State Fsm::kState1("State1", &Fsm::State1Handler);
        const Fsm::State Fsm::kState2("State2", &Fsm::State2Handler);

        const Fsm::Transition Fsm::kState2ToState1Transition(Fsm::kState1, &Fsm::Impl::State2ToState1Action);

6) Initialize with implementation, name and initial state, then and start statemachine.
    Starting the statemachine is a separate function since it calls the entry handler of the initial state (if present).
    This may not be desired when the machine is initialized.

        class StatemachineImplementation
        {
        public:
            StatemachineImplementation()
            {
                fsm_.Init(this, "Fsm", &Fsm::kState1);
                fsm_.Start();
            }
        
        [...]
        };

7) Send events to statemachine

        void Run()
        {
            fsm_.React(EEvent::kGo2);
            fsm_.React(EEvent::kGo1);
        }

### Possible state handler return values

1) Transition to another state:

        return Fsm::TransitionTo(Fsm::kState1);

2) Transition to another state with transition action:

        // Using class StatemachineImplementation member function
        return Fsm::TransitionTo(Fsm::kState1, &StatemachineImplementation::SomeAction);

        // using non-capturing lambda
        return Fsm::TransitionTo(Fsm::kState1,
            [](Fsm::ImplPtr, Fsm::Event) { std::cout << "Transition action" << std::endl; });

3) No transition - event is handled, but no state transition occurs:

        return Fsm::NoTransition();

4) No state transition, but an action is executed:

        // Using class StatemachineImplementation member function
        return Fsm::NoTransition(&StatemachineImplementation::SomeAction);

        // using non-capturing lambda
        return Fsm::NoTransition([](Fsm::ImplPtr, Fsm::Event) { std::cout << "Transition action" << std::endl; });

5) Event is not handled in this state. In hierarchical statemachines, the event will be passed to parent state handler.
   When topmost state does not handle the event, fsm_.on_unhandled_event_ is called.

        return Fsm::UnhandledEvent();

6) Defer event (needs external framework support)

        return Fsm::DeferEvent();

### Predefining transitions (like states)

    class Fsm : public FsmBase
    {
    public:
        [...]
        static const Fsm::Transition Fsm::kState2ToState1;

        static const Fsm::Transition::ActionContainer<2> kState2ToState3Actions;
        static const Fsm::Transition Fsm::kState2ToState3;

        static const Fsm::Transition::ActionType kState2ToState4Actions[]; 
        static const Fsm::Transition Fsm::kState2ToState4;
    };

    // Single action
    const Fsm::Transition Fsm::kState2ToState1(kState1, &Fsm::Impl::FsmState2ToState1Action);

    // Multiple actions (clean, std::array<> based)
    const Fsm::Transition::ActionContainer<2> Fsm::kState2ToState3Actions = {&Fsm::Impl::FsmState2ToState3Action1, &Fsm::Impl::FsmState2ToState3Action2};
    const Fsm::Transition Fsm::kState2ToState3(kState3, kState2ToState3Actions);

    // Multiple actions (unclean, C-style array)
    const Fsm::Transition::ActionType Fsm::kState2ToState4Actions[] = {&Fsm::Impl::FsmState2ToState4Action1, &Fsm::Impl::FsmState2ToState4Action2};
    const Fsm::Transition Fsm::kState2ToState4(kState4, kState2ToState4Actions);

### Hierarchical states

To create a hierarchical statemachine, states may have parent states:

    const Fsm::State Fsm::kChildState("ChildState", &Fsm::Impl::ChildHandler, &Fsm::SomeParent);

Parent states may have initial states:

    const Fsm::State Fsm::kParentState("ParentState", &Fsm::Impl::ParentHandler, nullptr /* no parent */, &Fsm::ChildState);

### State entry/exit actions

    const Fsm::State Fsm::kSomeState("SomeState", &Fsm::Impl::SomeStateHandler, nullptr, nullptr,
        &Fsm::Impl::FsmSomeStateEntry, &Fsm::Impl::FsmSomeStateExit);

### History state

A parent state may be a history state:

    const Fsm::HistoryState Fsm::kSomeState("SomeState", &Fsm::Impl::SomeStateHandler, nullptr, nullptr, nullptr, nullptr);

### Deferred events

Events can be deferred by using "Fsm::DeferEvent()" transition. The statemachine provides an on_defer_event_ event for this.
External code is responsible to store events and to provide a possibility to recall deferred events.
Example:

    class Impl;
    class Fsm : public cpp_event_framework::Statemachine<Impl, EEvent>
    {
        [...]
        Fsm::Transition FsmStateActiveHandler(Fsm::StateRef, Fsm::Event event)
        {
            switch (event->Id())
            {
            case EvtDoSomething::kId:
                return Fsm::DeferEvent();
            default:
                return Fsm::UnhandledEvent();
            }
        }
    };

    class Impl
    {
        Fsm fsm_;
        std::vector<Fsm::Event> deferred_events;

        void Init()
        {
            fsm_.on_defer_event_ = [this](Fsm::StateRef, Fsm::Event event)
            {
                deferred_events.emplace_back(event);
            };

            fsm_.on_recall_deferred_events_ = [this](Fsm::StateRef)
            {
                for (auto& event : deferred_events)
                {
                    fsm_.React(event);
                }
            };
        }

        void FsmOnStateIdleEntry(Fsm::StateRef)
        {   
            fsm_.RecallEvents();
        }
    };

### Function signatures

- State handlers. The signature allows to use class member functions as state handlers.
    Also, the argument "state" allows to use the same handler function for multiple states.

        Fsm::Transition (Fsm::ImplPtr)(Fsm::StateRef state, Fsm::Event event)

- Entry/Exit actions. Same as for handlers - the signature allows to use class member functions
    as action. Also, the argument "state" allows to use the same action function for multiple states.

        void (Fsm::ImplPtr)(Fsm::StateRef state)

- Transition actions. The signature allows to use class member functions and non-capturing lambdas as actions.
    The argument "event" may be useful in actions because the action may depend on the event type or attributes
    of the event.

        void (*)(Fsm::ImplPtr impl, Fsm::Event event) // Non-capturing lambda
        void (Fsm::ImplPtr)(Fsm::Event event)         // C++

### Logging

        fsm_.on_state_entry_ = [this](Fsm::StateRef state)
            { std::cout << fsm_.Name() << " enter state " << state.Name() << std::endl; };

        fsm_.on_state_exit_ = [this](Fsm::StateRef state)
            { std::cout << fsm_.Name() << " exit state " << state.Name() << std::endl; };

        fsm_.on_handle_event_ = [this](Fsm::StateRef state, Fsm::Event event)
            { std::cout << fsm_.Name() << " state " << state.Name() << " handle event " << event->Name() << std::endl; };

        fsm_.on_unhandled_event_ = [this](Fsm::StateRef state, Fsm::Event event)
            { std::cout << fsm_.Name() << " unhandled event " << event->Name() << " in state " << state.Name() << std::endl; };

### Simple statemachine example

Uses integers as events.

<https://github.com/dziegel/cpp_event_framework/blob/main/examples/SimpleStatemachineExample.cxx>

### Complex statemachine example

Uses cpp_event_framework::Signal as events.

<https://github.com/dziegel/cpp_event_framework/blob/main/test/Statemachine_unittest.cxx>

## Statemachine generation

<https://github.com/dziegel/cpp_statemachine_generator>

## License

Apache-2.0

## Author

Dirk Ziegelmeier <dirk@ziegelmeier.net>

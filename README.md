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
- Hierarchical state support (if a state does not handle an event, it is passed to parent state)
- Entry/Exit funtions
- Transition actions
- History support
- Unhandled event support
- Possibility to use same handler/entry/exit function for multiple states (state is passed as argument to functions)
- Independent of event type (can be int, enum, pointer, shared pointer...)
- Designed to be aggregated by a class
- Designed to call member functions of class aggregating statemachine
- State and transition declaration is read-only (const, but due to std::function/std::vector usage not placed in RO section)
- Logging support (state entry/exit/handler events)
- States have names for logging

## Introduction to events

### Basic usage
A simple event class which is allocated from heap is declared via a class that inherits from cpp_event_framework::SignalBase template:

    class SimpleTestEvent : public cpp_event_framework::SignalBase<SimpleTestEvent, 0>
    {
    };

The integer "0" is the ID of the event and can be accessed via:

    SimpleTestEvent::kId
    aSimpleTestEventInstance->Id()

Event instances have names:

    std::cout << "This is " << anEventInstance->Name() << " with ID " anEventInstance->Id() << std::endl;

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

### Usage with legacy C code
Events can be passed to C code as void pointer - but be aware this breaks shared_ptr refcounting!
After converting an event to void* it MUST be converted back ONCE from void* to event!

    static void VoidPointer()
    {
        auto event = SimpleTestEvent::MakeShared();

        void* vp = SimpleTestEvent::ToVoidPointer(event);

        // pass vp to C-function

        ...

        // get vp back in some callback function

        auto anonymous = cpp_event_framework::Signal::FromVoidPointer(vp);
        if (anonymous->Id() == SimpleTestEvent::kId)
        {
            auto event2 = SimpleTestEvent::FromSignal(anonymous);
            // use event2...
        }
    }

### Event pools
It is also possible to use event pools. The first step is to declare a pool allocator:

    class EventPoolAllocator : public cpp_event_framework::PoolAllocator<>
    {
    };

Allocators need to be assigned to a pool:

    auto pool = cpp_event_framework::Pool<>::MakeShared(PoolSizeCalculator::kSptrSize, 10, "MyPool");
    EventPoolAllocator::SetPool(pool);

To create a pool, a pool must know the maximum event size of all events that will be created from it.
A helper template is available for this, its argument list must contain ALL signals:

    using PoolSizeCalculator =
        cpp_event_framework::SignalPoolElementSizeCalculator<PooledSimpleTestEvent, PooledSimpleTestEvent2>;

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

    auto event = PooledSimpleTestEvent::MakeShared();
    assert(pool->FillLevel() == 9);

    auto event2 = PooledSimpleTestEvent2::MakeShared();
    assert(pool->FillLevel() == 8);

## Introduction to statemachine framework

### Step-by-step walkthrough of a simple statemachine

1) Declare events:

        enum class EEvent : uint32_t
        {
            kGo1,
            kGo2
        };

2) Forward declare the class that will contain the statemachine:

        class ClassContainingAStatemachine;

3) Declare statemachine class and its states:

        class Fsm : public cpp_event_framework::Statemachine<ClassContainingAStatemachine, EEvent>
        {
        public:
            static const Fsm::State kState1;
            static const Fsm::State kState2;
            static const StatePtr kInitialState;
        };

4) Declare class that contains the statemachine:

        class ClassContainingAStatemachine
        {
        private:
            friend class Fsm;
            Fsm fsm_;
        };

5) Declare statemachine states:

        const Fsm::State Fsm::kState1("State1", std::mem_fn(&Fsm::Owner::State1Handler));
        const Fsm::State Fsm::kState2("State2", std::mem_fn(&Fsm::Owner::State2Handler));
        const Fsm::StatePtr Fsm::kInitialState = &Fsm::kState1; // initial state of the statemachine

6) Initialize with owner, name and initial state, then and start statemachine:

        class ClassContainingAStatemachine
        {
        public:
            ClassContainingAStatemachine()
            {
                fsm_.Init(this, "Fsm", Fsm::kInitialState);
                fsm_.Start();
            }
        
        [...]
        };

7) Implement statemachine handlers in class that contains the statemachine:

        class ClassContainingAStatemachine
        {
        [...]
        private:
        [...]
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
                    return Fsm::TransitionTo(Fsm::kState1);
                default:
                    return Fsm::UnhandledEvent();
                }
            }
        };

8) Send events to statemachine

        void Run()
        {
            fsm_.React(EEvent::kGo2);
            fsm_.React(EEvent::kGo1);
        }

### Possible state handler return values

1) Transition to another state: 

        return Fsm::TransitionTo(Fsm::kState1);

2) Transition to another state with transition action: 

        return Fsm::TransitionTo(Fsm::kState1,
            [](Fsm::Owner*, Fsm::Event) { std::cout << "Transition action" << std::endl; });

3) No transition - event is handled, but no state transition occurs:

        return Fsm::NoTransition();

4) No state transition, but an action is executed:

        return Fsm::NoTransition([](Fsm::Owner*, Fsm::Event) { std::cout << "Transition action" << std::endl; });

5) Event is not handled in this state. In hierarchical statemachines, the event will be passed to parent state handler.
   When topmost state does not handle the event, fsm_.on_unhandled_event_ is called.

        return Fsm::UnhandledEvent();

### Predefining transitions (like states)

    class Fsm : public FsmBase
    {
    public:
        [...]
        static const Fsm::Transition Fsm::kState2ToState1;
    };

    const Fsm::Transition Fsm::kState2ToState1(kState1, std::mem_fn(&Fsm::Owner::FsmState2ToState1Action));

### Hierarchical states

To create a hierarchical statemachine, states may have parent states:

    const Fsm::State Fsm::kChildState("ChildState", std::mem_fn(&Fsm::Owner::ChildHandler), &Fsm::SomeParent);

Parent states may have initial states:

    const Fsm::State Fsm::kParentState("ParentState", std::mem_fn(&Fsm::Owner::ParentHandler), nullptr /* no parent */, &Fsm::ChildState);

### State entry/exit actions

    const Fsm::State Fsm::kSomeState("SomeState", std::mem_fn(&Fsm::Owner::SomeStateHandler), nullptr, nullptr,
        std::mem_fn(&Fsm::Owner::FsmSomeStateEntry), std::mem_fn(&Fsm::Owner::FsmSomeStateExit));

### History state

A parent state may be a history state

    const Fsm::State Fsm::kSomeState("SomeState", std::mem_fn(&Fsm::Owner::SomeStateHandler), nullptr, nullptr,
        nullptr, nullptr, Fsm::EFlags::kHistory);

### Function signatures

- State handlers. The signature allows to use class member functions via std::mem_fn() as state handlers.
    Also, the argument "state" allows to use the same handler function for multiple states.

        Fsm::Transition StateHandler(Fsm::Owner* owner, Fsm::StatePtr state, Fsm::Event event)

- Entry/Exit actions. Same as for handlers - the signature allows to use class member functions via
    std::mem_fn() as action. Also, the argument "state" allows to use the same action function for multiple states.

        void EntryAction(Fsm::Owner* owner, Fsm::StatePtr state)

- Transition actions. The signature allows to use class member functions via std::mem_fn() as state handlers.
    The argument "event" may be useful in actions because the action may depend on the event type or attributes
    of the event.

        void TransitionAction(Fsm::Owner* owner, Fsm::Event event)


### Simple statemachine example

https://github.com/dziegel/cpp_event_framework/blob/main/examples/SimpleStatemachineExample.cxx

### Complex statemachine example

https://github.com/dziegel/cpp_event_framework/blob/main/test/Statemachine_unittest.cxx

## Future ideas

- Statemachine generator e.g. from scxml or xmi

## License

Apache-2.0

## Author

Dirk Ziegelmeier <dirk@ziegelmeier.net>

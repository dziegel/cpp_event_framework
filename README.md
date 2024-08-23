# C++ event framework

Header-only C++ event, statemachine and active object framework

## Overview

### [Events](#introduction-to-events)

- Shared pointers for easy handling
- Allocatable from heap or pools
- Simple to declare
- Events have IDs that can be used in switch/case
- Possibility to add data members to signal class
- Events have names for logging (and an ostream operator<<)

### [Statemachine](#introduction-to-statemachine-framework)

- Hierarchical state support. If a state does not handle an event, it is passed to parent state.
- Entry/Exit funtions
- Transition actions
- History support
- Unhandled event support
- Deferred event support (needs external framework)
- Independent of event type (can be int, enum, shared pointer...)
- Designed to call member functions of a C++ interface
- It is fairly simple to write statemachines "by hand" without a code generator
- Suitable for small systems: state declarations can be const and in RO section
- Allow non-capturing lambdas as transition action
- Logging support (state entry/exit/handler events)
- States have names for logging (and an ostream operator<<)
- Statemachines have names for logging (and an ostream operator<<)
- A statemachine generator is available: <https://github.com/dziegel/cpp_statemachine_generator>

### [Active Object Framework](#introduction-to-active-object-framework)

- Implements active object framework pattern
- Embedded version available that works without heap usage
- Domain implementation with single worker thread

## Introduction to events

### Basic usage

A simple event class which is allocated from heap is declared via a class that inherits from cpp_event_framework::SignalBase template:

    class SimpleTestEvent : public cpp_event_framework::SignalBase<SimpleTestEvent, 0>
    {
    };

The integer template parameter "0" is the ID of the event and can be accessed via:

    SimpleTestEvent::kId                // static context
    SimpleTestEvent::MakeShared()->Id() // instance context

Event instances have names:

    std::cout << "This is " << anEventInstance << " with ID " << anEventInstance->Id() << std::endl;

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
        std::cout << "Dispatching " << event << std::endl;
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

### Event pools (custom allocators)

It is also possible to use event pools. The first step is to declare a custom allocator:

    class EventPoolAllocator : public cpp_event_framework::CustomAllocator<EventPoolAllocator>
    {
    };

To create a pool, a pool must know the maximum event size of all events that will be created from it.
A helper template is available for this, its argument list must contain ALL signals:

    using PoolSizeCalculator =
        cpp_event_framework::SignalPoolElementSizeCalculator<PooledSimpleTestEvent, PooledSimpleTestEvent2>;

Using the size calculator, a pool can be instantiated and assigned to the custom allocator:

    auto pool = cpp_event_framework::Pool<>::MakeShared(PoolSizeCalculator::kSptrSize, 10, "MyPool");
    EventPoolAllocator::SetAllocator(pool);

or when using statically allocated pools (embedded systems):

    cpp_event_framework::StaticPool<10, PoolSizeCalculator::kSptrSize> pool("MyPool");
    EventPoolAllocator::SetAllocator(&pool);

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

Note the CustomAllocator<> takes a std::pmr::memory_resource as allocator because the whole allocation scheme is based on std::pmr::polymorphic_allocator!
You can supply your own allocators that implement the std::pmr::memory_resource interface here.
The predefined HeapAllocator is simply an allocator based on std::pmr::new_delete_resource.

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
                    return TransitionTo(kState1, &Fsm::Impl::State2ToState1Action);
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

6) Initialize with implementation, name and initial state, then and start statemachine.
    Starting the statemachine is a separate function since it calls the entry handler of the initial state (if present).
    This may not be desired when the machine is initialized.

        class StatemachineImplementation
        {
        public:
            StatemachineImplementation()
            {
                fsm_.Init(this, "Fsm");
                fsm_.Start(&Fsm::kState1);
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

- State handlers. The ImplPtr allows to call implementation member functions.

        Fsm::Transition (*)(Fsm::ImplPtr impl, Fsm::Event event)

- Entry/Exit actions. Actions are member functions of a class.

        void (Fsm::ImplPtr)()

- Transition actions. The signature allows to use class member functions and non-capturing lambdas as actions. The argument "event" may be useful in actions because the action may depend on the event type or attributes of the event.

        void (*)(Fsm::ImplPtr impl, Fsm::Event event) // Non-capturing lambda
        void (Fsm::ImplPtr)(Fsm::Event event)         // C++

### Logging

        fsm_.on_state_change_ = [](Fsm::Ref fsm, Fsm::Event event, Fsm::StateRef old_state, Fsm::StateRef new_state)
            { std::cout << fsm << ": " << old_state << " --- " << event << " ---> " << new_state << std::endl; };

        fsm_.on_state_entry_ = [](Fsm::Ref fsm, Fsm::StateRef state)
            { std::cout << fsm << " enter state " << state << std::endl; };

        fsm_.on_state_exit_ = [](Fsm::Ref fsm, Fsm::StateRef state)
            { std::cout << fsm << " exit state " << state << std::endl; };

        fsm_.on_handle_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
            { std::cout << fsm << " state " << state << " handle event " << event << std::endl; };

        fsm_.on_unhandled_event_ = [](Fsm::Ref fsm, Fsm::StateRef state, Fsm::Event event)
            { std::cout << fsm << " unhandled event " << event << " in state " << state << std::endl; };

### Implementation variants

There are multiple possible implementation variants:

1) Complete separation of statemachine and implementation code using an interface. This is the cleanest solution, it allows unit testing of statemachine code, at the cost of using virtual calls for actions. See <https://github.com/dziegel/cpp_event_framework/tree/main/examples/interface>

2) Statemachine and implementation are tighly coupled, implementation uses PIMPL pattern to aggregate statemachine. Less clean, but does not need virtual calls for actions which might be interesting for embedded systems. See <https://github.com/dziegel/cpp_event_framework/tree/main/examples/pimpl>

3) Statemachine and implementation are even more tighly coupled, implementation and statemachine code intermix. Least clean solution, also does not need virtual calls for actions. Use this only if you want to avoid interfaces AND PIMPL pattern, again this might be interesting for embedded systems. Together with a static pool as event pool and the embedded version of the active object framework, no heap is used at all except for thread creation (rewrite SingleThreadActiveObjectDomain to avoid this, too). See <https://github.com/dziegel/cpp_event_framework/tree/main/examples/plain>

### Simple statemachine example

Uses integers as events.

<https://github.com/dziegel/cpp_event_framework/tree/main/examples/interface>

### Complex statemachine example

Uses cpp_event_framework::Signal as events.

<https://github.com/dziegel/cpp_event_framework/blob/main/test/Statemachine_unittest.cxx>

## Introduction to Active Object Framework

A framework that implements the active object pattern is also available. It comes in two flavors:

- Normal applications: Uses heap and std::shared_ptr<> for everything. Namespace: cpp_active_objects.
- Embedded applications: No heap usage and std::shared_ptr<> only for signals. Namespace: cpp_active_objects_embedded.

The framework consists of the following elements:

### Interfaces to decouple components

- IEventTarget: Base class for an object that can receive events. Hides that e.g. an event is queued and dispatched in another thread.
- IActiveObject: Adds functions to assign a queue to enqueue events, and a function to dispatch queued events.
- IActiveObjectDomain: Interface to register active objects in a domain.
- IEventQueue: Interface of a queue to decouple IActiveObject from an actual queue implementation.

### Base classes for Active Objects

- ActiveObjectBase: Contains queue pointer and implements queuing of events.
- Hsm: Base class to aggregate a statemachine. Implements event deferral.

### Base class for an Active Object Domain

- ActiveObjectDomainBase: Contains a queue pointer and implements thread function to dequeue and dispatch events from queue.

### Single-threaded Active Object Domain

- SingleThreadActiveObjectDomain: Contains a single worker thread that runs the Run() function of ActiveObjectDomainBase.

### Usage example

<https://github.com/dziegel/cpp_event_framework/tree/main/examples/activeobject> together with <https://github.com/dziegel/cpp_event_framework/blob/main/test/ActiveObjectFramework_unittest.cxx>

## License

Apache-2.0

## Author

Dirk Ziegelmeier <dirk@ziegelmeier.net>

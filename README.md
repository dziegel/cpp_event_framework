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
- State and transition declaration is read-only (const)
- Logging support (state entry/exit/handler events)
- States have names for logging

## Introduction to events
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

To convert an event back from Signal base class to its actual type use the following function:

    auto te3 = SimpleTestEvent3::FromSignal(event);

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

## Introduction to Statemachine

## Future ideas

- Statemachine generator e.g. from scxml or xmi

## License

Apache-2.0

## Author

Dirk Ziegelmeier <dirk@ziegelmeier.net>

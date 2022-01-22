#include <chrono>
#include <iostream>
#include <ostream>
#include <ratio>
#include <thread>

#include <cpp_event_framework/Pool.hxx>
#include <cpp_event_framework/Signal.hxx>
#include <cpp_event_framework/Statemachine.hxx>
#include <vector>

using namespace std::chrono_literals;

class EventTestBaseClass : public cpp_event_framework::Signal
{
public:
    const int val_ = 0;

protected:
    EventTestBaseClass(uint32_t id, int val) : Signal(id), val_(val)
    {
    }
};

class SimpleTestEvent : public cpp_event_framework::SignalBase<SimpleTestEvent, 0>
{
};

class SimpleTestEvent2 : public cpp_event_framework::NextSignal<SimpleTestEvent2, SimpleTestEvent>
{
};

class PayloadTestEvent : public cpp_event_framework::NextSignal<PayloadTestEvent, SimpleTestEvent2>
{
public:
    const std::vector<uint8_t> payload_;

    PayloadTestEvent(std::vector<uint8_t> payload) : payload_(std::move(payload))
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

class EventPoolAllocator : public cpp_event_framework::PoolAllocator<>
{
};

class PooledSimpleTestEvent
    : public cpp_event_framework::SignalBase<PooledSimpleTestEvent, 3, cpp_event_framework::Signal, EventPoolAllocator>
{
};

class PooledSimpleTestEvent2 : public cpp_event_framework::NextSignal<PooledSimpleTestEvent2, PooledSimpleTestEvent>
{
};

using PoolSizeCalculator =
    cpp_event_framework::SignalPoolElementSizeCalculator<PooledSimpleTestEvent, PooledSimpleTestEvent2>;

struct EventsFixture
{
public:
    static void BasicTest()
    {
        auto event = SimpleTestEvent::MakeShared();
        assert(event->Id() == 0);
        assert(event->Id() == SimpleTestEvent::kId);
        auto event2 = SimpleTestEvent2::MakeShared();
        assert(event2->Id() == 1);
        assert(event2->Id() == SimpleTestEvent2::kId);
    }

    static void SignalBaseClass()
    {
        auto e = TestEventWithBaseClass::MakeShared(4);
        assert(e->Id() == 3);
        assert(e->val_ == 4);
    }

    static void PooledSignalsTestHelper(const cpp_event_framework::Pool<>::SPtr& pool)
    {
        auto event = PooledSimpleTestEvent::MakeShared();
        assert(event->Id() == 3);
        assert(event->Id() == PooledSimpleTestEvent::kId);
        assert(pool->FillLevel() == 9);

        auto event2 = PooledSimpleTestEvent2::MakeShared();
        assert(event2->Id() == 4);
        assert(event2->Id() == PooledSimpleTestEvent2::kId);
        assert(pool->FillLevel() == 8);

        // Reference events again to avoid them being freed
        assert(event.use_count() == 1);
        assert(event2.use_count() == 1);
    }

    static void PooledSignals()
    {
        auto pool = cpp_event_framework::Pool<>::MakeShared(PoolSizeCalculator::kSptrSize, 10, "MyPool");
        EventPoolAllocator::SetPool(pool);

        assert(pool->FillLevel() == 10);
        PooledSignalsTestHelper(pool);
        assert(pool->FillLevel() == 10);
    }

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
};

void EventsFixtureMain()
{
    EventsFixture::BasicTest();
    EventsFixture::SignalBaseClass();
    EventsFixture::PooledSignals();
    EventsFixture::UsageInSwitchCase();
}

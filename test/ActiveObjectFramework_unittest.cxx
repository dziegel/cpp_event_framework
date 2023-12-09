/**
 * @file ActiveObjectFrameworkMain.cxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#include <memory>

#include "../examples/activeobject/FsmImpl.hxx"

#include <cpp_active_objects/SingleThreadActiveObjectDomain.hxx>
#include <cpp_active_objects/ThreadSafeEventQueue.hxx>

using namespace std::chrono_literals;

void ActiveObjectFrameworkMain()
{
    // Create a pool with elemen size calculated via EventPoolElementSizeCalculator, 10 elements
    auto pool = cpp_event_framework::Pool<>::MakeShared(
        example::activeobject::EventPoolElementSizeCalculator::kSptrSize, 10, "EventPool");

    // Tell EventPoolAllocator to use pool created above
    example::activeobject::EventPoolAllocator::SetPool(pool);

    auto queue = std::make_shared<cpp_active_objects::ThreadSafeEventQueue<>>();
    auto domain = std::make_shared<cpp_active_objects::SingleThreadActiveObjectDomain<>>(queue);

    auto active_object = std::make_shared<example::activeobject::FsmImpl>();
    domain->RegisterObject(active_object);

    assert(active_object->CurrentState() == &example::activeobject::Fsm::kState1);

    active_object->Take(example::activeobject::Go2::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object->CurrentState() == &example::activeobject::Fsm::kState2);

    active_object->Take(example::activeobject::Go1::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object->CurrentState() == &example::activeobject::Fsm::kState1);
}

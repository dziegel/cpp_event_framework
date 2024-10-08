/**
 * @file ActiveObjectFrameworkMain.cxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#include <iostream>
#include <memory>

#include "../examples/activeobject/FsmImpl.hxx"

#include <cpp_active_objects/SingleThreadActiveObjectDomain.hxx>
#include <cpp_event_framework/Pool.hxx>

using namespace std::chrono_literals;

void ActiveObjectFrameworkMain()
{
    auto pool = std::make_shared<cpp_event_framework::Pool<>>(
        example::activeobject::EventPoolElementSizeCalculator::kSptrSize, 10, "EventPool");

    // Tell EventPoolAllocator to use pool created above
    example::activeobject::EventPoolAllocator::SetAllocator(pool);

    auto domain = std::make_shared<cpp_active_objects::SingleThreadActiveObjectDomain<>>();

    auto active_object = std::make_shared<example::activeobject::FsmImpl>();
    domain->RegisterObject(active_object);

    assert(active_object->CurrentState() == &example::activeobject::Fsm::kState1);

    std::cout << pool << std::endl;

    active_object->Take(example::activeobject::Go2::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object->CurrentState() == &example::activeobject::Fsm::kState2);

    active_object->Take(example::activeobject::Go1::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object->CurrentState() == &example::activeobject::Fsm::kState1);
}

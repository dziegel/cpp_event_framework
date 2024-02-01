/**
 * @file ActiveObjectFrameworkEmbeddedMain.cxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#include "../examples/activeobject_embedded/FsmImpl.hxx"

#include <cpp_active_objects_embedded/EventQueue.hxx>
#include <cpp_active_objects_embedded/SingleThreadActiveObjectDomain.hxx>
#include <cpp_event_framework/StaticPool.hxx>

using namespace std::chrono_literals;

void ActiveObjectFrameworkEmbeddedMain()
{
    cpp_event_framework::StaticPool<2, example::activeobject_embedded::EventPoolElementSizeCalculator::kSptrSize> pool(
        "EmbeddedEventPool");

    // Tell EventPoolAllocator to use pool created above
    example::activeobject_embedded::EventPoolAllocator::SetAllocator(&pool);

    cpp_active_objects_embedded::EventQueue<10> queue;
    cpp_active_objects_embedded::SingleThreadActiveObjectDomain domain(&queue);

    example::activeobject_embedded::FsmImpl active_object;
    domain.RegisterObject(&active_object);

    assert(active_object.CurrentState() == &example::activeobject_embedded::Fsm::kState1);

    active_object.Take(example::activeobject_embedded::Go2::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object.CurrentState() == &example::activeobject_embedded::Fsm::kState2);

    active_object.Take(example::activeobject_embedded::Go1::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object.CurrentState() == &example::activeobject_embedded::Fsm::kState1);
}

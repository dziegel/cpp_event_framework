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

extern "C"
{
    extern void* __libc_malloc(size_t size);
}
static bool malloc_called = false;

// Uncomment this to check heap usage, but ASAN must be disabled in CMakeLists.txt for this to work
// void* malloc(size_t size)
// {
//     malloc_called = true;
//     return __libc_malloc(size);
// }

void ActiveObjectFrameworkEmbeddedMain()
{
    malloc_called = false;
    cpp_active_objects_embedded::EventQueue<10> queue;
    assert(!malloc_called);

    // SingleThreadActiveObjectDomain uses jthread - thread creation uses heap.
    // You need to write your own heapless SingleThreadActiveObjectDomain here if desired.
    cpp_active_objects_embedded::SingleThreadActiveObjectDomain domain(&queue);

    malloc_called = false;

    cpp_event_framework::StaticPool<2, example::activeobject_embedded::EventPoolElementSizeCalculator::kSptrSize> pool(
        "EmbeddedEventPool");
    // Tell EventPoolAllocator to use pool created above
    example::activeobject_embedded::EventPoolAllocator::SetAllocator(&pool);

    example::activeobject_embedded::FsmImpl active_object;
    domain.RegisterObject(&active_object);

    assert(active_object.CurrentState() == &example::activeobject_embedded::Fsm::kState1);

    active_object.Take(example::activeobject_embedded::Go2::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object.CurrentState() == &example::activeobject_embedded::Fsm::kState2);

    active_object.Take(example::activeobject_embedded::Go1::MakeShared());
    std::this_thread::sleep_for(500ms);
    assert(active_object.CurrentState() == &example::activeobject_embedded::Fsm::kState1);

    assert(!malloc_called);
}

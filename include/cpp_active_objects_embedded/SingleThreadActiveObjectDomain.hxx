/**
 * @file SingleThreadActiveObjectDomain.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <thread>

#include <cpp_active_objects_embedded/ActiveObjectDomainBase.hxx>
#include <cpp_active_objects_embedded/IEventQueue.hxx>

namespace cpp_active_objects_embedded
{
/**
 * @brief A thread-safe event queue
 *
 * @tparam ThreadType Thread type to use - e.g. to be able to use own RT-capable implementation
 */
template <typename ThreadType = std::jthread>
class SingleThreadActiveObjectDomain : public ActiveObjectDomainBase
{
public:
    /**
     * @brief Constructor
     *
     * @param queue
     */
    explicit SingleThreadActiveObjectDomain(IEventQueue* queue)
        : ActiveObjectDomainBase(queue), thread_(RunWrapper, this)
    {
    }

    // Non-copyable, non-movable
    SingleThreadActiveObjectDomain(const SingleThreadActiveObjectDomain& rhs) = delete;
    SingleThreadActiveObjectDomain(SingleThreadActiveObjectDomain&& rhs) = delete;
    SingleThreadActiveObjectDomain& operator=(const SingleThreadActiveObjectDomain& rhs) = delete;
    SingleThreadActiveObjectDomain& operator=(SingleThreadActiveObjectDomain&& rhs) = delete;

    ~SingleThreadActiveObjectDomain() override
    {
        Stop();
        thread_.join();
    }

    /**
     * @brief Get thread object
     *
     * @return ThreadType&
     */
    ThreadType& Thread()
    {
        return thread_;
    }

private:
    static void RunWrapper(void* arg)
    {
        auto me = static_cast<SingleThreadActiveObjectDomain*>(arg);
        me->Run();
    }

    ThreadType thread_;
};
} // namespace cpp_active_objects_embedded

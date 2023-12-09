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

#include <memory>
#include <semaphore>
#include <thread>

#include <cpp_active_objects/ActiveObjectDomainBase.hxx>
#include <cpp_active_objects/IEventQueue.hxx>

namespace cpp_active_objects
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
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<SingleThreadActiveObjectDomain>;

    /**
     * @brief Constructor
     *
     * @param queue
     */
    explicit SingleThreadActiveObjectDomain(IEventQueue::SPtr queue)
        : ActiveObjectDomainBase(std::move(queue)), thread_([this]() { Run(); })
    {
    }

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
    ThreadType thread_;
};
} // namespace cpp_active_objects

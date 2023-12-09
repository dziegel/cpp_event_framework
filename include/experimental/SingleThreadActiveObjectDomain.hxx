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

#include <experimental/ActiveObjectDomainBase.hxx>
#include <experimental/ThreadSafeEventQueue.hxx>

namespace cpp_event_framework
{
/**
 * @brief A thread-safe event queue
 *
 * @tparam SemaphoreType Sempahore type to use - e.g. to be able to use own RT-capable implementation
 * @tparam MutexType Mutex type to use - e.g. to be able to use own RT-capable implementation
 * @tparam ThreadType Thread type to use - e.g. to be able to use own RT-capable implementation
 */
template <typename SemaphoreType = std::binary_semaphore, typename MutexType = std::mutex,
          typename ThreadType = std::jthread>
class SingleThreadActiveObjectDomain : public ActiveObjectDomainBase
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<SingleThreadActiveObjectDomain>;

    SingleThreadActiveObjectDomain()
        : ActiveObjectDomainBase(std::make_shared<ThreadSafeEventQueue<SemaphoreType, MutexType>>())
        , thread_([this]() { Run(); })
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
} // namespace cpp_event_framework

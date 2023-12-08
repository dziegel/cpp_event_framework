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
#include <thread>
#include <semaphore>

#include <experimental/ActiveObjectDomainBase.hxx>
#include <experimental/ThreadSafeEventQueue.hxx>

namespace cpp_event_framework
{
template <typename SemaphoreType = std::counting_semaphore, typename MutexType = std::mutex>
class SingleThreadActiveObjectDomain : public ActiveObjectDomainBase
{
public:
    using SPtr = std::shared_ptr<SingleThreadActiveObjectDomain>;

    SingleThreadActiveObjectDomain()
        : ActiveObjectDomainBase(std::make_shared<ThreadSafeEventQueue<SemaphoreType, MutexType>>()), thread_([this]() { Run(); })
    {
    }

    ~SingleThreadActiveObjectDomain() override
    {
        queue_->PushBack(nullptr, nullptr);
        thread_.join();
    }

    std::jthread& Thread() const
    {
        return thread_;
    }

private:
    std::jthread thread_;
};
} // namespace cpp_event_framework

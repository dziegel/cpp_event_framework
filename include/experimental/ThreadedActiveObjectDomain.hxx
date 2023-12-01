/**
 * @file ThreadedActiveObjectDomain.hxx
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

#include <experimental/ActiveObjectDomainBase.hxx>
#include <experimental/ThreadSafeEventQueue.hxx>

namespace cpp_event_framework
{
class ThreadedActiveObjectDomain : public ActiveObjectDomainBase
{
public:
    using SPtr = std::shared_ptr<ThreadedActiveObjectDomain>;

    ThreadedActiveObjectDomain()
        : ActiveObjectDomainBase(std::make_shared<ThreadSafeEventQueue<>>()), thread_([this]() { Run(); })
    {
    }

    ~ThreadedActiveObjectDomain() override
    {
        queue_->PushBack(nullptr, nullptr);
        thread_.join();
    }

    std::jthread& Thread()
    {
        return thread_;
    }

private:
    std::jthread thread_;
};
} // namespace cpp_event_framework

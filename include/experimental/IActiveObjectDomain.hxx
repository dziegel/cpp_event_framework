/**
 * @file IActiveObjectDomain.hxx
 * @author Dirk Ziegelmeier (dirk@ziegelmeier.net)
 * @brief
 * @date 16-08-2023
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 */

#pragma once

#include <experimental/IActiveObject.hxx>

namespace cpp_event_framework
{
/**
 * @brief Interface of an Active Object Domain
 *
 */
class IActiveObjectDomain
{
public:
    /**
     * @brief Shared pointer alias
     *
     */
    using SPtr = std::shared_ptr<IActiveObjectDomain>;

    virtual ~IActiveObjectDomain() = default;

    /**
     * @brief Assign an active object to this domain
     *
     * @param active_object
     */
    virtual void RegisterObject(const IActiveObject::SPtr& active_object) = 0;
};
} // namespace cpp_event_framework

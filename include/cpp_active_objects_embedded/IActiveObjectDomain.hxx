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

#include <cpp_active_objects_embedded/IActiveObject.hxx>

namespace cpp_active_objects_embedded
{
/**
 * @brief Interface of an Active Object Domain
 *
 */
class IActiveObjectDomain
{
public:
    virtual ~IActiveObjectDomain() = default;

    /**
     * @brief Assign an active object to this domain
     *
     * @param active_object
     */
    virtual void RegisterObject(IActiveObject* active_object) = 0;
};
} // namespace cpp_active_objects_embedded

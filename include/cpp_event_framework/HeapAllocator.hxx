/**
 * @file HeapAllocator.hxx
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-24
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <memory_resource>

namespace cpp_event_framework
{
/**
 * @brief Use this allocator to allocate from heap
 */
class HeapAllocator
{
public:
    /**
     * @brief Default heap-based allocator
     */
    static std::pmr::memory_resource* GetAllocator()
    {
        return std::pmr::new_delete_resource();
    }
};
} // namespace cpp_event_framework

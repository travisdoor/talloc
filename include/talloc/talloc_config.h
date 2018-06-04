//*****************************************************************************
// talloc
//
// File:   config.h
// Author: Martin Dorazil
// Date:   19/05/2017
//
// Copyright 2017 Martin Dorazil
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//*****************************************************************************

#ifndef CONFIG_H_IF6CXWGS
#define CONFIG_H_IF6CXWGS

#ifdef _MSC_VER
#else
#include <stdalign.h>
#endif

/**
 * @def Default preallocated block size. When allocator is out of space, new
 * memory block of this size will be allocated. This value is also smallest
 * memory space allocated on first call of tmalloc(). Try to choose best
 * value for your use.
 */
#define TALLOC_BLOCK_SIZE 4194304 // 4 MB

/**
 * @def Every allocation with pool allocator is rounded up to next multiply of
 * this value. Objects of same size are in same pool.
 */
#define TALLOC_POOL_GROUP_MULT 32

/**
 * @def Count of cells allocated on heap in every pool. When count of
 * per-pool allocations reach this value, new pool block of this size will be
 * allocated on heap.
 */
#define TALLOC_INIT_POOL_SIZE 128

/**
 * @def Objects with size under this value will be allocated in pools, larger
 * objects will be allocated directly on heap.
 */
#define TALLOC_SMALL_TO 2048 // 2KB

/**
 * @def Enable or disable pooling of small objects.
 */
#define TALLOC_USE_POOLS 1

/**
 * @def Every allocation of talloc is aligned using this alignment
 */
#ifdef _MSC_VER
#define TALLOC_ALIGNMENT 16
#else
#define TALLOC_ALIGNMENT alignof(max_align_t)
#endif

/**
 * @def Enable or disable force freeing of system memory and reset of allocator.
 */
#define TALLOC_FORCE_RESET 0

/**
 * @brief Enable checking of freeing unallocated memory.
 */
#define TALLOC_MEM_CHECKING 1

/**
 * @brief Enable error notification system.
 *
 * Mathod talloc_exception(const char *) must be implemented on client side to handle
 * allocator errors. Abort is called after talloc_exception call.
 */
#define TALLOC_EXCEPTION_HANDLING 1

#endif /* end of include guard: CONFIG_HPP_IF6CXWGS */

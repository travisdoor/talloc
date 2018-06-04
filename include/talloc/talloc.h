//*****************************************************************************
// talloc
//
// File:   talloc.h
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

#ifndef TALLOC_H_QYTR1XNS
#define TALLOC_H_QYTR1XNS

#include <stdio.h>
#include "talloc_config.h"

#define TALLOC_VERSION_MAJOR 1
#define TALLOC_VERSION_MINOR 4
#define TALLOC_VERSION_PATCH 0

#ifdef _MSC_VER
#define TALLOC_EXPORT __declspec(dllexport)
#else
#define TALLOC_EXPORT __attribute__((__visibility__("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*talloc_err_f)(const char *);

/**
 * @brief Memory allocation.
 * Allocates memory of requested size. Automatic pooling is allowed for objects
 * with size up to TALLOC_SMALL_TO (check the config.h).
 * Large objects are allocated directly on heap.
 *
 * @param count Byte count.
 * @return Pointer to allocated memory block.
 */
extern TALLOC_EXPORT void *
tmalloc(size_t count);

/**
 * @brief Reuse or reallocate memory with different size
 * Reuse memory block when requested size fits already allocated block or
 * allocates new block of size. Null pointer as input is valid and function
 * will act like tmalloc.
 *
 * @param ptr Pointer to block to reuse.
 * @param size New requested size of block.
 * @return Pointer to allocated memory block.
 */
extern TALLOC_EXPORT void *
trealloc(void *ptr, size_t size);

/**
 * @brief Allocate memory and set it value to 0.
 * @param nelem Number of elements.
 * @param elsize Element size.
 * @return Pointer to allocated memory block.
 */
extern TALLOC_EXPORT void *
tcalloc(const size_t nelem, const size_t elsize);

/**
 * @brief Free allocated memory.
 * Use this function only with tmalloc(). Freeing of null address is valid.
 * Double freeing will cause call to talloc_exception (when enabled) and immediate
 * crash the application.
 *
 * @param ptr Memory to be freed.
 */
extern TALLOC_EXPORT void
tfree(void *ptr);

/**
 * @brief Preallocate memory block.
 * Allocates new block of system memory using default malloc. Use this method
 * in cases when allocated space is known to be not enough. Minimum reserve
 * size is TALLOC_BLOCK_SIZE.
 *
 * @param count Bytes to be preallocated.
 */
extern TALLOC_EXPORT void
talloc_expand(size_t count);

/**
 * @brief Print free block table into file stream.
 * @param file Appended file stream.
 */
extern TALLOC_EXPORT void
talloc_print_blocks(FILE *file);

/**
 * @brief Removes unused pools of memory.
 */
extern TALLOC_EXPORT void
talloc_optimize();

/**
 * @brief Returns allocated system memory in bytes.
 */
extern TALLOC_EXPORT size_t
talloc_allocated();

/**
 * @brief Returns used memory.
 */
extern TALLOC_EXPORT size_t
talloc_used();

/**
 * Set custom callback called instead of direct abort.
 * @param func Callback function.
 */
extern TALLOC_EXPORT void
talloc_set_err_func(talloc_err_f func);

#if TALLOC_FORCE_RESET
/**
 * @brief Force free allocated system memory and reset allocator.
 * Call this method only in special cases when memory allocated by the talloc
 * will never be used anymore.
 */
extern TALLOC_EXPORT void
talloc_force_reset();
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: TALLOC_H_QYTR1XNS */

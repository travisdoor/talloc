//*****************************************************************************
// talloc
//
// File:   ptr_tools.h
// Author: Martin Dorazil
// Date:   24/05/2017
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

#ifndef PTR_TOOLS_H_RVOIP9IG
#define PTR_TOOLS_H_RVOIP9IG

#include <stdbool.h>
#include <stddef.h>

/**
 * Check if pointer is properly aligned for specified alignment.
 * @param p Pointer to check.
 * @param alignment Alignment.
 * @return True if pointer is aligned.
 */
extern bool
is_aligned(const void *p, size_t alignment);

/**
 * Will align given address up to next aligned address based on alignment.
 * Memory allocated for aligned data must be sizeof(useful data) + alignment
 * due to memory leak prevent.
 * Alignment must be power of two.
 *
 * @param unaligned Pointer to be aligned. Also output value.
 * @param alignment Alignment must be power of two.
 * @param adjustment [out] The amount of bytes used for alignment.
 */
extern void
align_ptr_up(void **p, size_t alignment, ptrdiff_t *adjustment);

/**
 * Alignment of address with extra space for allocation header.
 *
 * @param unaligned Pointer to be aligned. Also output value.
 * @param alignment Alignment must be power of two.
 * @param header_size Size of header structure.
 * @param adjustment [out] The amount of bytes used for alignment.
 */
extern void
align_ptr_with_header(void **p, size_t alignment, size_t header_size, ptrdiff_t *adjustment);

#endif

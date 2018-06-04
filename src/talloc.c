//*****************************************************************************
// talloc
//
// File:   talloc.c
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

#include <string.h>
#include "talloc/talloc.h"
#include "heap.h"
#include "pool.h"
#include "types.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef struct universal_meta {
#if TALLOC_MEM_CHECKING
    uintptr_t check;
#endif
    size_t size;
} universal_meta_t;

#define GET_UNI_META_PTR(ptr) (universal_meta_t *)(ptr) - 1;

void *
tmalloc(size_t count)
{
    if (count == 0)
        return NULL;

#if TALLOC_USE_POOLS
    if (pool_cell_size(count) <= TALLOC_SMALL_TO)
        return pool_malloc(count);
#endif
    return heap_malloc(count);
}

void *
trealloc(void *ptr, size_t size)
{
    if (!ptr)
        return tmalloc(size);
    void *mem = tmalloc(size);
    memcpy(mem, ptr, size);
    tfree(ptr);
    return mem;
}

void *
tcalloc(const size_t nelem, const size_t elsize)
{
    const size_t size = nelem * elsize;
    void *mem = tmalloc(size);
    memset(mem, 0, size);
    return mem;
}

void
tfree(void *ptr)
{
    if (!ptr)
        return;
#if TALLOC_USE_POOLS
    const universal_meta_t *block = GET_UNI_META_PTR(ptr);

#if TALLOC_MEM_CHECKING
    if (block->check != (uintptr_t)ptr) {
        ABORT("pointer being freed was not allocated");
    }
#endif
    if (block->size <= TALLOC_SMALL_TO) {
        pool_free(ptr);
        return;
    }
#endif
    heap_free(ptr);
}

void
talloc_expand(size_t count)
{
    heap_expand(count);
}

void
talloc_print_blocks(FILE *file)
{
    heap_print_blocks(file);
}

void
talloc_optimize()
{
#if TALLOC_USE_POOLS
    pool_optimize();
#endif
}

size_t
talloc_allocated()
{
    return heap_allocated();
}

size_t
talloc_used()
{
    return heap_used();
}

#if TALLOC_FORCE_RESET
void
talloc_force_reset()
{
    heap_force_reset();
}
#endif

#ifdef __cplusplus
}
#endif

//*****************************************************************************
// talloc
//
// File:   pool.h
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

#include "pool.h"
#include "talloc/talloc_config.h"
#include "heap.h"
#include "types.h"
#include "utils.h"

typedef struct pool_meta {
    struct pool_meta *next;
} pool_meta_t;

typedef struct free_cell_meta {
    struct free_cell_meta *next;
} free_cell_meta_t;

typedef struct alloc_cell_meta {
#if TALLOC_MEM_CHECKING
    uintptr_t check;
#endif
    size_t size;
} alloc_cell_meta_t;

typedef struct category {
    free_cell_meta_t *head;
    pool_meta_t *next_pool;
    tatomic_bool flag;
    size_t used;
} category_t;

#define CATEGORY_COUNT (TALLOC_SMALL_TO / TALLOC_POOL_GROUP_MULT)
#define SIZE_TO_CATEGORY(s) (((s) / TALLOC_POOL_GROUP_MULT) - 1)
#define FREE_CELL_META_SIZE() sizeof(free_cell_meta_t)
#define ALLOC_CELL_META_SIZE() sizeof(alloc_cell_meta_t)
#define POOL_META_SIZE() sizeof(pool_meta_t)
#define MOVE_FREE_CELL_META_PTR(cell, n) (free_cell_meta_t *)((byte_t *)(cell) + (n))
#define GET_ALLOC_CELL_META(ptr) ((alloc_cell_meta_t *)(ptr)-1);

static category_t categories[CATEGORY_COUNT];

static void
new_category(category_t *category, size_t size)
{
    // allocate space for n objects of size on global heap
    void *new_head =
        heap_malloc((TALLOC_INIT_POOL_SIZE * size) + POOL_META_SIZE() + ALLOC_CELL_META_SIZE());
    pool_meta_t *new_pool = (pool_meta_t *)new_head;

    // store linked list of pools in category (for future freeing)
    new_pool->next = category->next_pool;
    category->next_pool = new_pool;

    new_head = (byte_t *)new_head + POOL_META_SIZE() + ALLOC_CELL_META_SIZE();
    free_cell_meta_t *iter = (free_cell_meta_t *)new_head;
    alloc_cell_meta_t *buf = NULL;
    for (size_t i = 0; i < TALLOC_INIT_POOL_SIZE - 1;
         ++i, iter = MOVE_FREE_CELL_META_PTR(iter, size)) {
        iter->next = MOVE_FREE_CELL_META_PTR(iter, size);
        buf = GET_ALLOC_CELL_META(iter);
        buf->size = size;
#if TALLOC_MEM_CHECKING
        buf->check = (uintptr_t)iter;
#endif
    }
    iter->next = NULL;
    buf = GET_ALLOC_CELL_META(iter);
    buf->size = size;
#if TALLOC_MEM_CHECKING
    buf->check = (uintptr_t)iter;
#endif

    category->head = new_head;
}

static free_cell_meta_t *
allocate(category_t *category, size_t size)
{
    LOCK(category->flag);

    if (category->head == NULL)
        new_category(category, size);
    free_cell_meta_t *ret = category->head;
    alloc_cell_meta_t *meta = GET_ALLOC_CELL_META(ret);
    ASSERT(meta->check == (uintptr_t)ret, "pool corrupted");
    category->head = category->head->next;
    category->used++;
    UNLOCK(category->flag);

    return ret;
}

static void
deallocate(category_t *category, free_cell_meta_t *free_cell)
{
    LOCK(category->flag);
    free_cell->next = category->head;
    category->head = free_cell;
    category->used--;
    UNLOCK(category->flag);
}

void *
pool_malloc(size_t count)
{
    count = pool_cell_size(count);
    const size_t category_id = SIZE_TO_CATEGORY(count);
    ASSERT(category_id < CATEGORY_COUNT, "pool category overflow");
    category_t *category = &categories[category_id];

    return allocate(category, count);
}

void
pool_free(void *ptr)
{
    alloc_cell_meta_t *cell = GET_ALLOC_CELL_META(ptr);
    const size_t size = cell->size;
    ASSERT(cell->check == (uintptr_t)ptr, "pool corrupted");
    const size_t category_id = SIZE_TO_CATEGORY(size);
    ASSERT(category_id < CATEGORY_COUNT, "pool category overflow");
    category_t *category = &categories[category_id];
    free_cell_meta_t *free_cell = (free_cell_meta_t *)ptr;
    deallocate(category, free_cell);
}

size_t
pool_cell_size(size_t size)
{
    if (size < FREE_CELL_META_SIZE())
        size = FREE_CELL_META_SIZE();
    size += ALLOC_CELL_META_SIZE();
    return NEXT_MULT_OF(size, TALLOC_POOL_GROUP_MULT);
}

void
pool_optimize(void)
{
    category_t *c = NULL;
    for (size_t i = 0; i < CATEGORY_COUNT; i++) {
        c = &categories[i];
        LOCK(c->flag);
        // check category, when its not used -> clean up all its allocated pools
        if (!c->used) {
            pool_meta_t *current = c->next_pool;
            pool_meta_t *prev = NULL;
            while (current) {
                prev = current;
                current = current->next;
                heap_free(prev);
            }
            c->next_pool = NULL;
            c->head = NULL;
        }
        UNLOCK(c->flag);
    }
}

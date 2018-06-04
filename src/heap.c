//*****************************************************************************
// talloc
//
// File:   heap.h
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

#include <stdlib.h>
#include <stdio.h>
#include "heap.h"
#include "talloc/talloc_config.h"
#include "utils.h"
#include "types.h"
#if TALLOC_FORCE_RESET
#include "vector.h"
#endif

typedef struct free_meta {
    struct free_meta *next;
    struct free_meta *prev;
    bool used;
    uintptr_t check;
    size_t size;
    // additional data for free blocks
    struct free_meta *left;
    struct free_meta *right;
    int height;
} free_meta_t;

typedef struct alloc_meta {
    struct free_meta *next;
    struct free_meta *prev;
    bool used;
#if TALLOC_MEM_CHECKING
    uintptr_t check;
#endif
    size_t size;
} alloc_meta_t;

#define FREE_META_SIZE sizeof(free_meta_t)
#define ALLOC_META_SIZE sizeof(alloc_meta_t)
#define GET_ALLOC_META_PTR(ptr) ((alloc_meta_t *)(ptr)-1)
#define MOVE_FREE_META_PTR(ptr, bytes) (free_meta_t *)((byte_t *)(ptr) + (bytes))
#define ADJUST_SIZE(size)                                                                          \
    {                                                                                              \
        (size) += TALLOC_ALIGNMENT - ((size) % TALLOC_ALIGNMENT);                                  \
    }

static tatomic_bool heap_flag;
static free_meta_t list_head;
static free_meta_t *free_tree_head;

#if TALLOC_FORCE_RESET
static vector_t sys_alloc_buffer;
#endif

static size_t allocated, used;

//*****************************************************************************
// TREE
//*****************************************************************************

inline static int
height(const free_meta_t *node)
{
    if (!node)
        return 0;
    return node->height;
}

inline static int
balance(const free_meta_t *node)
{
    if (!node)
        return 0;
    return height(node->left) - height(node->right);
}

inline static int
max_node(int a, int b)
{
    return (a > b) ? a : b;
}

static free_meta_t *
r_rotate(free_meta_t *pivot)
{
    free_meta_t *new_root = pivot->left;
    if (!new_root)
        return pivot;
    free_meta_t *stree = new_root->right;

    new_root->right = pivot;
    pivot->left = stree;
    pivot->height = max_node(height(pivot->left), height(pivot->right)) + 1;
    new_root->height = max_node(height(new_root->left), height(new_root->right)) + 1;

    return new_root;
}

static free_meta_t *
l_rotate(free_meta_t *pivot)
{
    free_meta_t *new_root = pivot->right;
    if (!new_root)
        return pivot;
    free_meta_t *stree = new_root->left;

    new_root->left = pivot;
    pivot->right = stree;
    pivot->height = max_node(height(pivot->left), height(pivot->right)) + 1;
    new_root->height = max_node(height(new_root->left), height(new_root->right)) + 1;

    return new_root;
}

static free_meta_t *
min_node(free_meta_t *node)
{
    free_meta_t *current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

inline static void
init_node(free_meta_t *node)
{
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
}

static free_meta_t *
remove_node(free_meta_t *root, free_meta_t *key)
{
    if (root == NULL)
        return root;
    if (key->size < root->size)
        root->left = remove_node(root->left, key);
    else if (key->size > root->size)
        root->right = remove_node(root->right, key);
    else {
        // cases with same size -> found node is not key node only has same
        // size
        if (root != key) {
            if (root->left)
                root->left = remove_node(root->left, key);
            if (root->right)
                root->right = remove_node(root->right, key);
        } else {
            // root == key -> node to be deleted
            if (!root->right || !root->left) {
                free_meta_t *tmp = root->left ? root->left : root->right;
                if (!tmp) {
                    // no child
                    root = NULL;
                } else {
                    // one child
                    root = tmp;
                }
            } else {
                // two children
                free_meta_t *tmp = min_node(root->right);
                root->right = remove_node(root->right, tmp);
                tmp->right = root->right;
                tmp->left = root->left;
                tmp->height = root->height;
                root = tmp;
            }
        }
    }

    if (root == NULL)
        return root;

    root->height = 1 + max_node(height(root->left), height(root->right));

    int bl = balance(root);

    // Left Left Case
    if (bl > 1 && balance(root->left) >= 0)
        return r_rotate(root);

    // Left Right Case
    if (bl > 1 && balance(root->left) < 0) {
        root->left = l_rotate(root->left);
        return r_rotate(root);
    }

    // Right Right Case
    if (bl < -1 && balance(root->right) <= 0)
        return l_rotate(root);

    // Right Left Case
    if (bl < -1 && balance(root->right) > 0) {
        root->right = r_rotate(root->right);
        return l_rotate(root);
    }

    return root;
}

static free_meta_t *
insert_node(free_meta_t *node, free_meta_t *new_node)
{
    ASSERT(new_node != node, "trying to insert already inserted node into heap tree");
    if (!node) {
        init_node(new_node);
        return new_node;
    }

    const size_t size = new_node->size;
    if (size < node->size) {
        // left
        node->left = insert_node(node->left, new_node);
    } else {
        node->right = insert_node(node->right, new_node);
    }

    // AVL tree balance
    node->height = 1 + max_node(height(node->left), height(node->right));
    const int bl = balance(node);

    // Left Left Case
    if (bl > 1 && size < node->left->size) {
        return r_rotate(node);
    }

    // Right Right Case
    if (bl < -1 && size > node->right->size) {
        return l_rotate(node);
    }

    // Left Right Case
    if (bl > 1 && size > node->left->size) {
        node->left = l_rotate(node->left);
        return r_rotate(node);
    }

    // Right Left Case
    if (bl < -1 && size < node->right->size) {
        node->right = r_rotate(node->right);
        return l_rotate(node);
    }

    return node;
}

static free_meta_t *
find_free_node(free_meta_t *node, size_t size)
{
    if (!node)
        return NULL;

    free_meta_t *current = node;
    free_meta_t *best_fit = NULL;
    size_t node_size = 0;

    while (current) {
        node_size = current->size;
        if (size <= node_size) {
            // can fit
            best_fit = current;
            current = current->left;
        }
        if (size > node_size)
            current = current->right;
    }
    return best_fit;
}

// static void print_tree(FILE *file, free_meta_t *node)
//{
//    if (!node)
//        return;
//    fprintf(file, "\nNode: %p Size: %zu\n", node, node->size);
//    fprintf(file, "   left: %p\n", node->left);
//    fprintf(file, "   right: %p\n", node->right);
//    fprintf(file, "   height: %i\n", node->height);
//
//    print_tree(file, node->left);
//    print_tree(file, node->right);
//}

//*****************************************************************************

//*****************************************************************************
// DOUBLE-LINKED LIST
//*****************************************************************************

static inline void
insert_block(free_meta_t *prev, free_meta_t *next, free_meta_t *block);
static inline void
remove_block(free_meta_t *block);

void
insert_block(free_meta_t *prev, free_meta_t *next, free_meta_t *block)
{
    if (prev)
        prev->next = block;
    if (next)
        next->prev = block;
    block->prev = prev;
    block->next = next;
}

void
insert_block_sorted(free_meta_t *block)
{
    free_meta_t *current = list_head.next;
    free_meta_t *prev = &list_head;

    while (current && (block > current)) {
        ASSERT(current != current->next, "memory corrupted");
        prev = current;
        current = current->next;
    }
    insert_block(prev, current, block);
}

void
remove_block(free_meta_t *block)
{
    free_meta_t *prev = block->prev;
    free_meta_t *next = block->next;
    if (prev)
        prev->next = next;
    if (next)
        next->prev = prev;
}
//*****************************************************************************

static free_meta_t *
new_space(size_t size)
{
    if (size < TALLOC_BLOCK_SIZE)
        size = TALLOC_BLOCK_SIZE;

    free_meta_t *new_block = (free_meta_t *)malloc(size);
    if (!new_block)
        ABORT("bad allocation");

    new_block->size = size;
    new_block->used = false;
    insert_block_sorted(new_block);
    free_tree_head = insert_node(free_tree_head, new_block);

#if TALLOC_FORCE_RESET
    // store for future free
    if (sys_alloc_buffer.data == NULL)
        vector_init(&sys_alloc_buffer);
    vector_push_back(&sys_alloc_buffer, new_block);
#endif

    allocated += size;
    return new_block;
}

// determinates if block can be merged from right with another block
static free_meta_t *
can_merge_next(free_meta_t *block)
{
    if (!block->next || block->next->used)
        return NULL;

    free_meta_t *last = MOVE_FREE_META_PTR(block, block->size);
    if (last == block->next)
        return block->next;
    return NULL;
}

// determinates if block can be merged from left with another block
static free_meta_t *
can_merge_prev(free_meta_t *block)
{
    if (!block->prev || block->prev->used)
        return NULL;

    size_t size = block->prev->size;
    free_meta_t *prev_last = MOVE_FREE_META_PTR(block->prev, size);
    if (prev_last == block)
        return block->prev;
    return NULL;
}

// allocate block with proper alignment and save allocation meta data
static void *
allocate(free_meta_t *block, size_t size)
{
    const size_t rem_space = block->size - size;
    free_tree_head = remove_node(free_tree_head, block);
    if (rem_space > FREE_META_SIZE) {
        free_meta_t *new_block = MOVE_FREE_META_PTR(block, size);

        new_block->size = rem_space;
        new_block->used = false;
        insert_block(block, block->next, new_block);
        free_tree_head = insert_node(free_tree_head, new_block);
    } else {
        size += rem_space;
    }

    alloc_meta_t *alloc_block = (alloc_meta_t *)block;
    alloc_block->size = size;
    alloc_block->used = true;
#if TALLOC_MEM_CHECKING
    alloc_block->check = (uintptr_t)(alloc_block + 1);
#endif
    return (alloc_block + 1);
}

static void
deallocate(free_meta_t *block)
{
    free_meta_t *new_block = block;
    block->used = false;

    free_meta_t *neighbour = can_merge_next(block);
    if (neighbour) {
        // remove from tree
        free_tree_head = remove_node(free_tree_head, neighbour);
        // remove from list
        remove_block(neighbour);
        block->size = block->size + neighbour->size;
    }

    neighbour = can_merge_prev(block);
    if (neighbour) {
        // remove from tree
        free_tree_head = remove_node(free_tree_head, neighbour);
        // remove from list
        remove_block(block);
        neighbour->size = block->size + neighbour->size;
        new_block = neighbour;
    }

    free_tree_head = insert_node(free_tree_head, new_block);
}

void *
heap_malloc(size_t count)
{
    count = count + ALLOC_META_SIZE;
    if (count < FREE_META_SIZE)
        count = FREE_META_SIZE;
    ADJUST_SIZE(count);

    LOCK(heap_flag);
    free_meta_t *block = find_free_node(free_tree_head, count);
    if (!block) {
        // no free block with requested size -> allocate new one
        block = new_space(count);
    }

    ASSERT(block->size >= count, "not enough space");
    void *ret = allocate(block, count);
    used += count;
    UNLOCK(heap_flag);

    return ret;
}

void
heap_free(void *ptr)
{
    if (!ptr)
        return;
    free_meta_t *block = (free_meta_t *)GET_ALLOC_META_PTR(ptr);

    LOCK(heap_flag);
    used -= block->size;
    deallocate(block);
    UNLOCK(heap_flag);
}

void
heap_expand(size_t count)
{
    if (count < TALLOC_BLOCK_SIZE)
        count = TALLOC_BLOCK_SIZE;

    new_space(count);
}

void
heap_print_blocks(FILE *file)
{
    fprintf(file, "\n");
    fprintf(file, "┏━━━━━━━━━━━━━━━━━━┯━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━┯━━━━━━━━━━━━━━━━━━┓\n");
    fprintf(file, "┃ address          │   size   │ previous         │ next             ┃\n");
    fprintf(file, "┠──────────────────┼──────────┼──────────────────┼──────────────────┨\n");
    free_meta_t *current = &list_head;
    LOCK(heap_flag);
    while (current) {
        if (!current->used)
            fprintf(file, "┃ %16p │ %8zu │ %16p │ %16p ┃\n", current, current->size, current->prev,
                    current->next);
        if (current == current->next)
            break;
        current = current->next;
    }
    UNLOCK(heap_flag);
    fprintf(file, "┗━━━━━━━━━━━━━━━━━━┷━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━┷━━━━━━━━━━━━━━━━━━┛\n\n");

    //    print_tree(file, free_tree_head);
}

size_t
heap_allocated(void)
{
    return allocated;
}

size_t
heap_used(void)
{
    return used;
}

#if TALLOC_FORCE_RESET
void
heap_force_reset(void)
{
    for (size_t i = 0; i < sys_alloc_buffer.size; i++) {
        free(vector_at(&sys_alloc_buffer, i));
    }
    vector_free(&sys_alloc_buffer);

    list_head = (const free_meta_t){0};
    free_tree_head = NULL;
    allocated = 0;
    used = 0;
}
#endif

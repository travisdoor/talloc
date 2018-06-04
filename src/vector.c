//*****************************************************************************
// talloc
//
// File:   vector.c
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

#include <stdio.h>
#include <stdlib.h>

#include "vector.h"
#include "utils.h"

#define VECTOR_INIT_CAPACITY 128

static void
vector_double_if_needed(vector_t *vec)
{
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, sizeof(void *) * vec->capacity);
    }
}

void
vector_init(vector_t *vec)
{
    vec->size = 0;
    vec->capacity = VECTOR_INIT_CAPACITY;
    vec->data = malloc(sizeof(void *) * VECTOR_INIT_CAPACITY);
}

void
vector_free(vector_t *vec)
{
    free(vec->data);
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

void
vector_push_back(vector_t *vec, void *data)
{
    vector_double_if_needed(vec);
    vec->data[vec->size++] = data;
}

void *
vector_at(vector_t *vec, size_t index)
{
    ASSERT(index < vec->size && index >= 0, "vector index out of range!!!");
    return vec->data[index];
}

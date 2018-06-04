//*****************************************************************************
// talloc
//
// File:   vector.h
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

#ifndef VECTOR_H_UPGBSXNL
#define VECTOR_H_UPGBSXNL

#include <stddef.h>

typedef struct vector {
    size_t size;
    size_t capacity;
    void **data;
} vector_t;

void
vector_init(vector_t *vec);

void
vector_free(vector_t *vec);

void
vector_push_back(vector_t *vec, void *data);

void *
vector_at(vector_t *vec, size_t index);

#endif /* end of include guard: VECTOR_H_UPGBSXNL */

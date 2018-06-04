//*****************************************************************************
// talloc
//
// File:   utils.h
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

#ifndef UTILS_H_LRSUGIAD
#define UTILS_H_LRSUGIAD

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "tatomic.h"
#include "talloc/talloc.h"

extern talloc_err_f err_f;

#define ABORT(msg)                                                                                 \
    do {                                                                                           \
        err_f(msg);                                                                                \
        abort();                                                                                   \
    } while (0);

#define ASSERT(exp, msg) assert((exp) && (msg))

#define LOCK(flag)                                                                                 \
    while (tatomic_exchange(&(flag), true)) {                                                      \
        ;                                                                                          \
    }
#define UNLOCK(flag) tatomic_store(&(flag), false)
#define TRY_LOCK(flag) !tatomic_exchange(&(flag), true)
#define WAIT_LOCK(flag)                                                                            \
    while (tatomic_load(&(flag))) {                                                                \
        ;                                                                                          \
    }

#define NEXT_MULT_OF(n, mult) ((n) + (mult)-1 - ((n)-1) % (mult))

#endif /* end of include guard: UTILS_H_LRSUGIAD */

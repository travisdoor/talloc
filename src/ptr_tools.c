//*****************************************************************************
// talloc
//
// File:   ptr_tools.c
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

#include <stdint.h>
#include "ptr_tools.h"
#include "utils.h"

bool
is_aligned(const void *p, size_t alignment)
{
    return (uintptr_t)p % alignment == 0;
}

void
align_ptr_up(void **p, size_t alignment, ptrdiff_t *adjustment)
{
    if (is_aligned(*p, alignment)) {
        *adjustment = 0;
        return;
    }

    const size_t mask = alignment - 1;
    ASSERT((alignment & mask) == 0, "wrong alignemet"); // pwr of 2
    const uintptr_t i_unaligned = (uintptr_t)(*p);
    const uintptr_t misalignment = i_unaligned & mask;
    *adjustment = alignment - misalignment;
    *p = (void *)(i_unaligned + *adjustment);
}

void
align_ptr_with_header(void **p, size_t alignment, size_t header_size, ptrdiff_t *adjustment)
{
    unsigned char *chptr = (unsigned char *)(*p);
    chptr += header_size;
    *p = (void *)chptr;
    align_ptr_up(p, alignment, adjustment);
    *adjustment += header_size;
}

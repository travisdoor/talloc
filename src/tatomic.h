//*****************************************************************************
// talloc
//
// File:   tatomic.h
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

#ifndef TATOMIC_H_7PDCBQAZ
#define TATOMIC_H_7PDCBQAZ

#ifdef _MSC_VER
#include <Windows.h>
#define tatomic_exchange(ex, val) InterlockedExchange((LONG *)(ex), (val))
#define tatomic_store(st, val) ((*st) = (val))
#define tatomic_load(l) atomic_load((l)) * (l)

typedef volatile bool tatomic_bool;
#else
#include <stdatomic.h>
#define tatomic_exchange(ex, val) atomic_exchange((ex), (val))
#define tatomic_store(st, val) atomic_store((st), (val))
#define tatomic_load(l) atomic_load((l))

typedef atomic_bool tatomic_bool;
#endif
#endif /* end of include guard: TATOMIC_H_7PDCBQAZ */

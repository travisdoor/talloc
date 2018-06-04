# talloc 
Author: Martin Dorazil

Year: 2017

Current version: 1.4.0

## Introduction
Custom thread-safe malloc implementation with pooling of small objects (~2KB) and fast AVL tree based best-fit memory allocation written in C.

## Change log
  * 1.4.0 Add exception method setter. 
  * 1.3.0 Add exception handling and checking for invalid pointer freeing.
  * 1.2.0 Locking in pool and MSVC support.
  * 1.1.1 Fix aba problem random crashes in multithread pool access.
  * 1.1.0 Add talloc_force_reset method.
  * 1.0.1 Add documentation and minor bug fixes.
  * 1.0.0 Initial version.

## How to use
### Building 
#### Supported compilers
  * GCC
  * clang
  * MinGW
  * MSVC
#### MacOS or Linux
Build the project with cmake.
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
#### Windows
Build talloc on windows with MinGW using cmake.
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. -G"MinGW Makefiles"
cmake --build . --config Release
```
To use the dll with MSVC you must create lib file first with lib.exe utility. [Guide here.](http://www.mingw.org/wiki/MSVC_and_MinGW_DLLs)

Build talloc on windows with MSVC using cmake.
```bash
mkdir build
cd build
cmake .. -G"Visual Studio 15 Win64"
cmake --build . --config Release
```

### Linking
Link libtalloc library and include include/talloc.h interface.

### Usage
```c
#include "talloc.h"

// allocation
foo_t *foo = (foo_t *)tmalloc(sizeof(foo_t));
// some useful stuff
tfree(foo);
foo = NULL;
```

## Other
Large memory chunk is preallocated on first call of tmalloc. Every next allocation lives only in this preallocated space (see config.h). When the allocator gets out of free space, new large block is preallocated. Preallocation can be done manually using talloc_expand method.

Memory allocations under TALLOC_SMALL_TO (defined in talloc_config.h) are organized into pools. Call talloc_optimize to free unused pools.

Preallocated memory will never be returned to system automatically, you can use talloc_force_reset method (can be enabled in config.h) to free system memory and reset allocator to initial state (make sure that already allocated memory will never be used after reset). The memory will be returned back to system on application exit on most platforms.

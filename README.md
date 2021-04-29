# pbrt
A ray tracer based on [pbrt-v3](http://www.pbr-book.org/3ed-2018/contents.html).

## Build
A **C++20** compliant compiler is needed.  
The project is regularly built with:  
 - MSVC 19.28
 - Clang 11.0
 - GCC 10.2

**CMake 3.14 or higher**.  
Note that **warnings are treated as errors by default**,  
so builds may fail because of a warning with other compiler versions.  
If this is the case, you can disable the corresponding option.

CMake options:
 - PBRT_FLOAT_AS_DOUBLE - use 64-bit floats (off by default)
 - PBRT_TREAT_WARNINGS_AS_ERRORS - treat compiler warnings as errors (on by default)

Example:  
 ```
 # build with 64-bit floats
 $ git clone https://github.com/IDragnev/pbrt.git  
 $ cd pbrt  
 $ mkdir build && cd build  
 $ cmake -D PBRT_FLOAT_AS_DOUBLE=ON ..  
 $ cmake --build .  
 ```

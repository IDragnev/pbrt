# pb-ray-tracer
A ray tracer based on [pbrt-v3](http://www.pbr-book.org/3ed-2018/contents.html).

## Build
A **C++17** compliant compiler is needed.  
The project is regularly built with:  
 - MSVC 19.27
 - Clang 10.0
 - GCC 10.1
 - GCC 9.3 (less often)
 - GCC 7.5 (less often)

CMake options:
 - RT_FLOAT_AS_DOUBLE - use 64-bit floats (off by default)
 
Example (build with 64-bit floats):  
 ```
 $ git clone --recursive https://github.com/IDragnev/pb-ray-tracer.git  
 $ cd pb-ray-tracer  
 $ mkdir src/build && cd src/build  
 $ cmake -D RT_FLOAT_AS_DOUBLE=ON ../..  
 $ cmake --build .  
 ```

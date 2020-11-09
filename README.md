# pb-ray-tracer
A ray tracer based on [pbrt-v3](http://www.pbr-book.org/3ed-2018/contents.html).

## Build
A **C++17** compliant compiler is needed.  
The project is regularly built with:  
 - MSVC 19.27
 - Clang 10.0
 - GCC 10.2
 - GCC 9.3 (less often)  

Note that **the warning level is set to Error for CMake builds**,  
so builds may fail because of a warning with other compiler versions.  
If this is the case, you can lower the warning level from [the main CMake file](https://github.com/IDragnev/pb-ray-tracer/blob/master/CMakeLists.txt).    

CMake options:
 - RT_FLOAT_AS_DOUBLE - use 64-bit floats (off by default)
 
Example (build with 64-bit floats):  
 ```
 $ git clone --recursive https://github.com/IDragnev/pb-ray-tracer.git  
 $ cd pb-ray-tracer  
 $ mkdir build && cd build  
 $ cmake -D RT_FLOAT_AS_DOUBLE=ON ..  
 $ cmake --build .  
 ```

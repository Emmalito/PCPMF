# JLParser

## Introduction

**JLParser** is a C++ tool to read files with format

This version requires your compiler to support `C++-17`.
```
# Some comment

name1 <type1> value1
name2 <type2> value2
...
```
where
- `name` is a string made of words
- `type` can be `int`, `long`, `float`, `vector`, `string` or `void *`
- `value` is the value associated to the key `name` and is supposed to match `type`. Elements of a vector are separated by white spaces.


## Usage

```
git submodule add git@gricad-gitlab.univ-grenoble-alpes.fr:jlparser/parser.git 3rdparty/jlparser
cd 3rdparty/jlparser
git submodule update --init
```

In your `CMakeLists.txt`, add

```
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_subdirectory(3rdparty/jlparser)
add_executable(hello-world hello_world.cpp)
target_link_libraries(hello-world jl_parser)
```
and then simply add `#include "jlparser/parser.hpp"` to your code.

This project has 4 branches:
- The `master` branch is for `C++-11` compilers
- The `nonstd-variant` is for `C++-11` compilers. It does not require the full support of the whole `C++-11` standard.
- The `c++-17` branch is for `C++-17` compilers
- The `boost` branch is for other cases. Note that you need to install the Boost library.

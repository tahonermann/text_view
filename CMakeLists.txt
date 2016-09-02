
cmake_minimum_required(VERSION 2.8)

project(text_view CXX)

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fconcepts")
endif()

find_path(CMCSTL2_INCLUDE_DIR "experimental/ranges/concepts")

include_directories(include)
include_directories(${CMCSTL2_INCLUDE_DIR})

add_subdirectory(examples)
add_subdirectory(test)


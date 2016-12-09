cmake_minimum_required(VERSION 2.8)

include(${CMAKE_ROOT}/Modules/FindBoost.cmake)

if (MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /MD /W3 /WX")
    add_definitions(/D_CRT_SECURE_NO_WARNINGS)
else()
    set(CMAKE_CXX_FLAGS "-std=gnu++11")
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -O3")
endif()

if (TESTS_WITH_HEAP_USAGE)
    add_definitions(-DTESTS_WITH_HEAP_USAGE)
endif()

find_package(Boost)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(-DBOOST_LEXICAL_CAST)
endif()

add_executable(tester tester.cpp)

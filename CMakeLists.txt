cmake_minimum_required(VERSION 3.7...3.23)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION.${CMAKE_MINOR_VERSION}})
endif()

project(Hypoinput LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_subdirectory(${PROJECT_NAME})

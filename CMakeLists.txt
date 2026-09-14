cmake_minimum_required(VERSION 3.16)
project(IntelligentLoadBalancer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

add_library(load_balancer_lib INTERFACE)
target_include_directories(load_balancer_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(load_balancer
    src/main.cpp
    src/Logger.cpp
    src/BackendServer.cpp
    src/LoadBalancingStrategy.cpp
    src/LoadBalancer.cpp
    src/TrafficSimulator.cpp
)

target_include_directories(load_balancer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(load_balancer PRIVATE load_balancer_lib)

add_executable(load_balancer_tests
    tests/test_main.cpp
    src/Logger.cpp
    src/BackendServer.cpp
    src/LoadBalancingStrategy.cpp
    src/LoadBalancer.cpp
    src/TrafficSimulator.cpp
)

target_include_directories(load_balancer_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(load_balancer_tests PRIVATE load_balancer_lib)

add_test(NAME load_balancer_tests COMMAND load_balancer_tests)

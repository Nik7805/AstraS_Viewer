cmake_minimum_required(VERSION 3.1.15)
project(AstraMiniSViewer LANGUAGES CXX C)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    src/net/udpsender.cpp
    src/net/udpreceiver.cpp
)

set(OrbbecSDK_ROOT_DIR ${CMAKE_SOURCE_DIR}/SDK)
set(OrbbecSDK_LIBRARY_DIRS ${OrbbecSDK_ROOT_DIR}/lib)
set(OrbbecSDK_INCLUDE_DIR ${OrbbecSDK_ROOT_DIR}/include)

include_directories(${OrbbecSDK_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/src/common
${CMAKE_SOURCE_DIR}/src/net
)

link_directories(${OrbbecSDK_LIBRARY_DIRS})
find_package(OpenCV REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OrbbecSDK_LIBRARY_DIRS}/libOrbbecSDK.so
    ${OpenCV_LIBS}
)
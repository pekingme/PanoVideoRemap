cmake_minimum_required(VERSION 2.6)
project(panovideoremap)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug)
ENDIF()

set(PANOVIDEO_LIB_TYPE SHARED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(OpenCV REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem system)

include_directories(
${PROJECT_SOURCE_DIR}
${PROJECT_SOURCE_DIR}/include/
)

set(PANOVIDEO_HEADERS
include/utils.h
include/pano_video_mapper.h
include/frame_mapper.h
include/camera.h
include/mesh.h
include/combined_video_clip.h
include/video_clip.h
)

add_library(${PROJECT_NAME} ${PANOVIDEO_LIB_TYPE}
src/utils.cpp
src/pano_video_mapper.cpp
src/frame_mapper.cpp
src/camera.cpp
src/mesh.cpp
src/combined_video_clip.cpp
src/video_clip.cpp
${PANOVIDEO_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${Boost_FILESYSTEM_LIBRARY}
${Boost_SYSTEM_LIBRARY}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/examples)
add_executable(pano_video_example pano_video_example.cpp)
target_link_libraries(pano_video_example ${PROJECT_NAME})

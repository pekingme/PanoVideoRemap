cmake_minimum_required(VERSION 2.6)
project(panovideoremap)

set(PANOVIDEO_LIB_TYPE SHARED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")

find_package(OpenCV REQUIRED)
find_package(FFmpeg REQUIRED)

include_directories(
${PROJECT_SOURCE_DIR}
)

set(PANOVIDEO_HEADERS
pano_video_mapper.h
frame_mapper.h
video_feed.h
camera.h
utils.h
mesh.h
video_synchronizer.h
video_clip.h
)

add_library(${PROJECT_NAME} ${PANOVIDEO_LIB_TYPE}
pano_video_mapper.cpp
frame_mapper.cpp
video_feed.cpp
camera.cpp
utils.cpp
mesh.cpp
video_synchronizer.cpp
video_clip.cpp
${PANOVIDEO_HEADERS}
)

target_link_libraries(${PROJECT_NAME}
${OpenCV_LIBS}
${FFMPEG_LIBRARIES}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/examples)
add_executable(pano_video_example pano_video_example.cpp)
target_link_libraries(pano_video_example ${PROJECT_NAME})
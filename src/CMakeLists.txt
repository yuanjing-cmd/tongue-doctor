set(rknpu2_yolov7_file rknpu2/yolov7.cc)
if (TARGET_SOC STREQUAL "rv1106" OR TARGET_SOC STREQUAL "rv1103")
    add_definitions(-DRV1106_1103)
    set(rknpu2_yolov7_file rknpu2/yolov7_rv1106_1103.cc)
    #dma
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/allocator/dma)
endif()

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

set(rknpu_yolov7_file rknpu2/yolov7.cc)

if(TARGET_SOC STREQUAL "rv1106" OR TARGET_SOC STREQUAL "rv1103")
    add_definitions(-DRV1106_1103)
    set(rknpu_yolov7_file rknpu2/yolov7_rv1106_1103.cc)
    # dma
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../3rdparty/allocator/dma)
endif()

if(TARGET_SOC STREQUAL "rk1808" OR TARGET_SOC STREQUAL "rv1109" OR TARGET_SOC STREQUAL "rv1126")
    add_definitions(-DRKNPU1)
    set(rknpu_yolov7_file rknpu1/yolov7.cc)
endif()

set(TONGUE_DOCTOR_FOLDER
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/logger
    ${CMAKE_CURRENT_SOURCE_DIR}/view
)

foreach(folder ${TONGUE_DOCTOR_FOLDER})
    file(GLOB srcs ${folder}/*.cc)
    list(APPEND TONGUE_DOCTOR_SRC ${srcs})
endforeach()

add_executable(${PROJECT_NAME}
    ${TONGUE_DOCTOR_SRC}
    ${rknpu_yolov7_file}
)

target_link_libraries(${PROJECT_NAME}
    NGUI
    imageutils
    fileutils
    imagedrawing
    ${LIBRKNNRT}
    opencv2
    rkllm
    dl
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${TONGUE_DOCTOR_FOLDER}

    ${LIBRKNNRT_INCLUDES}
    ${LIBTIMER_INCLUDES}
)

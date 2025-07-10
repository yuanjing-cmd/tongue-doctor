cmake_minimum_required(VERSION 3.15)

project(rknn_model_zoo_3rdparty_libs)

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    set (TARGET_LIB_ARCH ${CMAKE_ANDROID_ARCH_ABI})
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set (TARGET_LIB_ARCH aarch64)
    else()
        set (TARGET_LIB_ARCH armhf)
    endif()
    if (CMAKE_C_COMPILER MATCHES "uclibc")
        set (TARGET_LIB_ARCH ${TARGET_LIB_ARCH}_uclibc)
    endif()
endif()

# stb image
set(STB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/stb_image)
set(STB_INCLUDES ${STB_PATH} PARENT_SCOPE)

# jpeg turbo
set(JPEG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/jpeg_turbo)
set(LIBJPEG ${JPEG_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/libturbojpeg.a PARENT_SCOPE)
set(LIBJPEG_INCLUDES ${JPEG_PATH}/include PARENT_SCOPE)

# NGUI
set(NGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/ngui)
list(APPEND NGUI_INCLUDE
    ${NGUI_PATH}/include
    ${NGUI_PATH}/include/deps
    ${NGUI_PATH}/include/indev
    ${NGUI_PATH}/include/layout
    ${NGUI_PATH}/include/msg
    ${NGUI_PATH}/include/painter
    ${NGUI_PATH}/include/widget
)
add_library(NGUI INTERFACE)
target_include_directories(NGUI INTERFACE ${NGUI_INCLUDE})
target_link_directories(NGUI INTERFACE ${NGUI_PATH}/lib)
target_link_libraries(NGUI INTERFACE NeatGUI asound avcodec avformat avutil graphic_lsf rockchip_mpp rockit drm)

# opencv2
set(OpenCV_DIR ${CMAKE_CURRENT_SOURCE_DIR}/opencv/share/OpenCV)
find_package(OpenCV REQUIRED core videoio)
add_library(opencv2 INTERFACE)
target_include_directories(opencv2 INTERFACE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(opencv2 INTERFACE ${OpenCV_LIBS})
get_target_property(OPENCV_LIB_PATH opencv_core LOCATION)
get_filename_component(OPENCV_LIB_DIR ${OPENCV_LIB_PATH} DIRECTORY)
file(GLOB OPENCV_LIBS "${OPENCV_LIB_DIR}/*.so*")
install(FILES ${OPENCV_LIBS} DESTINATION lib)

# rkllm runtime
set(RKLLM_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rkllm-runtime/Linux/librkllm_api)
add_library(rkllm INTERFACE)
target_link_directories(rkllm INTERFACE ${RKLLM_PATH}/aarch64)
target_link_libraries(rkllm INTERFACE rkllmrt gomp)
target_include_directories(rkllm INTERFACE ${RKLLM_PATH}/include)
file(GLOB RKLLM_LIBS "${RKLLM_PATH}/aarch64/*.so*")
install(FILES ${RKLLM_LIBS} DESTINATION lib)

# rknn runtime
# for rknpu2
if (TARGET_SOC STREQUAL "rk3588" OR TARGET_SOC STREQUAL "rk3576" OR TARGET_SOC STREQUAL "rk356x" OR TARGET_SOC STREQUAL "rv1106" OR TARGET_SOC STREQUAL "rv1103")
    set(RKNN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rknpu2)
    if (TARGET_SOC STREQUAL "rk3588" OR TARGET_SOC STREQUAL "rk356x" OR TARGET_SOC STREQUAL "rk3576" )
        set(LIBRKNNRT ${RKNN_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/librknnrt.so)
    endif()
    if (TARGET_SOC STREQUAL "rv1106" OR TARGET_SOC STREQUAL "rv1103")
        set(LIBRKNNRT ${RKNN_PATH}/${CMAKE_SYSTEM_NAME}/armhf-uclibc/librknnmrt.so)
    endif()
    set(LIBRKNNRT_INCLUDES ${RKNN_PATH}/include PARENT_SCOPE)
endif()

# for rknpu1
if(TARGET_SOC STREQUAL "rk1808" OR TARGET_SOC STREQUAL "rv1109" OR TARGET_SOC STREQUAL "rv1126")
    set(RKNN_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rknpu1)

    set(LIBRKNNRT ${RKNN_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/librknn_api.so)

    set(LIBRKNNRT_INCLUDES ${RKNN_PATH}/include PARENT_SCOPE)
endif()
install(PROGRAMS ${LIBRKNNRT} DESTINATION lib)
set(LIBRKNNRT ${LIBRKNNRT} PARENT_SCOPE)

# rga
set(RGA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/librga)
set(LIBRGA ${RGA_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/librga.a PARENT_SCOPE)
set(LIBRGA_INCLUDES ${RGA_PATH}/include PARENT_SCOPE)
install(PROGRAMS ${RGA_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/librga.so DESTINATION lib)

# timer
set(TIMER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/timer)
set(LIBTIMER_INCLUDES ${TIMER_PATH} PARENT_SCOPE)

# libsndfile
set(LIBSNDFILE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libsndfile)
set(LIBSNDFILE_INCLUDES ${LIBSNDFILE_PATH}/include PARENT_SCOPE)
set(LIBSNDFILE ${LIBSNDFILE_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/libsndfile.a PARENT_SCOPE)

# fftw
set(LIBFFTW_PATH ${CMAKE_CURRENT_SOURCE_DIR}/fftw)
set(LIBFFTW_INCLUDES ${LIBFFTW_PATH}/include PARENT_SCOPE)
set(LIBFFTW ${LIBFFTW_PATH}/${CMAKE_SYSTEM_NAME}/${TARGET_LIB_ARCH}/libfftw3f.a PARENT_SCOPE)
cmake_minimum_required(VERSION 3.21)

project(tongue-doctor)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(common)

if (ENABLE_ASAN)
	message(STATUS "BUILD WITH ADDRESS SANITIZER")
	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif ()

# set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(${PROJECT_SOURCE_DIR}/3rdparty)
add_subdirectory(${PROJECT_SOURCE_DIR}/utils)

add_subdirectory(src)

set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")

install(TARGETS ${PROJECT_NAME} DESTINATION .)
file(GLOB PIC_FILES "${PROJECT_SOURCE_DIR}/pic/*")
install(FILES ${PIC_FILES} DESTINATION pic)
file(GLOB MODEL_FILES "${PROJECT_SOURCE_DIR}/model/*")
install(FILES ${MODEL_FILES} DESTINATION model)

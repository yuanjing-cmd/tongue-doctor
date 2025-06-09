# 设置编译器路径
set(TOOLCHAIN_ROOT "/opt/RK3588/aarch64-buildroot-linux-gnu_sdk-buildroot/bin/aarch64-buildroot-linux-gnu")

set(CMAKE_C_COMPILER "${TOOLCHAIN_ROOT}-gcc")
set(CMAKE_CXX_COMPILER "${TOOLCHAIN_ROOT}-g++")
set(CMAKE_LINKER "${TOOLCHAIN_ROOT}-ld")

# 设置目标平台和架构
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(TARGET_SOC rk3588)

set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS ${CXX_FLAGS}
    -DCHECK_PTHREAD_RETURN_VALUE
    -D_FILE_OFFSET_BITS=64
    -D__USE_FILE_OFFSET64=1
    -D_FORTIFY_SOURCE=2
    -Wall
    -Wextra
    # -Werror
    -Wno-unused-parameter
    -Wno-unused-function
    -Werror=return-type
    # -Werror=sign-compare
    # -Werror=sign-conversion
    -Wsign-compare
    -Wsign-conversion
    -Wno-missing-field-initializers
    -Woverloaded-virtual
    -Wwrite-strings
    -Werror=reorder
    # -Wshadow
    -Wformat-security
    -rdynamic
    -pthread
    -fPIE
    -fstack-protector-strong
    -fvisibility=hidden
    -fvisibility-inlines-hidden
    # -fno-rtti
)

# 开启GOT表保护
set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-z,relro,-z,now")

# 随机化
set(LINK_FLAGS ${LINK_FLAGS} "-pie")

# 隐藏依赖的静态库符号
set(LINK_FLAGS ${LINK_FLAGS} "-Wl,--exclude-libs=ALL")

# 不链接未用函数
set(LINK_FLAGS ${LINK_FLAGS} "-Wl,--gc-sections")

set(LINK_FLAGS ${LINK_FLAGS} "-Wl,-z,defs")

add_link_options(${LINK_FLAGS})

string(REPLACE ";" " " CXX_FLAGS "${CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    # Debug 配置
    set(CMAKE_C_FLAGS_DEBUG "-O0 -g -ggdb")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -ggdb -fpermissive")
    # set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -fsanitize=address -fsanitize-recover -fno-omit-frame-pointer")
    # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fsanitize-recover -fno-omit-frame-pointer")
    
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    # Release 配置
    set(CMAKE_C_FLAGS_RELEASE "-O2 -g0 -DNDEBUG")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fno-strict-aliasing -funwind-tables")

    set(CMAKE_CXX_FLAGS_RELEASE "-O2 -g0 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -ffunction-sections -fdata-sections")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fno-strict-aliasing -funwind-tables")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wno-invalid-offsetof -fpermissive")
    
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    # RelWithDebInfo 配置
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -ffunction-sections -fdata-sections")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -fno-strict-aliasing -funwind-tables")

    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -ffunction-sections -fdata-sections")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fno-strict-aliasing -funwind-tables")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Wno-invalid-offsetof -fpermissive")

else ()
    # 其他配置
endif ()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILENAME__='\"$(notdir $(subst .o,,$(abspath $@)))\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__FILENAME__='\"$(notdir $(subst .o,,$(abspath $@)))\"'")

link_libraries(gcc_s c)

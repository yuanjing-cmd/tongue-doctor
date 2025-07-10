function(create_versioned_symlinks LIB_DIR OUTPUT_DIR)
    if(NOT EXISTS ${OUTPUT_DIR})
        file(MAKE_DIRECTORY ${OUTPUT_DIR})
    endif()

    file(GLOB lib_files LIST_DIRECTORIES false "${LIB_DIR}/*.so*")
    
    foreach(lib_file ${lib_files})
        get_filename_component(lib_name ${lib_file} NAME)
        
        # 匹配 libfoo.so.1.2.3 格式
        if(lib_name MATCHES "^(.*\\.so)\\.[0-9]+\\.[0-9]+\\.[0-9]+$")
            create_symlink("${LIB_DIR}" "${lib_file}" "${OUTPUT_DIR}" "${CMAKE_MATCH_1}")
        
        # 匹配 libfoo.so.1.2 格式
        elseif(lib_name MATCHES "^(.*\\.so)\\.[0-9]+\\.[0-9]+$")
            create_symlink("${LIB_DIR}" "${lib_file}" "${OUTPUT_DIR}" "${CMAKE_MATCH_1}")
        
        # 匹配 libfoo.so.1 格式
        elseif(lib_name MATCHES "^(.*\\.so)\\.[0-9]+$")
            create_symlink("${LIB_DIR}" "${lib_file}" "${OUTPUT_DIR}" "${CMAKE_MATCH_1}")
        endif()
    endforeach()
endfunction()

function(create_symlink SOURCE_DIR SOURCE_FILE OUTPUT_DIR OUTPUT_FILE)
    get_filename_component(SOURCE_NAME ${SOURCE_FILE} NAME)
    message("create_symlink: ${SOURCE_NAME} ${OUTPUT_FILE}")
    execute_process(
                COMMAND ${CMAKE_COMMAND} -E create_symlink 
                        ${SOURCE_DIR}/${SOURCE_NAME} ${OUTPUT_DIR}/${OUTPUT_FILE}
                WORKING_DIRECTORY ${SOURCE_DIR}
            )
endfunction()
